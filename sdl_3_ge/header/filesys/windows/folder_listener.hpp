
#pragma once
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

namespace SKC::file_api {
	
	enum FILE_SYS_API {
		WINODOWS = 0, 
		LINUX,
		POSIX_OTHER,
		CURRENT = WINODOWS
	};


	namespace _win32_priv {
		
		
		void print_error_string() {
			LPSTR messageBuffer = nullptr;
			DWORD charsCopied = FormatMessageA(
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
	}

	static constexpr DWORD defnotfilt =
		FILE_NOTIFY_CHANGE_CREATION |
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE;
	
	
	
	//TODO(skc): make it so that this can be abstracted to other platforms... 
	struct fs_change_info_t {
		std::filesystem::path fname{}, old_name{};
		DWORD action;
		std::string get_event_type_as_string() const {
			switch (action) {
			case FILE_ACTION_ADDED:
				return "FILE_ACTION_ADDED";
			case FILE_ACTION_REMOVED:
				return "FILE_ACTION_REMOVED";
			case FILE_ACTION_MODIFIED:
				return "FILE_ACTION_MODIFIED";
			case FILE_ACTION_RENAMED_OLD_NAME:
				return "RENAMED";
			case FILE_ACTION_RENAMED_NEW_NAME:
				return "RENAMED";
			default:
				return "UNKNOWN FILE ACTION";
			}
		}
		std::string to_string() const{
			std::string re = fname.generic_string();
			re += " had a "; 
			re += get_event_type_as_string(); 
			return re + " event";
		}
	};
	
	using fs_callback_fntn_t = std::function<void(fs_change_info_t cinfo)>;
	
	
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
			std::cerr << "Failed to open directory. Error: " << GetLastError() << std::endl;
			exit(1);
		}
		return hDir; 
	}

	void watch_directory(
		std::stop_token st,
		std::filesystem::path directory_path,
		fs_callback_fntn_t call_back,
		DWORD notf_filter = defnotfilt,
		bool watch_subdirs = true
	) {

		auto hDir = create_dir_handle(directory_path);
		DWORD bytesReturned;
		OVERLAPPED overlapped = { 0 };
		overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual-reset event
		
		if (overlapped.hEvent == NULL) {
			std::cerr << "Failed to create event. Error: " << GetLastError() << std::endl;
			CloseHandle(hDir);
			exit(-1);
		}
		std::vector<BYTE> buffer;
		buffer.resize(1024 * 8);

		while (!st.stop_requested()) {
			
			BOOL result = ReadDirectoryChangesW(
				hDir,
				buffer.data(),
				buffer.size(),
				watch_subdirs,
				notf_filter,
				&bytesReturned,
				&overlapped,
				NULL
			);
			
			if (!result && GetLastError() != ERROR_IO_PENDING) {
				std::cerr << "ReadDirectoryChangesW failed. Error: " << GetLastError() << std::endl;
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
						_win32_priv::print_error_string(); 
					}
					do {
						auto action = notification->Action; 

						auto fpath = _win32_priv::ws2s(notification->FileName);
						
						switch (action) {
							case FILE_ACTION_ADDED: 
							case FILE_ACTION_REMOVED:
							case FILE_ACTION_MODIFIED:
							{
								call_back({ fpath, "", action }); 
								break; 
							}
							case FILE_ACTION_RENAMED_OLD_NAME: {
								old_path = fpath;
								break; 
							}
							case FILE_ACTION_RENAMED_NEW_NAME: {
								call_back({ fpath, old_path, action });
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