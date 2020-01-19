#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.


#include "Macros.hpp"

// [CLI11:public_includes:set]
#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
// [CLI11:public_includes:end]


namespace CLI {

// [CLI11:StringTools:verbatim]
//
/// Include the items in this namespace to get free conversion of enums to/from streams.
/// (This is available inside CLI as well, so CLI11 will use this without a using statement).
namespace enums {

/// output streaming for enumerations
template <typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
std::ostream &operator<<(std::ostream &in, const T &item) {
    // make sure this is out of the detail namespace otherwise it won't be found when needed
    return in << static_cast<typename std::underlying_type<T>::type>(item);
}

} // namespace enums

/// Export out of enum namespace
using enums::operator<<;

namespace detail {

/// a constant defining an expected max vector size defined to be a big number that could be multiplied by 4 and not
/// produce overflow for some expected uses
constexpr int expected_max_vector_size{1 << 29};
// Based on http://stackoverflow.com/questions/236129/split-a-string-in-c
/// Split a string by a delim
CLI11_INLINE std::vector<std::string> split(const std::string &s, char delim);

/// Simple function to join a string
template <typename T> std::string join(const T &v, std::string delim = ",") {
    std::ostringstream s;
    auto beg = std::begin(v);
    auto end = std::end(v);
    if(beg != end)
        s << *beg++;
    while(beg != end) {
        s << delim << *beg++;
    }
    return s.str();
}

/// Simple function to join a string from processed elements
template <typename T,
          typename Callable,
          typename = typename std::enable_if<!std::is_constructible<std::string, Callable>::value>::type>
std::string join(const T &v, Callable func, std::string delim = ",") {
    std::ostringstream s;
    auto beg = std::begin(v);
    auto end = std::end(v);
    if(beg != end)
        s << func(*beg++);
    while(beg != end) {
        s << delim << func(*beg++);
    }
    return s.str();
}

/// Join a string in reverse order
template <typename T> std::string rjoin(const T &v, std::string delim = ",") {
    std::ostringstream s;
    for(std::size_t start = 0; start < v.size(); start++) {
        if(start > 0)
            s << delim;
        s << v[v.size() - start - 1];
    }
    return s.str();
}

// Based roughly on http://stackoverflow.com/questions/25829143/c-trim-whitespace-from-a-string

/// Trim whitespace from left of string
CLI11_INLINE std::string &ltrim(std::string &str);

/// Trim anything from left of string
CLI11_INLINE std::string &ltrim(std::string &str, const std::string &filter);

/// Trim whitespace from right of string
CLI11_INLINE std::string &rtrim(std::string &str);

/// Trim anything from right of string
CLI11_INLINE std::string &rtrim(std::string &str, const std::string &filter);

/// Trim whitespace from string
CLI11_INLINE std::string &trim(std::string &str);

/// Trim anything from string
CLI11_INLINE std::string &trim(std::string &str, const std::string filter);

/// Make a copy of the string and then trim it
CLI11_INLINE std::string trim_copy(const std::string &str);

/// remove quotes at the front and back of a string either '"' or '\''
CLI11_INLINE std::string &remove_quotes(std::string &str);

/// Make a copy of the string and then trim it, any filter string can be used (any char in string is filtered)
CLI11_INLINE std::string trim_copy(const std::string &str, const std::string &filter);

/// Print a two part "help" string
CLI11_INLINE std::ostream &format_help(std::ostream &out, std::string name, std::string description, std::size_t wid);

/// Verify the first character of an option
template <typename T> bool valid_first_char(T c) {
    return std::isalnum(c, std::locale()) || c == '_' || c == '?' || c == '@';
}

/// Verify following characters of an option
template <typename T> bool valid_later_char(T c) { return valid_first_char(c) || c == '.' || c == '-'; }

/// Verify an option name
CLI11_INLINE bool valid_name_string(const std::string &str);

/// Verify that str consists of letters only
CLI11_INLINE bool isalpha(const std::string &str);

/// Return a lower case version of a string
CLI11_INLINE std::string to_lower(std::string str);

/// remove underscores from a string
CLI11_INLINE std::string remove_underscore(std::string str);

/// Find and replace a substring with another substring
CLI11_INLINE std::string find_and_replace(std::string str, std::string from, std::string to);

/// check if the flag definitions has possible false flags
CLI11_INLINE bool has_default_flag_values(const std::string &flags);

CLI11_INLINE void remove_default_flag_values(std::string &flags);

/// Check if a string is a member of a list of strings and optionally ignore case or ignore underscores
CLI11_INLINE std::ptrdiff_t find_member(std::string name,
                                  const std::vector<std::string> names,
                                  bool ignore_case = false,
                                  bool ignore_underscore = false);

/// Find a trigger string and call a modify callable function that takes the current string and starting position of the
/// trigger and returns the position in the string to search for the next trigger string
template <typename Callable> CLI11_INLINE std::string find_and_modify(std::string str, std::string trigger, Callable modify) {
    std::size_t start_pos = 0;
    while((start_pos = str.find(trigger, start_pos)) != std::string::npos) {
        start_pos = modify(str, start_pos);
    }
    return str;
}

/// Split a string '"one two" "three"' into 'one two', 'three'
/// Quote characters can be ` ' or "
CLI11_INLINE std::vector<std::string> split_up(std::string str, char delimiter = '\0');

/// Add a leader to the beginning of all new lines (nothing is added
/// at the start of the first line). `"; "` would be for ini files
///
/// Can't use Regex, or this would be a subs.
CLI11_INLINE std::string fix_newlines(const std::string &leader, std::string input);

/// This function detects an equal or colon followed by an escaped quote after an argument
/// then modifies the string to replace the equality with a space.  This is needed
/// to allow the split up function to work properly and is intended to be used with the find_and_modify function
/// the return value is the offset+1 which is required by the find_and_modify function.
CLI11_INLINE std::size_t escape_detect(std::string &str, std::size_t offset);

/// Add quotes if the string contains spaces
CLI11_INLINE std::string &add_quotes_if_needed(std::string &str);

} // namespace detail

// [CLI11:StringTools:end]

} // namespace CLI

#ifndef CLI11_COMPILED
#include "StringToolsInl.hpp"
#endif
