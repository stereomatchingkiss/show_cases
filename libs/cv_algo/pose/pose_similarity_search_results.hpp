#pragma once

#include <string>
#include <vector>

namespace flt::cvt::pose{

struct pose_similarity_search_results
{
    std::vector<std::string> img_paths_;
    std::vector<float> similarity_;
};

}
