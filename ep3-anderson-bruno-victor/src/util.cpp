#include "util.hpp"

#include <sstream>
#include <iterator>

// Splits string by spaces
std::vector<std::string> split_spaces (std::string const &str)
{
    std::istringstream buffer(str);

    std::vector<std::string> ret (
            (std::istream_iterator<std::string> (buffer)),
            std::istream_iterator<std::string> ());

    return ret;
}
