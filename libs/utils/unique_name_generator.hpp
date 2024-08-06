#pragma once

#include <string>
#include <vector>

namespace flt{

/**
 * This class will return unique name "prefix_index" every time you
 * call the api "get_unique_name"
 */
class unique_name_generator
{
public:
    explicit unique_name_generator(std::string prefix = "");

    bool add_unique_name(std::string const &val, void *key);
    void change_prefix(std::string val);
    void clear();
    std::string get_unique_name() const;
    void remove_unique_name(std::string const &val);

private:
    std::string prefix_;
    std::vector<std::pair<std::string, void*>> unique_name_;
};

}
