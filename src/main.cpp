#include <iostream>
#include <exception>
#include <SDL3/SDL_main.h>
#include <configuration.h>
#include <font.h>
#include <settings.h>

void init();

int main(int argc, char *argv[])
{
    std::cout << "Tetris" << std::endl;

    try
    {
        init();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        throw;
    }

    return 0;
}

void init()
{
    load_configuration("asset/config.json");
    load_font(config.font_path);
    load_settings(config.settings_path);
}

