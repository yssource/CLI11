#pragma once

// [cli11:public_includes:set]
#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
// [cli11:public_includes:end]

#include "Macros.hpp"

namespace CLI {

// [cli11:string_tools_inl:verbatim]

namespace detail {

CLI11_INLINE std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    // Check to see if empty string, give consistent result
    if(s.empty()) {
        elems.emplace_back();
    } else {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while(std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }
    return elems;
}

}  // namespace detail

// [cli11:StringTools-inl:end]

}  // namespace CLI
