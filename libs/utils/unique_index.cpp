#include "unique_index.hpp"

#include <ranges>

using namespace std::ranges;

namespace flt{

bool unique_index::add_unique_index(size_t val)
{
    if(auto it = find(indexes_, val); it != std::end(indexes_)){
        indexes_.emplace_back(val);
        return true;
    }

    return false;
}

void unique_index::clear()
{
    indexes_.clear();
    indexes_.shrink_to_fit();
}

size_t unique_index::get_and_update_unique_index()
{
    for(size_t unique_index = 0; ; ++unique_index){
        if(auto it = find(indexes_, unique_index); it != std::end(indexes_)){
            indexes_.emplace_back(unique_index);
            return unique_index;
        }
    }

    return 0;
}

void unique_index::remove_unique_index(size_t val)
{
    if(auto it = find(indexes_, val); it != std::end(indexes_)){
        indexes_.erase(it);
    }
}

}
