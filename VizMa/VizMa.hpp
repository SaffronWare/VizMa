#pragma once
#include "ArkMat.hpp"
#include <SDL3/SDL.h>



#include <iostream>

using ark::Vec4;


constexpr float epsilon = 0.0001f;

namespace vzm {
	
	struct SDFValue
	{
		Vec4 color;
		float dist;
	};

	template <typename DerivedProject> 
	struct Project
	{
	private:
		unsigned int window_width = 0u;
		unsigned int window_height = 0u;
		float aspect_ratio = 0.0f;
		float dt = 0.0f;

		DerivedProject* imp_this;
		float time = 0.0f; 

		SDL_Window* window = nullptr;
		SDL_Surface* window_surface = nullptr; 

		unsigned int flags = 0;
		unsigned int frame = 0;
		const char* label;
		

		

		
	public:
	

		Project(const char* label = "misc", unsigned int flags = 0)
			: label(label), flags(flags)
		{
		}

		~Project()
		{
			SDL_DestroyWindow(window);
			SDL_Quit();
			window = nullptr;
			window_surface = nullptr;
		}

		Uint32 convert_color(const ark::Vec4& color)
		{
			Uint32 out = 0;
			
			out |= static_cast<Uint32>(255) << 24;
			out |= static_cast<Uint32>(std::fmaxf(0.0f, std::fminf(color.x * 255.0f, 255.0f))) << 16;
			out |= static_cast<Uint32>(std::fmaxf(0.0f, std::fminf(color.y * 255.0f, 255.0f))) << 8;
			out |= static_cast<Uint32>(std::fmaxf(0.0f, std::fminf(color.z * 255.0f, 255.0f)));

			return out;
		}

		Vec4 normal_at(Vec4 pos)
		{
			return ((
				Vec4(imp_this->scene(pos + Vec4(epsilon, 0.0f)), 
					imp_this->scene(pos + Vec4(0.0f, epsilon)), 
					imp_this->scene(pos + Vec4(0.0f, 0.0f, epsilon )),
					0.0f)
						 - Vec4(imp_this->scene(pos), 
							 imp_this->scene(pos), 
							 imp_this->scene(pos),
							 0.0f)
				)/epsilon
			).normalized();
		}

		Vec4 shade(Vec4 normal)

		{
			Vec4 light_dir = Vec4(0.0f, -0.0f, 1.0f, 0.0f).normalized();
			float v = std::fminf(0.2f,1.0f*fdot(normal, light_dir));
			//return Vec4(1.0f);
			return Vec4(v,v,v,1.0f);
		}

		Vec4 raymarch_pixel(unsigned int x, unsigned int y, Vec4& fragColor, const Vec4& window_dims, const Vec4& aspect_ratio_as_x)
		{
			
			Vec4 camera_pos = Vec4(0.0f, 0.0f, -4.0f);

			Vec4 uv = (Vec4(x, y) - (Vec4(0.5f) * window_dims)) / Vec4(window_dims.x, window_dims.y, 1.0f, 1.0f) * aspect_ratio_as_x;
			float view_dist = 1.0f;
			Vec4 pixel_coord = camera_pos + uv + Vec4(0.0f, 0.0f, view_dist);

			float f_value;
			float s_value = 100.0f;
			bool hit = false;
			Vec4 ray_dir = (pixel_coord - camera_pos).normalized();
			Vec4 ray_pos = camera_pos;
			
			for (int i = 0; i <= 50; i++)
			{
				s_value = imp_this->scene(ray_pos);
				if (s_value <= 0.0001f)
				{
					hit = true;
					break;
				}
				//std::cout << s_value << std::endl;
				ray_pos += ray_dir * s_value*0.99f;
			}
			fragColor = ray_dir;
			if (hit)
			{
				fragColor = shade(normal_at(ray_pos));
			}

			return fragColor;
			
		}

		void start(unsigned int window_width = 1000, unsigned int window_height = 800)
		{
			this->window_width = window_width;
			this->window_height = window_height;
			this->aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
			Vec4 window_dims = Vec4(static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, 0.0f);
			Vec4 aspect_as_x = Vec4(aspect_ratio, 1.0f, 0.0f, 0.0f);

			SDL_Init(SDL_INIT_VIDEO);

			window = SDL_CreateWindow(label, window_width, window_height, 0);
			window_surface = SDL_GetWindowSurface(window);

			imp_this = static_cast<DerivedProject*>(this);
			unsigned int pixel_pitch = window_surface->pitch / sizeof(Uint32);
		
			Uint32* buffer_position;
			Vec4 pixel_color;
			
			SDL_Event event;
			while (true)
			{
				while (SDL_PollEvent(&event))
				{
					if (event.type == SDL_EVENT_QUIT)
					{
						return;
					}
				}

				SDL_LockSurface(window_surface);

				Uint32* pixel_buffer = static_cast<Uint32*>(window_surface->pixels);

				buffer_position = pixel_buffer;

				for (int j = 0; j < window_height; j++)
				{
					for (int i = 0; i < window_width; i++)
					{

						raymarch_pixel(i, j, pixel_color, window_dims, aspect_as_x);
						buffer_position[i] = convert_color(pixel_color);
						
					}

					buffer_position += pixel_pitch;
				}
				
				SDL_UnlockSurface(window_surface);

				SDL_UpdateWindowSurface(window);

				frame++;
			}
		}
	};

}