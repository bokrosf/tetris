#include <iostream>
#include <exception>
#include <SDL3/SDL_main.h>
#include <configuration.h>

int main(int argc, char *argv[])
{
    std::cout << "Tetris" << std::endl;

    try
    {
        load_configuration("asset/config.json");
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        throw;
    }

    return 0;
}
