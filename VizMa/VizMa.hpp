#pragma once
#include "ArkMat.hpp"
#include <SDL3/SDL.h>

#include <iostream>

namespace vzm {
	

	template <typename DerivedProject> // static virtual struct
	struct Project
	{
	private:
		unsigned int window_size = 0u;
		unsigned int window_height = 0u;
		float dt = 0.0f;
		float time = 0.0f; // 1st 16 byte chunk of memory

		SDL_Window* window = nullptr;
		SDL_Surface* window_surface = nullptr; // 2nd 16 byte chunk

		const char* label = "";
		SDL_PixelFormat* format = nullptr;
		

		SDL_PixelFormatData* format_data = nullptr;
		unsigned int flags;
		unsigned int frame;
		
	public:
	

		Project(const char* label = "misc", unsigned int flags = 0)
			: label(label), flags(flags)
		{
		}

		~Project()
		{
			SDL_DestroyWindow(window);
			window = nullptr;
			window_surface = nullptr;
		}

		void start(unsigned int window_width = 1000, unsigned int window_height = 800)
		{

			window = SDL_CreateWindow(label, window_width, window_height, 0);

			SDL_WINDOW_RG

			SDL_Surface* window_surface = SDL_GetWindowSurface(window);

			DerivedProject* imp_this = static_cast<DerivedProject*>(this);

			SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(SDL_GetWindowPixelFormat(window));

			unsigned int pixel_stride = format->bits_per_pixel;

			std::cout << format->size << std::endl;
			while (true)
			{

				
				SDL_LockSurface(window_surface);

				// ignore for now just to test template
				float* 
				for (int i = 0; i < window_width; i++)
				{
					for (int j = 0; j < window_height; j++)
					{


						imp_this->render_pixel(i, j);
					}

				}
				
				SDL_UnlockSurface(window_surface);

				frame++;
			}
		}
	};

}