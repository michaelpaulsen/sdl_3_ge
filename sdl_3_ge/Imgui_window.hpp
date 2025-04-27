#pragma once
#include "Window.hpp"


#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

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
			m_io = ImGui::GetIO();
			m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();

			// Setup Platform/Renderer backends
			ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
			ImGui_ImplSDLRenderer3_Init(m_renderer);
		}
		~imgui_window() {
			if (--imgui_count == 0) {
				ImGui_ImplSDLRenderer3_Shutdown();
				ImGui_ImplSDL3_Shutdown();
				ImGui::DestroyContext();
			}
		}
		void start_frame() {
			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
		}
		void draw_imgui_data() {

			ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
		}
		void render() {
			ImGui::Render();
		}
		auto& imgui_io() {
			return m_io; 
		}
	};

}