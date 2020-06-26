//
// Created by Amogh on 6/19/20.
//

#include "utils.hpp"

/**
 * function to convert a square on the chessboard
 * to an index in order to access arrays of pieces, attack
 * sets, etc.
 * @param file: integer value representing the file of the current square, [a-h]
 * @param rank: integer value representing the the rank of the current square, [1-8]
 * @return index: integer in the range [0, 63] to index into the necessary array
 */
int util::fr_to_board_index(int file, int rank)
{
    return 8 * rank + file;
}

/**
 * function to convert a std::string object into a list of tokens
 * where each token in the string is space-separated
 * @param s: std::string object to tokenize
 * @return std::vector<std::string> containing the tokens of the string
 * in the order they appear
 */
std::vector<std::string> util::split_string(const std::string& s)
{
    std::vector<std::string> v;
    std::istringstream iss(s);

    std::copy(
            std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                            std::back_inserter(v)
            );

    return v;
}

/**
 * wrapper function to check whether one string contains
 * another within it
 * @pre: o.size() <= s.size()
 * @param s : string we are searching in
 * @param o : string we are searching for
 * @return boolean value that tells us whether s contains o or not
 * @throws std::invalid_argument error
 */
bool util::string_contains(const std::string& s, const std::string& o)
{
    if (s.size() < o.size())
    {
        throw std::invalid_argument("ERROR! size of o must be less than size of s");
    }
    return s.find(o) != std::string::npos;
}