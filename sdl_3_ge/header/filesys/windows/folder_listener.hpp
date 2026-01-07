
#pragma once
//NOTE(skc): this seems to be the best (and most portable) way of checking for platform 
//headers (as __has_include was included in the standard to do just that)
//in the Uinix versions of this we may need to do more work to decide what flavor of unix like 
//kernel we have (because BSD and Linux do some things differenetly) 
// but I do not have a way of testing this on Linux/BSD so that is TBD ... 

#if !__has_include(<Windows.h>)
#error attempting to include windows files when the windows headers are not present... 
#endif 
#ifndef NOMINMAX 
#pragma warn including window's min and max functions
#endif // !NOMINMAX 

#include <Windows.h> 
#include <filesystem>
#include <functional>
#include <string>

#include "../file_change_info.hpp"
#include <cstdint>

namespace SKC::file_api {
	//NOTE(skc) : this is meant to be used in if constexpr statements to take into account the differences between 
	// the UNIX and windows file system APIs. 
	enum FILE_SYS_API {
		WINODOWS = 0, 
		LINUX,
		POSIX_OTHER,
		CURRENT = WINODOWS
	};
	//NOTE(skc) : this is here to abstract away the fact that the values for these
	//are not the same on every platform. 
	//this should be defined in every implementation of this libary.
	enum FILE_ACTION : uint64_t {
		FA_ADDED    = FILE_ACTION_ADDED           ,
		FA_MODIFIED = FILE_ACTION_REMOVED         ,
		FA_REMOVED	= FILE_ACTION_MODIFIED        ,
		FA_RENAMED  = FILE_ACTION_RENAMED_NEW_NAME
	};

	namespace win32_priv_ {
		
		
		void print_error_string() {
			LPSTR messageBuffer = nullptr;
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&messageBuffer,
				0,
				NULL
			);
			std::println("last error {}", messageBuffer);
			free(messageBuffer); 
		}
		
		
		std::string ws2s(const std::wstring& wstr) {
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), strTo.data(), size_needed, NULL, NULL);
			return strTo;
		}
		auto create_dir_handle(std::filesystem::path directory_path) {



			HANDLE hDir = CreateFileW(
				directory_path.c_str(), // Replace with your directory
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL
			);

			if (hDir == INVALID_HANDLE_VALUE) {
				std::print(stderr, "Failed to open directory. Error: {}", GetLastError());
				exit(1);
			}
			return hDir;
		}
	}

	//this is constexpr so that we can use it do default a paramiter 
	static constexpr DWORD defnotfilt =
		FILE_NOTIFY_CHANGE_CREATION |
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE;
	using fs_callback_fntn_t = std::function<void(fs_change_info_t cinfo)>;
	
	
	

	void watch_directory(
		std::stop_token st,
		std::filesystem::path directory_path,
		fs_callback_fntn_t call_back,
		DWORD notf_filter = defnotfilt,
		bool watch_subdirs = true
	) {

		auto hDir = win32_priv_::create_dir_handle(directory_path);
		DWORD bytesReturned;
		OVERLAPPED overlapped = { 0 };
		overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual-reset event
		
		if (overlapped.hEvent == NULL) {
			std::print(stderr,"Failed to create event. Error: {} ", GetLastError());
			CloseHandle(hDir);
			exit(-1);
		}
		std::vector<BYTE> buffer;
		buffer.resize(1024 * 8);

		while (!st.stop_requested()) {
			
			BOOL result = ReadDirectoryChangesW(
				hDir,
				buffer.data(),
				(DWORD)buffer.size(),
				watch_subdirs,
				notf_filter,
				&bytesReturned,
				&overlapped,
				NULL
			);
			
			if (!result && GetLastError() != ERROR_IO_PENDING) {
				std::println(stderr, "ReadDirectoryChangesW failed. Error: {} ", GetLastError());
				win32_priv_::print_error_string(); 
				CloseHandle(overlapped.hEvent);
				CloseHandle(hDir);
				return;
			}

			DWORD waitResult = WaitForSingleObject(overlapped.hEvent, 200);
			std::string old_path{}; 
			if (waitResult == WAIT_OBJECT_0) {
				if (GetOverlappedResult(hDir, &overlapped, &bytesReturned, TRUE)) {
					FILE_NOTIFY_INFORMATION *notification = (FILE_NOTIFY_INFORMATION*)buffer.data();
					if (bytesReturned == 0) {
						win32_priv_::print_error_string();
					}
					do {
						auto action = notification->Action; 

						auto fpath = win32_priv_::ws2s(notification->FileName);
						
						switch (action) {
							case FILE_ACTION_ADDED: 
							case FILE_ACTION_REMOVED:
							case FILE_ACTION_MODIFIED:
							{
								call_back({ fpath, "", (FILE_ACTION)action }); 
								break; 
							}
							case FILE_ACTION_RENAMED_OLD_NAME: {
								old_path = fpath;
								break; 
							}
							case FILE_ACTION_RENAMED_NEW_NAME: {
								call_back({ fpath, old_path, (FILE_ACTION)action });
								old_path.clear(); 
								break; 
							}
						}



						notification = (FILE_NOTIFY_INFORMATION*)((BYTE*)notification + 
							notification->NextEntryOffset);
					} while ((notification->NextEntryOffset) != 0); 
				}
			}
			else if (waitResult == WAIT_TIMEOUT) {
				CancelIo(hDir);
			}
			/*while (!st.stop_requested()) {*/
		}
		CloseHandle(overlapped.hEvent);
		CloseHandle(hDir);

	} 
}