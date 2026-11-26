#ifndef TETRIS_SCORES_H
#define TETRIS_SCORES_H

#include <array>

using score_table = std::array<int, 5>;

namespace scores
{
    void init();
    void run();

    score_table load();
    void insert(score_table &table, int score);
    void save(const score_table &table);
}

#endif
