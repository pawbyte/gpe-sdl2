/*
gpe_sdl_basic.cpp
This file is part of:
GAME PENCIL ENGINE
https://www.pawbyte.com/gamepencilengine
Copyright (c) 2014-2021 Nathan Hurde, Chase Lee.

Copyright (c) 2014-2021 PawByte LLC.
Copyright (c) 2014-2021 Game Pencil Engine contributors ( Contributors Page )

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-Game Pencil Engine <https://www.pawbyte.com/gamepencilengine>


*/

#include "gpe_sdl_basic.h"

namespace gpe
{
    bool using_sdl_system_underneath;

    //initialize sdl
    bool init_sdl_main_system()
    {
        //Initialize all SDL subsystems
        error_log->report("--Starting SDL2...");

        error_log->report("--SDL2 systems started...");

        if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
        {
            error_log->report("---Error initializing SDL!");
            return false;
        }

        error_log->report("--Attempting to initialize SDL_INIT_TIMER");
        if( SDL_InitSubSystem( SDL_INIT_TIMER ) == -1 )
        {
            error_log->report("---Error initializing SDL_INIT_TIMER");
            return false;
        }

        error_log->report("--Attempting to initialize SDL_INIT_EVENTS");
        if( SDL_InitSubSystem( SDL_INIT_EVENTS ) == -1 )
        {
            error_log->report("---Error initializing SDL_INIT_EVENTS");
            return false;
        }


        error_log->report("--Attempting to initialize SDL_INIT_gamepad");
        if( SDL_InitSubSystem( SDL_INIT_GAMECONTROLLER ) == -1 )
        {
            error_log->report("---Error initializing SDL_INIT_gamepad");
            return false;
        }

        error_log->report("--Attempting to initialize SDL_INIT_HAPTIC");
        if( SDL_InitSubSystem( SDL_INIT_HAPTIC ) == -1 )
        {
            error_log->report("---Error initializing SDL_INIT_HAPTIC");
            return false;
        }

        error_log->report("-Setting event system..");

        //enable the event state, cursor and sdl_img
        SDL_EventState(SDL_DROPFILE,SDL_ENABLE);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
        SDL_ShowCursor(SDL_ENABLE);

        error_log->report("-GPE_Img_System...");
        //loads support for JPG and other image formats.
        int flags=IMG_INIT_JPG|IMG_INIT_PNG |IMG_INIT_TIF;
        if( (IMG_Init(flags)&flags) != flags)
        {
            error_log->report("--IMG_Init: Failed to init required jpg and png support!\n");
            std::string imgError = IMG_GetError();
            error_log->report("--IMG_Init: " + imgError );
            error_log->report("--Error initializing SDL_Mixer.");
            error_log->report("--Error initializing SDL_Mixer.");
            return false;
            // handle error
        }
        using_sdl_system_underneath = true;
        return true;
    }

    bool sdl_is_initted()
    {
        return using_sdl_system_underneath;
    }

    void quit_sdl_main_system()
    {
        SDL_Quit();
        using_sdl_system_underneath = false;
    }
}
