#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <configuration.h>
#include <scores.h>

using json = nlohmann::ordered_json;

namespace scores
{
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
        int i = 0;

        while (i < table.size() && table[i] >= score)
        {
            ++i;
        }

        if (i == table.size())
        {
            return;
        }

        for (int j = table.size() - 1; j > i; --j)
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
