#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <configuration.h>
#include <input.h>
#include <navigation.h>
#include <render.h>
#include <scores.h>
#include <ui.h>

using json = nlohmann::ordered_json;

namespace
{
    ui::scores_layout view;
}

namespace scores
{
    void init()
    {
        score_table table = load();
        view = ui::load_view<ui::scores_layout>("scores.json");

        for (size_t i = 0; i < table.size(); ++i)
        {
            ui::label l = view.scores.item_template;
            l.text = std::to_string(table[i]);
            l.position.y += i * view.scores.spacing;
            view.scores.items.push_back(l);
        }
    }

    void run()
    {
        if (input::active("cancel"))
        {
            navigation::pop();
        }

        render::begin_frame();
        render::draw(view.title);

        for (const auto &item : view.scores.items)
        {
            render::draw(item);
        }

        render::end_frame();
    }

    score_table load()
    {
        if (!std::filesystem::exists(config.scores_path))
        {
            return score_table({0, 0, 0, 0, 0});
        }

        std::ifstream input(config.scores_path);
        json j = json::parse(input);

        return j.get<score_table>();
    }

    void insert(score_table &table, int score)
    {
        size_t i = 0;

        while (i < table.size() && table[i] >= score)
        {
            ++i;
        }

        if (i == table.size())
        {
            return;
        }

        for (size_t j = table.size() - 1; j > i; --j)
        {
            table[j] = table[j - 1];
        }

        table[i] = score;
    }

    void save(const score_table &table)
    {
        std::ofstream output(config.scores_path);
        output << json(table);
    }
}
