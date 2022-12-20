/*
gpe_timer.cpp
This file is part of:
GAME PENCIL ENGINE
https://www.pawbyte.com/gamepencilengine
Copyright (c) 2014-2023 Nathan Hurde, Chase Lee.

Copyright (c) 2014-2023 PawByte LLC.
Copyright (c) 2014-2023 Game Pencil Engine contributors ( Contributors Page )

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

#include "../gpe/internal_libs/stg_ex.h"
#include "gpe_timer_sdl2.h"

namespace gpe
{
    bool init_sdl2_time_system()
    {
        error_log->report("Starting SDL2 timekeeper...");
        if( time_keeper != NULL )
        {
            delete time_keeper;
            time_keeper = NULL;
        }
        time_keeper = new time_keeper_sdl2("sdl");
        time_keeper->set_fps( fps_cap );
        error_log->report("Timekeeper swapped from base to sdl class...");
        return true;
    }

    void quit_sdl2_time_system()
    {
        if( time_keeper !=NULL )
        {
            delete time_keeper;
            time_keeper = NULL;
        }
        time_keeper = new time_keeper_base();
    }


    //The frames per second cap timer
    time_keeper_sdl2::time_keeper_sdl2(std::string t_name)
    {
        keeper_type = "sdl";
        keeper_name = t_name;
        time_past = 0;
        fps_average_count = 5;
        system_cap_on = true;
        min_delay_ms = 16;
        //Initialize the variables
        my_fps = 0;
        paused = false;
        started = false;
        if( fps_cap <=0 )
        {
            fps_cap = 20;
        }
        ticks_per_frame = (float)(1000.f / fps_cap);
        seconds_per_frame = 1.f/fps_cap;
        delta_ticks = ticks_per_frame;
        delta_performance =  ticks_per_frame;
        time_past = 0;
        ticks_now =0;
        time_now = 0;
        ticks_paused = 0;
        timer_frequency = (float)SDL_GetPerformanceFrequency();
        ticks_elapsed = 0;
        frames_passed_counter = 0;
        vsync_is_on = true;
    }

    time_keeper_sdl2::~time_keeper_sdl2()
    {
        recorded_fps.clear();
    }

    void time_keeper_sdl2::cap_fps( bool is_minimized )
    {
        //Returns if the window is minimized to prevent strange behaviors
        if( is_minimized)
        {
            delta_ticks = 0;
            delta_performance = 0;
            time_past = 0;
            ticks_start = 0;
            return;
        }
        time_now = get_performance_ms();
        ticks_now = get_ticks();
        delta_ticks =  ticks_now - ticks_start;
        delta_performance = time_now - time_past;

        if( vsync_is_on )
        {
            return;
        }

        if( delta_ticks < ticks_per_frame )
        {
            delay( ticks_per_frame - delta_ticks );
            time_now = get_performance_ms();
            delta_performance = time_now - time_past;
        }
    }

    void time_keeper_sdl2::delay( float delay_time )
    {
        if( delay_time <= 0.f )
        {
            return;
        }

        if( system_cap_on || delay_time >= min_delay_ms )
        {
            //error_log->report("Via SDL_Delay..");
            SDL_Delay( floor( delay_time + 0.5 ) );
            delta_ticks = ticks_per_frame;
            return;
        }

        float time_c = 0;
        float time_p = get_ticks();
        float timeDiff = 0;
        int loopCount = 0;
        while( timeDiff < delay_time )
        {
            time_c = get_ticks();
            timeDiff = get_time_difference(time_p, time_c );
            loopCount++;
        }
        delta_ticks = ticks_per_frame;
        delta_performance = ticks_per_frame;
    }

    void time_keeper_sdl2::finish_timer()
    {

    }

    float time_keeper_sdl2::get_delta_performance()
    {
        return delta_performance;
    }

    float time_keeper_sdl2::get_delta_ticks()
    {
        return delta_ticks;
    }

    float time_keeper_sdl2::get_fps()
    {
        return my_fps;
    }

    float time_keeper_sdl2::get_fps_cap()
    {
        return fps_cap;
    }

    float time_keeper_sdl2::get_needed_ticks()
    {
        return ticks_per_frame;
    }

    float time_keeper_sdl2::get_performance_ms()
    {
        return ( ( (float)SDL_GetPerformanceCounter() )/( (float)SDL_GetPerformanceFrequency() ) )* 1000.0f;
    }

    float time_keeper_sdl2::get_performance_seconds()
    {
        return ( (float)SDL_GetPerformanceCounter() )/( (float)SDL_GetPerformanceFrequency() );
    }

    uint64_t time_keeper_sdl2::get_ticks()
    {
        return SDL_GetTicks();
    }

    float time_keeper_sdl2::get_time_difference( uint64_t time_p, uint64_t time_c )
    {
        return time_c - time_p;
    }

    bool time_keeper_sdl2::is_started()
    {
        return started;
    }

    bool time_keeper_sdl2::is_paused()
    {
        return paused;
    }

    void time_keeper_sdl2::pause_timer()
    {
        //If the timer is running and isn't already paused
        if( started &&  !paused )
        {
            //Pause the timer
            paused = true;

            //Calculate the paused ticks
            ticks_paused = get_ticks();
        }
    }

    void time_keeper_sdl2::reset_timer()
    {
        time_now = 0;
        time_past = 0;
        ticks_now = 0;
        ticks_start = 0;

        ticks_elapsed = 0;
        frames_passed_counter = 0;
        delta_ticks = 0;
        recorded_fps.clear();
    }

    void time_keeper_sdl2::set_fps( float fps_new )
    {
        //resets the timers and such since a new FPS cap has been set.
        if( fps_new > 0 && fps_new <= 1000.f )
        {
            fps_cap = fps_new;
        }
        else
        {
            //defaults to 15 fps for out of range FPSs given.
            fps_cap = 15;
        }
        my_fps = fps_cap;
        ticks_per_frame = 1000.f / my_fps;
        seconds_per_frame = 1.f/my_fps;
        fps_ratio = my_fps/15.f;
        fps_current = my_fps;
        if( fps_ratio < 1.f)
        {
            fps_ratio = 1.f;
        }
        recorded_fps.clear();
    }

    void time_keeper_sdl2::set_vysnc( bool vs_on )
    {
        vsync_is_on = vs_on;

        if( vsync_is_on )
        {
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        }
        else
        {
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
        }
    }

    void time_keeper_sdl2::set_average_fps_count( int new_count )
    {
        if( fps_average_count != new_count)
        {
            fps_average_count = new_count;
            if( fps_average_count < 1 )
            {
                fps_average_count = 1 ;
            }
            recorded_fps.clear();
        }

    }

    void time_keeper_sdl2::start_timer()
    {
        ticks_start = get_ticks();
        time_past = get_performance_ms();
    }

    void time_keeper_sdl2::stop_timer()
    {
        reset_timer();

        //Stop the timer
        started = false;

        //Unpause the timer
        paused = false;

        ticks_paused = get_ticks();
    }


    void time_keeper_sdl2::unpause_timer()
    {
        //If the timer is paused
        if(  paused  )
        {
            start_timer();
        }
    }
}
