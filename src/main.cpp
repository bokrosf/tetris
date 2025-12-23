#include <iostream>
#include <exception>
#include <SDL3/SDL_main.h>
#include <asset.h>
#include <configuration.h>
#include <event.h>
#include <game.h>
#include <render.h>
#include <settings.h>

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
    render::init();
    game::init();
}

void shutdown()
{
    game::shutdown();
    asset::unload_all();
    render::shutdown();
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

        // Update game state.
        game::run();
    }
}
