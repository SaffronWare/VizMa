#pragma once
#include "ArkMat.hpp"
#include <SDL3/SDL.h>

#include <iostream>

using ark::Vec4;

namespace vzm {
	

	template <typename DerivedProject> 
	struct Project
	{
	private:
		unsigned int window_width = 0u;
		unsigned int window_height = 0u;
		unsigned float aspect_ratio = 0.0f;
		float dt = 0.0f;
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
			Uint32 out;
			out |= static_cast<Uint32>(color.x * 255.0f) << 16;
			out |= static_cast<Uint32>(color.y * 255.0f) << 8;
			out |= static_cast<Uint32>(color.z * 255.0f);

			return out;
		}

		Vec4 raymarch_pixel(unsigned int x, unsigned int y, Vec4& fragColor)
		{
			Vec4 uv = Vec4(x, y) / (float)window_height;
			fragColor = uv;
			fragColor = Vec4(0.0f, 1.0f, 1.0f);
				
			return fragColor;
			
		}

		void start(unsigned int window_width = 1000, unsigned int window_height = 800)
		{
			this->window_width = window_width;
			this->window_height = window_height;
			this->aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);

			SDL_Init(SDL_INIT_VIDEO);

			window = SDL_CreateWindow(label, window_width, window_height, 0);
			window_surface = SDL_GetWindowSurface(window);

			DerivedProject* imp_this = static_cast<DerivedProject*>(this);
			unsigned int pixel_pitch = window_surface->pitch / sizeof(Uint32);
		
			Uint32* pixel_buffer = static_cast<Uint32*>(window_surface->pixels);
			Uint32* buffer_position;

			
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

				buffer_position = pixel_buffer;

				for (int j = 0; j < window_height; j++)
				{
					for (int i = 0; i < window_width; i++)
					{
						Vec4 pixel_color;

						raymarch_pixel(i, j, pixel_color);
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