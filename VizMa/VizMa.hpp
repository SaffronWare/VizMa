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
		float time = 0.0f; 

		SDL_Window* window = nullptr;
		SDL_Surface* window_surface = nullptr; // 2nd 16 byte chunk

		unsigned int flags;
		unsigned int frame;
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

		void start(unsigned int window_width = 1000, unsigned int window_height = 800)
		{

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
						
						buffer_position[i] = imp_this->render_pixel(i, j);
						
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