#pragma once

#include <vector>

namespace flt{

class unique_index
{
public:
    bool add_unique_index(size_t val);
    void clear();
    /**
     * Get unique index, this unique index will add(update) into the dictionary
     * @return unique index
     */
    size_t get_and_update_unique_index();
    void remove_unique_index(size_t val);

private:
    std::vector<size_t> indexes_;
};

}
