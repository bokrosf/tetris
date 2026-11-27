#include <string>
#include <configuration.h>
#include <input.h>
#include <main.h>
#include <menu.h>
#include <navigation.h>
#include <ui.h>
#include <render.h>

namespace
{    
    struct menu_item
    {
        std::string text;
        void (*ok)();
    };

    menu_item main[4]
    {
        {"GAME", []() { navigation::root(navigation::scene::game); }},
        {"SCORES", []() { navigation::push(navigation::scene::scores); }},
        {"SETTINGS", []() { navigation::push(navigation::scene::settings); }},
        {"EXIT", []() { exit_app(); }},
    };

    menu_item pause[3]
    {
        {"CONTINUE", []() { navigation::pop(); }},
        {"SETTINGS", []() { navigation::push(navigation::scene::settings); }},
        {"EXIT", []() { exit_app(); }},
    };

    struct menu_context
    {
        menu_item *menu;
        ui::menu_layout view;
        int count;
        int selected;
        void (*cancel)();
        bool input_enabled;
    };

    menu_context context;

    void init_view(const std::string &name)
    {
        context.view = ui::load_view<ui::menu_layout>(config.views_path / name);
        
        for (int i = 0; i < context.count; ++i)
        {
            ui::label l = context.view.items.item_template;
            l.text = context.menu[i].text;
            l.position.y += i * context.view.items.spacing;
            context.view.items.items.push_back(std::move(l));
        }

        context.view.items.items[context.selected].color = context.view.selected_color;

    }

    void handle_input()
    {
        for (const auto &action : {"menu_down", "menu_up", "ok", "cancel"})
        {
            if (input::finished(action))
            {
                context.input_enabled = true;
            }
        }

        if (!context.input_enabled)
        {
            return;
        }
        
        if (input::active("menu_down"))
        {
            context.input_enabled = false;
            context.view.items.items[context.selected].color = context.view.items.item_template.color;
            ++context.selected;
            context.selected %= context.count;
            context.view.items.items[context.selected].color = context.view.selected_color;
        }

        if (input::active("menu_up"))
        {
            context.input_enabled = false;
            context.view.items.items[context.selected].color = context.view.items.item_template.color;
            --context.selected;

            if (context.selected < 0)
            {
                context.selected = context.count + context.selected;
            }

            context.selected %= context.count;
            context.view.items.items[context.selected].color = context.view.selected_color;
        }

        if (input::active("ok"))
        {
            context.input_enabled = false;
            context.menu[context.selected].ok();
        }
        else if (input::active("cancel"))
        {
            context.input_enabled = false;

            if (context.cancel)
            {
                context.cancel();
            }
        }
    }
}

namespace menu
{
    void init_main()
    {
        context =
        {
            .menu = main,
            .count = sizeof(main) / sizeof(menu_item),
            .selected = 0,
            .cancel = nullptr,
            .input_enabled = true,
        };

        init_view("main_menu.json");
    }

    void init_pause()
    {
        context =
        {
            .menu = pause,
            .count = sizeof(pause) / sizeof(menu_item),
            .selected = 0,
            .cancel = []() { navigation::pop(); },
            .input_enabled = true,
        };

        init_view("pause_menu.json");
    }

    void run()
    {
        handle_input();
        render::begin_frame();
        render::draw(context.view.title);

        for (const auto &item : context.view.items.items)
        {
            render::draw(item);
        }

        render::end_frame();
    }
}
