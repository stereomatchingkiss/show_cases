#include "unique_name_generator.hpp"

#include <format>
#include <ranges>

namespace flt{

unique_name_generator::unique_name_generator(std::string prefix) :
    prefix_{std::move(prefix)}
{

}

bool unique_name_generator::add_unique_name(std::string const &val, void *key)
{
    bool is_unique = true;
    for(auto const &temp : unique_name_){
        if(temp.first == val && key != temp.second){
            is_unique = false;
        }
    }

    if(is_unique){
        unique_name_.emplace_back(val, key);
    }

    return is_unique;
}

void unique_name_generator::change_prefix(std::string val)
{
    prefix_ = std::move(val);
}

void unique_name_generator::clear()
{
    unique_name_.clear();
}

std::string unique_name_generator::get_unique_name() const
{
    size_t index = 0;
    std::string uname;
    while(1){
        uname = std::format("{}{}", prefix_, index);
        auto it = std::ranges::find_if(unique_name_, [&uname](auto const &val)
                                       {
            return uname == val.first;
        });
        if(it == std::end(unique_name_)){
            return uname;
        }

        ++index;
    }

    return uname;
}

void unique_name_generator::remove_unique_name(std::string const &val)
{
    auto it = std::ranges::find_if(unique_name_, [&val](auto const &temp)
                                   {
                                       return val == temp.first;
                                   });
    if(it != std::end(unique_name_)){
        unique_name_.erase(it);
    }
}

}
