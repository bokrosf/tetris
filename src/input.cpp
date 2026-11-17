#include <string>
#include <unordered_map>
#include <event.h>
#include <input.h>
#include <settings.h>

namespace
{
    enum class action_state
    {
        inactive,
        active,
        finished,
    };

    std::unordered_map<std::string, action_state> actions;
}

namespace input
{
    void track_actions()
    {
        actions.clear();

        for (const auto &[name, _] : settings.controls)
        {
            actions[name] = action_state::inactive;
        }
    }

    void react_to_events()
    {
        for (auto &[name, state] : actions)
        {
            auto it = event::queue().crbegin();
            action_state new_state = action_state::inactive;

            while (it != event::queue().crend() && new_state == action_state::inactive)
            {
                switch (it->type)
                {
                    case SDL_EVENT_KEY_DOWN:
                    case SDL_EVENT_KEY_UP:
                        if (!it->key.repeat && it->key.key == settings.controls[name].keyboard)
                        {
                            new_state = it->key.down ? action_state::active : action_state::finished;
                        }
                        break;
                    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    case SDL_EVENT_GAMEPAD_BUTTON_UP:
                        if (it->gbutton.button == settings.controls[name].gamepad)
                        {
                            new_state = it->gbutton.down ? action_state::active : action_state::finished;
                        }
                        break;
                    default:
                        break;
                }

                ++it;
            }

            bool updatable = (new_state != action_state::inactive)
                || (new_state == action_state::inactive && state == action_state::finished);

            if (updatable)
            {
                state = new_state;
            }
        }
    }

    bool active(const std::string &action)
    {
        auto it = actions.find(action);

        return it != actions.end() && it->second == action_state::active;
    }

    bool finished(const std::string &action)
    {
        auto it = actions.find(action);

        return it != actions.end() && it->second == action_state::finished;
    }
}
