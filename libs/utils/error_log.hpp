#ifndef ERROR_LOG_HPP
#define ERROR_LOG_HPP

#include <format>
#include <source_location>
#include <string>

namespace flt{

inline std::string create_error_msg(std::source_location const &loc, std::string const &msg)
{
    return std::format("{}, {}, {}\n{}", loc.file_name(), loc.function_name(), loc.line(), msg);
}

inline std::string create_error_msg(std::source_location const &loc)
{
    return std::format("{}, {}, {}:", loc.file_name(), loc.function_name(), loc.line());
}

}

#endif // ERROR_LOG_HPP
