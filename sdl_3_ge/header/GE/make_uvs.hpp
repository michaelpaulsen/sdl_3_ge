#pragma once
#include "../geo/common.hpp"
#include "../math/Vector.hpp"
#include "./create_vertex_list.hpp"

#include <SDL3/SDL.h>
namespace SKC::GE {


	skc_Vect2f calc_min_position(const Geo::vect_list& points) {
		if (points.size() == 0) return { 0,0};
		float minx = points[0].x,
			miny = points[0].y;

		for (const auto& point : points) {
			if (point.x < minx) minx = point.x;
			if (point.y < miny) miny = point.y;
		}
		return { minx, miny};
	}

	void set_UVs(
		vertex_list_t& vert_list,
		skc_Vect2f bounds,
		skc_Vect2f texture_size,
		skc_Vect2f uv_offset = { 0,0 },
		float scale = 1,
		bool clamp = false
	) {
		const auto width = texture_size.x, height = texture_size.y;
		for (auto& vert : vert_list) {
			auto offset_x = vert.position.x - bounds.x;
			auto offset_y = vert.position.y - bounds.y;
			auto uu = SKC::Math::map(offset_x, 0, width, 0, scale);
			auto vv = SKC::Math::map(offset_y, 0, height, 0, scale);

			uu += uv_offset.x;
			vv += uv_offset.y;

			if (clamp) {
				if (uu > 1) uu = 1;
				if (uu < 0) uu = 0;
				if (vv > 1) vv = 1;
				if (vv < 0) vv = 0;
			}
			vert.tex_coord.x = uu;
			vert.tex_coord.y = vv;
 
		}
	}
}