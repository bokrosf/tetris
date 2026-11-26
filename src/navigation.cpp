#include <stdexcept>
#include <game.h>
#include <navigation.h>
#include <timer.h>

namespace
{
    using namespace navigation;

    enum class action
    {
        push,
        pop,
        root,
    };

    struct request
    {
        ::action action;
        ::scene scene;
    };

    const int max_context_count = 8;
    const int max_request_count = 8;

    int last_id;
    context contexts[max_context_count];
    int context_count;
    request requests[max_request_count];
    int request_count;

    context create_context(scene s)
    {
        context c{.id = ++last_id};

        switch (s)
        {
            case scene::main_menu:
                c.init = nullptr;
                c.run = nullptr;
                c.shutdown = nullptr;
                break;
            case scene::game:
                c.init = game::init;
                c.run = game::run;
                c.shutdown = game::shutdown;
                break;
            case scene::settings:
                c.init = nullptr;
                c.run = nullptr;
                c.shutdown = nullptr;
                break;
            case scene::pause:
                c.init = nullptr;
                c.run = nullptr;
                c.shutdown = nullptr;
                break;
            case scene::scores:
                c.init = nullptr;
                c.run = nullptr;
                c.shutdown = nullptr;
                break;
            default:
                c.init = nullptr;
                c.run = nullptr;
                c.shutdown = nullptr;
                break;
        }

        return c;
    }

    void push_context(scene s)
    {
        if (context_count < max_context_count)
        {
            context c = create_context(s);
            c.init();
            contexts[context_count++] = c;
        }
    }

    void pop_context()
    {
        if (context_count > 0)
        {
            context &c = contexts[--context_count];
            c.shutdown();
            timer::remove_context(c.id);
        }
    }

    void shutdown_all_contexts()
    {
        while (context_count > 0)
        {
            pop_context();
        }
    }
}

namespace navigation
{
    void init()
    {
        last_id = 0;
        context_count = 0;
        request_count = 0;
    }

    void shutdown()
    {
        shutdown_all_contexts();
    }

    void push(scene s)
    {
        if (request_count < max_request_count)
        {
            requests[request_count++] = request
            {
                .action = action::push,
                .scene = s,
            };
        }
    }

    void pop()
    {
        if (request_count < max_request_count)
        {
            requests[request_count++] = request
            {
                .action = action::pop,
            };
        }
    }

    void root(scene s)
    {
        if (request_count < max_request_count)
        {
            requests[request_count++] = request
            {
                .action = action::root,
                .scene = s,
            };
        }
    }

    context &top()
    {
        if (context_count < 1)
        {
            throw std::runtime_error("Navigation stack must not be empty.");
        }

        return contexts[context_count - 1];
    }

    void commit()
    {
        for (int i = 0; i < request_count; ++i)
        {
            switch (requests[i].action)
            {
                case action::push:
                    push_context(requests[i].scene);
                    break;
                case action::pop:
                    pop_context();
                    break;
                case action::root:
                    shutdown_all_contexts();
                    push_context(requests[i].scene);
                    break;
                default:
                    break;
            }
        }

        request_count = 0;
    }
}
