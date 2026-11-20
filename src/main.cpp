#include <iostream>
#include <exception>
#include <SDL3/SDL_main.h>
#include <asset.h>
#include <configuration.h>
#include <event.h>
#include <game.h>
#include <input.h>
#include <render.h>
#include <settings.h>
#include <timer.h>

void init();
void shutdown();
void run();

int main(int argc, char *argv[])
{
    try
    {
        init();
        run();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    shutdown();

    return 0;
}

void init()
{
    load_configuration("asset/config.json");
    load_settings(config.settings_path);
    event::init();
    timer::init();
    render::init();
    input::track_actions();
    game::init();
}

void shutdown()
{
    game::shutdown();
    asset::unload_all();
    render::shutdown();
    timer::shutdown();
    event::shutdown();
}

void run()
{
    while (true)
    {
        event::read_all();

        if (event::occured(SDL_EVENT_QUIT))
        {
            return;
        }

        if (event::occured(SDL_EVENT_WINDOW_RESIZED))
        {
            render::adapt_to_window();
        }

        if (event::key_down(SDLK_F1))
        {
            render::toggle_fullscreen();
        }

        input::react_to_events();
        game::run();
    }
}
