#include "pose_similarity_search.hpp"

#include "pose_similarity_estimation.hpp"
#include "pose_similarity_search_results.hpp"

#include <map>

namespace flt::cvt::pose{

struct pose_similarity_search::impl
{
    impl(size_t top_k) :
        top_k_{std::max(top_k, size_t(1))}
    {
    }

    void add_pose(std::string im_path, std::vector<keypoint> kpts)
    {
        pose_dicts_.insert({std::move(im_path), std::move(kpts)});
    }

    void clear()
    {
        pose_dicts_.clear();
    }

    void remove_pose(std::string const &im_path)
    {
        pose_dicts_.erase(im_path);
    }

    pose_similarity_search_results find_top_k(std::vector<keypoint> const &kpts)
    {
        pose_similarity_search_results results;
        std::vector<std::tuple<std::string, float>> scores_list;
        for(auto const &val : pose_dicts_){
            auto const score = euclidean_distance(val.second, kpts);
            scores_list.emplace_back(val.first, score);
        }
        std::ranges::sort(scores_list, [](auto const &a, auto const &b)
                          {
                              return std::get<1>(a) < std::get<1>(b);
                          });
        if(scores_list.size() > top_k_){
            scores_list.resize(top_k_);
        }
        for(auto &val : scores_list){
            results.img_paths_.emplace_back(std::move(std::get<0>(val)));
            results.similarity_.emplace_back(std::get<1>(val));
        }

        return results;
    }

    void set_top_k(size_t top_k)
    {
        top_k_ = top_k;
    }

    //im path, file path, similarity
    std::map<std::string, std::vector<keypoint>> pose_dicts_;
    size_t top_k_;
};

pose_similarity_search::pose_similarity_search(size_t top_k) :
    pose_similarity_search_base(),
    impl_{std::make_unique<impl>(top_k)}
{

}

pose_similarity_search::~pose_similarity_search()
{

}

void pose_similarity_search::add_pose(std::string im_path, std::vector<keypoint> kpts)
{
    impl_->add_pose(std::move(im_path), std::move(kpts));
}

void pose_similarity_search::clear()
{
    impl_->clear();
}

pose_similarity_search_results pose_similarity_search::find_top_k(std::vector<keypoint> const &kpts)
{
    return impl_->find_top_k(kpts);
}

void pose_similarity_search::remove_pose(std::string const &im_path)
{
    impl_->remove_pose(im_path);
}

void pose_similarity_search::set_top_k(size_t top_k)
{
    impl_->set_top_k(top_k);
}

}
