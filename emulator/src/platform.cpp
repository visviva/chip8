/// MIT License
///
/// Copyright(c) 2023 Simon Lauser
///
/// Permission is hereby granted,
/// free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"),
/// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
/// publish, distribute, sublicense, and / or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so,
/// subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in all copies
/// or
/// substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS",
/// WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND
/// NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "platform.h"

#include <Windows.h>

using namespace chip8;

Platform::Platform(
    char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight, int cycleTime)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);

    beepThread = std::thread([this, cycleTime]() {
        while (true)
        {
            if (beeping)
                Beep(440, cycleTime);
            if (close)
                return;
        }
    });
}

Platform::~Platform()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    close = true;
    beepThread.join();
}

void Platform::Update(void const* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys)
{
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT: {
                quit = true;
            }
            break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: {
                        quit = true;
                    }
                    break;

                    case SDLK_x: {
                        keys[0] = 1;
                    }
                    break;

                    case SDLK_1: {
                        keys[1] = 1;
                    }
                    break;

                    case SDLK_2: {
                        keys[2] = 1;
                    }
                    break;

                    case SDLK_3: {
                        keys[3] = 1;
                    }
                    break;

                    case SDLK_q: {
                        keys[4] = 1;
                    }
                    break;

                    case SDLK_w: {
                        keys[5] = 1;
                    }
                    break;

                    case SDLK_e: {
                        keys[6] = 1;
                    }
                    break;

                    case SDLK_a: {
                        keys[7] = 1;
                    }
                    break;

                    case SDLK_s: {
                        keys[8] = 1;
                    }
                    break;

                    case SDLK_d: {
                        keys[9] = 1;
                    }
                    break;

                    case SDLK_y: {
                        keys[0xA] = 1;
                    }
                    break;

                    case SDLK_c: {
                        keys[0xB] = 1;
                    }
                    break;

                    case SDLK_4: {
                        keys[0xC] = 1;
                    }
                    break;

                    case SDLK_r: {
                        keys[0xD] = 1;
                    }
                    break;

                    case SDLK_f: {
                        keys[0xE] = 1;
                    }
                    break;

                    case SDLK_v: {
                        keys[0xF] = 1;
                    }
                    break;
                }
            }
            break;

            case SDL_KEYUP: {
                switch (event.key.keysym.sym)
                {
                    case SDLK_x: {
                        keys[0] = 0;
                    }
                    break;

                    case SDLK_1: {
                        keys[1] = 0;
                    }
                    break;

                    case SDLK_2: {
                        keys[2] = 0;
                    }
                    break;

                    case SDLK_3: {
                        keys[3] = 0;
                    }
                    break;

                    case SDLK_q: {
                        keys[4] = 0;
                    }
                    break;

                    case SDLK_w: {
                        keys[5] = 0;
                    }
                    break;

                    case SDLK_e: {
                        keys[6] = 0;
                    }
                    break;

                    case SDLK_a: {
                        keys[7] = 0;
                    }
                    break;

                    case SDLK_s: {
                        keys[8] = 0;
                    }
                    break;

                    case SDLK_d: {
                        keys[9] = 0;
                    }
                    break;

                    case SDLK_y: {
                        keys[0xA] = 0;
                    }
                    break;

                    case SDLK_c: {
                        keys[0xB] = 0;
                    }
                    break;

                    case SDLK_4: {
                        keys[0xC] = 0;
                    }
                    break;

                    case SDLK_r: {
                        keys[0xD] = 0;
                    }
                    break;

                    case SDLK_f: {
                        keys[0xE] = 0;
                    }
                    break;

                    case SDLK_v: {
                        keys[0xF] = 0;
                    }
                    break;
                }
            }
            break;
        }
    }

    return quit;
}

void chip8::Platform::SoundOutput(bool on)
{
    beeping = on;
}