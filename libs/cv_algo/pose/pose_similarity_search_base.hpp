#pragma once

#include "pose_estimation_results.hpp"

#include <string>

namespace flt::cvt::pose{

struct pose_similarity_search_results;

class pose_similarity_search_base
{
public:
    pose_similarity_search_base();
    ~pose_similarity_search_base();

    virtual void add_pose(std::string im_path, std::vector<keypoint> kpts) = 0;
    virtual void clear() = 0;
    virtual pose_similarity_search_results find_top_k(std::vector<keypoint> const &kpts) = 0;    
    virtual void remove_pose(std::string const &im_path) = 0;
    virtual void set_top_k(size_t top_k) = 0;
};

}
