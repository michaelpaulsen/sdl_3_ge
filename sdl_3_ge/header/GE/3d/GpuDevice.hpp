#pragma once 
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <iterator>
#include <fstream>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_properties.h>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <format>
namespace SKC::GE::s3d {
#ifdef _DEBUG
	static constexpr bool DEBUG = true; 
#else 
	static constexpr bool DEBUG = false;
#endif

#ifndef SHADER_SLOTS 
	static constexpr size_t SHADER_SLOTS = 10;
#endif
	struct vertex {
		SKC::Math::Vect3<float> pos; 
		SKC::Math::Vect3<float> color;
		SKC::Math::Vect2f UV; 
	};
	constexpr size_t INVALID_SLOT = -1; 
	class gpu_device {
		SDL_GPUDevice* m_device ;
		struct shader_info_t {
			SDL_GPUShader* shader = nullptr;
			SDL_GPUShaderFormat format =0; 
			SDL_GPUShaderStage stage; 
		} m_shaders[SHADER_SLOTS]{ nullptr };
		
		//SDL_GPUShader* m_shaders[SHADER_SLOTS]{ nullptr };
		SDL_GPUShaderFormat m_format; 
		size_t m_slot = 0; 
		SDL_GPUGraphicsPipeline* m_pipeline = nullptr; 
	public: 
		
		//TODO(skc) :make this not take a char* because that will case memory issues ... 
		gpu_device(
			SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_SPIRV, 
			bool debug = DEBUG,
			const char* name = nullptr
		) {
			m_format = format; 
			m_device = SDL_CreateGPUDevice(
				format,
				debug,
				name
			); 
			std::println("[ERROR] {}", SDL_GetError());
			if (!m_device) {
			}
		}

		gpu_device(const gpu_device&) = delete; 

		gpu_device(gpu_device&& o) {
			this->~gpu_device(); 
			this->m_device = o.m_device; 
			o.~gpu_device(); 
		}
		void operator =(gpu_device&) = delete; 
		gpu_device& operator =(gpu_device&& o) noexcept {
			if (this != &o) { 
				this->~gpu_device(); 
				m_device = o.m_device; 
				m_format = o.m_format; 
				for (size_t i = 0; i < o.m_slot; ++i) {
					m_shaders[i] = o.m_shaders[i]; 
					o.m_shaders[i] = { 0 }; 
				}
			}
			return *this; 
		}

		~gpu_device() {
			for (size_t slot = 0; slot < m_slot; ++slot) {
				auto shader = m_shaders[slot];
				if (shader.shader) {
					SDL_ReleaseGPUShader(m_device, shader.shader);
				}
			}
			SDL_DestroyGPUDevice(m_device); 
		}
		auto bind_to_window(SDL_Window* window) {
			return SDL_ClaimWindowForGPUDevice(m_device, window); 
		}
		auto unbind_from_window(SDL_Window* window) {
			return SDL_ReleaseWindowFromGPUDevice(m_device, window);
		}
		auto make_shader(
			std::filesystem::path path,
			SDL_GPUShaderStage stage,
			Uint32 format, 
			Uint32 num_storage_buffers = 0,
			Uint32 num_samplers = 0,
			Uint32 num_storage_textures = 0,
			Uint32 num_uniform_buffers = 0,
			SDL_PropertiesID props = 0
		) noexcept {
			auto input = std::ifstream(path, std::ios::binary);
			std::println("[LOG] {}/{} SHADERS USED", m_slot+1, SHADER_SLOTS);

			if (m_slot >= SHADER_SLOTS) {
				std::println("[ERROR] TOO MANY SHADERS"); 
				return ULLONG_MAX;


			}
			if (!input.good()) {
				std::println("ERROR UNABLE TO READ SHADER FILE {}", std::filesystem::absolute(path).string());
				return ULLONG_MAX;

			}
			auto contents = std::vector<uint8_t>((std::istreambuf_iterator<char>(input)), {});
			input.close();
			SDL_GPUShaderCreateInfo info = {
				.code_size = contents.size(),
				.code = contents.data(),
				.entrypoint = "main",
				.format = format,
				.stage = stage,
				.num_samplers = num_samplers,
				.num_storage_textures = num_storage_textures,
				.num_storage_buffers = num_storage_buffers,
				.num_uniform_buffers = num_uniform_buffers,
				.props = props

			};
			
			m_shaders[m_slot] = {
				.shader = SDL_CreateGPUShader(m_device, &info),
				.format = format,
				.stage = stage
				};
			return m_slot++; 
		}
		auto get_shader(size_t idx) const {
			if (idx >= m_slot) {
				throw std::runtime_error(std::format("[ERROR] Shader in slot {} is not initilized\nmax slot {}", idx, SHADER_SLOTS));
			}
			return m_shaders[idx]; 
		}
		
	};
}