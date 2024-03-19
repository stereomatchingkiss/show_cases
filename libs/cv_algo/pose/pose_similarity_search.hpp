#pragma once

#include "pose_estimation_results.hpp"
#include "pose_similarity_search_base.hpp"

#include <any>
#include <memory>
#include <string>

namespace flt::cvt::pose{

struct pose_similarity_search_results;

class pose_similarity_search : public pose_similarity_search_base
{
public:
    explicit pose_similarity_search(size_t top_k);
    ~pose_similarity_search();

    void add_pose(std::string im_path, std::vector<keypoint> kpts) override;
    void clear() override;
    pose_similarity_search_results find_top_k(std::vector<keypoint> const &kpts) override;
    void remove_pose(std::string const &im_path) override;
    void set_top_k(size_t top_k) override;

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
