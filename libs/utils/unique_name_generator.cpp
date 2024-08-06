#include "unique_name_generator.hpp"

#include <format>

namespace flt{

unique_name_generator::unique_name_generator(std::string prefix) :
    prefix_{std::move(prefix)}
{

}

bool unique_name_generator::add_unique_name(std::string const &val)
{
    if(!unique_name_.contains(val)){
        unique_name_.insert(val);

        return true;
    }

    return false;
}

void unique_name_generator::change_prefix(std::string val)
{
    prefix_ = std::move(val);
}

void unique_name_generator::clear()
{
    unique_name_.clear();
}

std::string unique_name_generator::get_and_update_unique_name()
{
    size_t index = 0;
    std::string uname;
    while(1){
        uname = std::format("{}{}", prefix_, index);
        if(auto it = unique_name_.find(uname); it == std::end(unique_name_)){
            unique_name_.insert(uname);

            return uname;
        }

        ++index;
    }

    return uname;
}

void unique_name_generator::remove_unique_name(const std::string &val)
{
    if(auto it = unique_name_.find(val); it != std::end(unique_name_)){
        unique_name_.erase(it);
    }
}

}
