//
// Created by Amogh on 6/19/20.
//

#ifndef DIOGENES_UTILS_HPP
#define DIOGENES_UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace util
{
    int fr_to_board_index(int file, int rank);
    std::vector<std::string> split_string(const std::string& s);
    bool string_contains(const std::string& s, const std::string& o);
}

#endif //DIOGENES_UTILS_HPP
