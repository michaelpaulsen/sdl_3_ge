#pragma once
#include <numbers>
#include "./perlin.hpp"
namespace SKC::Math {
	double map(double t, double min, double max, double to_min, double to_max) {
		return (t - min) / (max - min) * (to_max - to_min) + to_min;
	}
	double clamped_map(double t, double min, double max, double to_min, double to_max) {
		if (t < min) return to_min; 
		if (t > max) return to_max; 
		return (t - min) / (max - min) * (to_max - to_min) + to_min;
	}
	double cubic_interp(double x, double y, double z,double w,  double t) {
		double P = (z - w) - (x - y); 
		double Q = (x - y) - P; 
		double R = (z - x); 
		double S = y; 
		return P * (t * t * t) + Q * (t * t) + S; 
	}
	double noise2d(double x, double y) {
		
		uint32_t n = x + y * 57; 
		//uint32_t tn = n; 
		n = pow(n<<13,n);
		return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

	}
	double smooth_noise2d(double x, double y) {
		for (int xoff = -1; xoff <= 1; ++xoff) {
			double crn = 0; 
			double edge = 0;
			double center = 0; 

			if (x == 0 && xoff < 0) continue; 
			for (int yoff = -1; yoff <= 1; ++yoff) {
				
				if (y == 0 && yoff < 0) continue;
				auto val = noise2d(x + xoff, y + yoff);
				if (xoff == 0 && yoff == 0) {
					center = val; 
				}
				if (xoff == 0 || yoff == 0) {
					edge += val;
					continue; 
				}
				crn += val; 
			}
			auto ret = crn / 16 + edge / 8 + center / 4; 
			return ret;
		}
		
	}
}