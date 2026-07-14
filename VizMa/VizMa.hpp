#pragma once
#include "ArkMat.hpp"
#include <omp.h>
#include <SDL3/SDL.h>
#include <iostream>

using ark::Vec4;
using ark::Vec3;

constexpr float pi = 3.14159265358979323846f;


// for speed when calculating normal
constexpr float epsilon = 0.0001f;
constexpr float hit_zero = 0.001f;
constexpr float max_dist = 1000.0f;
constexpr int max_num_marches = 10000;
const Vec3 epsilon_vec = Vec3(epsilon);
const Vec3 inv_epsilon_vec = Vec3(1.0f / epsilon);
const Vec3 epsilon_x = Vec3(epsilon, 0.0f);
const Vec3 epsilon_y = Vec3(0.0f, epsilon);
const Vec3 epsilon_z = Vec3(0.0f, 0.0f, epsilon);

const Vec3 i = Vec3(1.0f, 0.0f);
const Vec3 j = Vec3(0.0f, 1.0f);
const Vec3 k = Vec3(0.0f, 0.0f, 1.0f);


namespace vzm {
	
	struct SDFValue
	{
		Vec4 color;
		float dist = max_dist;
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

		Uint32 convert_color(ark::Vec4 color)
		{
			Uint32 out = 0;
			
			out |= static_cast<Uint32>(255) << 24;
			out |= static_cast<Uint32>(std::fmaxf(0.0f, std::fminf(color.x * 255.0f, 255.0f))) << 16;
			out |= static_cast<Uint32>(std::fmaxf(0.0f, std::fminf(color.y * 255.0f, 255.0f))) << 8;
			out |= static_cast<Uint32>(std::fmaxf(0.0f, std::fminf(color.z * 255.0f, 255.0f)));

			return out;
		}

		Vec3 normal_at(Vec3 pos)
		{
			return ((
				Vec3(imp_this->scene(pos + epsilon_x).dist,
					imp_this->scene(pos + epsilon_y).dist,
					imp_this->scene(pos + epsilon_z).dist)
				- Vec3(imp_this->scene(pos).dist,
					imp_this->scene(pos).dist,
					imp_this->scene(pos).dist)
				) * inv_epsilon_vec
			).normalized();
		}

		Vec4 shade(Vec4 color, Vec3 normal)
		{
			Vec3 light_dir = Vec3(.5f, -5.0f, -1.0f).normalized();

			float v = std::fmaxf(0.2f,1.0f*fdot(normal, light_dir));
			//return Vec4(1.0f);
			return color * Vec4(v,v,v, 1.0f);
		}

		Vec4 raymarch_point(Vec3 uv_point)
		{
			Vec4 fragColor;
			
			static const Vec3 camera_pos = Vec3(0.0f, 0.0f, -4.0f);
			static const float view_dist = 1.0f;

			Vec3 pixel_coord = camera_pos + uv_point + Vec3(0.0f, 0.0f, view_dist);

			float f_value;
			float s_value = 10.0f;
			bool hit = false;
			Vec3 ray_dir = (pixel_coord - camera_pos).normalized();
			Vec3 ray_pos = camera_pos;
			
			for (int i = 0; i < max_num_marches; i++)
			{
				s_value = imp_this->scene(ray_pos).dist;
				if (s_value <= hit_zero)
				{
					hit = true;
					break;
				}
				else if (s_value >= max_dist)
				{
					break;
				}
				//std::cout << s_value << std::endl;
				ray_pos += ray_dir * s_value*0.999f;
			}
		
			if (!hit)
			{
				fragColor = imp_this->sky(ray_dir);
			}
			else if (hit)
			{
				fragColor = shade(imp_this->scene(ray_pos).color, normal_at(ray_pos));
			}

			return fragColor;
			
		}

		void start(unsigned int window_width = 1000, unsigned int window_height = 800)
		{
			this->window_width = window_width;
			this->window_height = window_height;
			this->aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
			
			const float fwindow_width = static_cast<float>(this->window_width);
			const float fwindow_height = static_cast<float> (this->window_height);
			const float pixel_uv_spacing = 1 /fwindow_height;

			const float start_uv_x = -0.5f * aspect_ratio;
			const float start_uv_y = -0.5f;

			SDL_Init(SDL_INIT_VIDEO);

			window = SDL_CreateWindow(label, window_width, window_height, 0);
			window_surface = SDL_GetWindowSurface(window);

			imp_this = static_cast<DerivedProject*>(this);
			unsigned int pixel_pitch = window_surface->pitch / sizeof(Uint32);
		
			Uint32* buffer_position;
			Vec4 pixel_color;

			Uint64 last_frame_time = SDL_GetPerformanceCounter();
			Uint64 current_frame_time = SDL_GetPerformanceCounter();
			unsigned int acc_frames = 0;
			omp_set_num_threads(omp_get_num_procs());

			SDL_Event event;
			while (true)
			{
				
				current_frame_time = SDL_GetPerformanceCounter();
				if (current_frame_time - last_frame_time > 5 * SDL_GetPerformanceFrequency())
				{
					std::cout << acc_frames / 5 << " fps" << std::endl;
					last_frame_time = current_frame_time;
					acc_frames = 0;
				}
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
				#pragma omp parallel for schedule(dynamic, 2)
				for (int j = 0; j < static_cast<int>(window_height); j++)
				{
					for (int i = 0; i < static_cast<int>(window_width); i++)
					{
						Vec3 pixel_uv = Vec3(start_uv_x + pixel_uv_spacing * i, start_uv_y + pixel_uv_spacing * j, 0.0f);
						Vec4 cumsum = Vec4(0.0f);
						for (int k = 0; k < 1; k++)
						{
							cumsum += raymarch_point(pixel_uv + ark::v3uniform(i * 1236123 + j * 987644 + 98476 * frame) * pixel_uv_spacing);
						}
						buffer_position[i + pixel_pitch * j] = convert_color(cumsum);
						//buffer_position[i + pixel_pitch * j] = convert_color(Vec4(1.0f));
					}
				}
				
				SDL_UnlockSurface(window_surface);

				SDL_UpdateWindowSurface(window);

				frame++;
				acc_frames++;
			}
		}
	};

}