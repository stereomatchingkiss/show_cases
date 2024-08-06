#pragma once

#include <set>
#include <string>

namespace flt{

/**
 * This class will return unique name "prefix_index" every time you
 * call the api "get_and_update_unique_name"
 */
class unique_name_generator
{
public:
    explicit unique_name_generator(std::string prefix = "");

    bool add_unique_name(std::string const &val);
    void change_prefix(std::string val);
    void clear();
    /**
     * Get unique name, this unique index will add(update) into the dictionary
     * @return unique index
     */
    std::string get_and_update_unique_name();
    void remove_unique_name(std::string const &val);

private:
    std::string prefix_;
    std::set<std::string> unique_name_;
};

}
