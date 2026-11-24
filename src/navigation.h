#ifndef TETRIS_NAVIGATION_H
#define TETRIS_NAVIGATION_H

namespace navigation
{
    enum class scene
    {
        main_menu,
        game,
        settings,
        pause,
        scores,
    };

    using operation = void (*)();

    struct context
    {
        int id;
        operation init;
        operation run;
        operation shutdown;
    };

    void init();
    void shutdown();
    void push(scene s);
    void pop();
    void root(scene s);
    context &top();
    void commit();
}

#endif
