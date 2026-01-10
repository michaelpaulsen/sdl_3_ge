#pragma once
#include "Window.hpp"


#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "../math/vector.hpp"


namespace SKC::GE {
	class imgui_window : public window {
		static inline size_t imgui_count = false;
		ImGuiIO m_io;
	public: 
		imgui_window(std::string title, int  width, int height, SDL_WindowFlags flags) :
			window(title, width, height, flags), m_io{}
		{
			if (imgui_count == 0) {
				ImGui::CreateContext();	
			}
			++imgui_count; 
			
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();

			// Setup Platform/Renderer backends
			ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
			ImGui_ImplSDLRenderer3_Init(m_renderer);
			m_io = ImGui::GetIO();
			m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			m_io.DisplaySize = ImVec2((float)width, (float)height);
			ImGui::StyleColorsDark();
		}
		~imgui_window() {
			if (--imgui_count == 0) {
				ImGui_ImplSDLRenderer3_Shutdown();
				ImGui_ImplSDL3_Shutdown();
				ImGui::DestroyContext();
			}
		}
		void im_start_frame() {
			// Start the Dear ImGui frame
		
				ImGui_ImplSDLRenderer3_NewFrame();
				ImGui_ImplSDL3_NewFrame();
				ImGui::NewFrame();
				auto ws = (SKC::Math::Vect2d)get_window_dimentions(); 
				//SDL_setwind
		
		}
	
		void update_io_display_size() {
			int width{}, height{};
			get_window_dimentions(width, height);
			m_io.DisplaySize = ImVec2((float)width, (float)height);
		}
		void render() {
			if (ImGui::GetCurrentContext() &&
				m_io.DisplaySize.x > 0.0f && m_io.DisplaySize.y > 0.0f) {

				//ImGui::End();
				ImGui::Render();
				ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);

			}
		}
		auto& imgui_io() {
			return m_io; 
		}
	};

}