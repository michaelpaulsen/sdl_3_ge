#pragma once 
#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <vector>
#include "../concepts.hpp" 
#include "../math/Vector.hpp"
#undef WIDTH 
#undef HEIGHT 
namespace SKC::GE {
	using namespace concepts; 
	
	template <enum_or_integral tiles/*, uint16_t WIDTH, uint16_t HEIGHT*/>
	class tile_map {
		using tile_size_t = uint16_t;
		using tile_list_t = std::vector<tiles>; 
		//static constexpr  size_t els = (HEIGHT * WIDTH) + 1;
		//static constexpr uint16_t W = WIDTH, H = HEIGHT;
		size_t m_tiles_x_count, m_tiles_y_count, m_tile_count; 
		tile_list_t m_tiles; 
		//tiles m_tiles[els];
		tile_size_t m_tile_size{};
		SKC::Math::Vect2d m_offset{0,0};
	public:
		using type = tiles; 
		tile_map(size_t width, size_t height, tiles default_value, tile_size_t size) {
			if (width == 0 || height == 0) {
				//this may not be the best way to do this. 
				throw "invalid tile map dimensions "; 
			}
			m_tile_size = size; 
			m_tiles_x_count = width;
			m_tiles_y_count = height; 
			m_tile_count = width * height; 
		
			m_tiles.reserve(m_tile_count);
			for (int x = 0; x < m_tile_count; ++x) {
				m_tiles.push_back(default_value);
			}
		}
		void fill(tiles tile ) {
			for (auto& _tile : m_tiles) {
				_tile = tile; 
			}

		}

		SKC::Math::Vect2i to_tile_space(SKC::Math::Vect2i pos) const {
			auto ppos = pos / m_tile_size; 
			return ppos + m_offset; 
		}


		SKC::Math::Vect2i to_tile_space(int x, int y) const {
			return to_tile_space({ x,y }); 
		}
		
		skc_Vect2f to_screen_space(int x, int y) const {
			skc_Vect2f pos = skc_Vect2f((float)x, (float)y);
			pos *= m_tile_size; 
			pos += m_offset; 
			return pos; 
		}
		auto tile_size() const noexcept {
			return m_tile_size;
		}
		 
		
		void tile_size_from_window_size(SKC::Math::Vect2i window_size) noexcept {
			auto min_size = std::min(window_size.x, window_size.y); 
			
			if (window_size.x == min_size) {
				m_tile_size = min_size / m_tiles_x_count; 
			}
			else {
				m_tile_size = min_size / m_tiles_y_count;
			}
		}
		void set_tile_size(tile_size_t size)  noexcept{
			m_tile_size = size; 
		}
		void offset_px(Math::Vect2d offset) {
			m_offset += offset; 
		}
		
		SKC::Math::Vect2<size_t> size_px() const noexcept {
			return {
				m_tiles_x_count * m_tile_size, m_tiles_y_count * m_tile_size
			};
		}
		SKC::Math::Vect2d get_offset() const noexcept {
			return m_offset; 
		}
		tiles& at(size_t x, size_t y){
			return m_tiles.at(y * m_tiles_x_count + x);
		}

		tiles& operator[](size_t x, size_t y) {
			return at(x, y);
		}

	};
}