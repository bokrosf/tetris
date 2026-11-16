#include <event.h>
#include <input.h>
#include <settings.h>

namespace input
{
    bool active(const std::string &action)
    {
        if (!settings.controls.contains(action))
        {
            return false;
        }

        const auto &[key, button] = settings.controls[action];

        return event::key_down(key) || event::button_down(button);
    }

    bool finished(const std::string &action)
    {
        if (!settings.controls.contains(action))
        {
            return false;
        }

        const auto &[key, button] = settings.controls[action];

        return event::key_up(key) || event::button_up(button);
    }
}
