#include "pose_similarity_fast_search.hpp"

#include "pose_similarity_estimation.hpp"
#include "pose_similarity_search_results.hpp"

#include <kissrandom.h>
#include <annoylib.h>

#include <map>

using namespace Annoy;

namespace flt::cvt::pose{

struct pose_similarity_fast_search::impl
{
    impl(size_t top_k, int n_tree) :
        annoy_tree_{annoy_item_size_},
        n_tree_{std::max(n_tree, annoy_item_size_)},
        top_k_{std::max(top_k, size_t(1))}
    {
    }

    void to_annoy_item(std::vector<keypoint> const &kpts)
    {
        for(size_t i = 0, j = 0; i != kpts.size(); ++i, j += 2){
            annoy_item_[j] = static_cast<annoy_val_type>(kpts[i].hx_);
            annoy_item_[j + 1] = static_cast<annoy_val_type>(kpts[i].hy_);
        }
    }

    void add_pose(std::string im_path, std::vector<keypoint> kpts)
    {
        to_annoy_item(kpts);

        annoy_tree_.add_item(annoy_id_, annoy_item_.data());
        name_key_.insert({im_path, {annoy_id_, std::move(kpts)}});
        annoy_id_key_.insert({annoy_id_, std::move(im_path)});
        ++annoy_id_;
    }

    void build()
    {
        annoy_tree_.build(n_tree_);
    }

    void clear()
    {
        annoy_id_ = 0;
        annoy_tree_.reinitialize();
        name_key_.clear();
        name_key_.clear();
    }

    void remove_pose(std::string const &im_path)
    {

    }

    pose_similarity_search_results find_top_k(std::vector<keypoint> const &kpts)
    {
        pose_similarity_search_results results;
        to_annoy_item(kpts);

        std::vector<int> ids;
        annoy_tree_.get_nns_by_vector(annoy_item_.data(),
                                      top_k_, static_cast<int>(top_k_ * n_tree_),
                                      &ids, &results.similarity_);
        auto sim_it = std::begin(results.similarity_);
        for(size_t i = 0; i != ids.size(); ++i){
            if(auto it = annoy_id_key_.find(ids[i]); it != std::end(annoy_id_key_)){
                results.img_paths_.emplace_back(it->second);
                ++sim_it;
            }else{
                results.similarity_.erase(sim_it++);
            }
        }

        return results;
    }

    void set_top_k(size_t top_k)
    {
        top_k_ = top_k;
    }

    using annoy_val_type = float;
    using annoy_tree = AnnoyIndex<int, annoy_val_type, Angular, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> ;


    static int constexpr annoy_item_size_ = 34;
    int annoy_id_ = 0;
    std::array<annoy_val_type, annoy_item_size_> annoy_item_;
    annoy_tree annoy_tree_;
    int n_tree_;
    std::map<std::string, std::tuple<int, std::vector<keypoint>>> name_key_;
    std::map<int, std::string> annoy_id_key_;
    size_t top_k_;
};

pose_similarity_fast_search::pose_similarity_fast_search(size_t top_k, int n_tree)  :
    pose_similarity_search_base(),
    impl_{std::make_unique<impl>(top_k, n_tree)}
{

}

pose_similarity_fast_search::~pose_similarity_fast_search()
{

}

void pose_similarity_fast_search::add_pose(std::string im_path, std::vector<keypoint> kpts)
{
    impl_->add_pose(std::move(im_path), std::move(kpts));
}

void pose_similarity_fast_search::build()
{
    impl_->build();
}

void pose_similarity_fast_search::clear()
{
    impl_->clear();
}

pose_similarity_search_results pose_similarity_fast_search::find_top_k(std::vector<keypoint> const &kpts)
{
    return impl_->find_top_k(kpts);
}

void pose_similarity_fast_search::remove_pose(std::string const &im_path)
{
    impl_->remove_pose(im_path);
}

void pose_similarity_fast_search::set_top_k(size_t top_k)
{
    impl_->set_top_k(top_k);
}

}
