#include "track_object_pass.hpp"

#include "track_results.hpp"

#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include <map>

namespace flt::tracker
{

namespace{

namespace bg = boost::geometry;

using point_t = bg::model::point<int, 2, bg::cs::cartesian>;

using segment = boost::geometry::model::segment<point_t>;

struct track_info
{
    cv::Point first_point_in_rect_;
    bool entered_roi_ = false;
    bool lost_track_ = true;
    int lost_count_ = 0;
};

inline point_t pt_convert(cv::Point const &pt)
{
    return point_t(pt.x, pt.y);
}

inline segment to_segment(cv::Point const &pt1, cv::Point const &pt2)
{
    return {pt_convert(pt1), pt_convert(pt2)};
}

}

struct track_object_pass::impl
{
    impl(cv::Rect const &scaled_roi, size_t lost_threshold) :
        lost_threshold_{lost_threshold},
        scaled_roi_{scaled_roi},
        seg_left_(pt_convert(scaled_roi.tl()), pt_convert({scaled_roi.x, scaled_roi.y + scaled_roi.height})),
        seg_right_(pt_convert({scaled_roi.x + scaled_roi.width, scaled_roi.y}), pt_convert(scaled_roi.br())),
        seg_top_(pt_convert(scaled_roi.tl()), pt_convert({scaled_roi.x + scaled_roi.width, scaled_roi.y})),
        seg_bottom_(pt_convert({scaled_roi.x, scaled_roi.y + scaled_roi.height}), pt_convert(scaled_roi.br()))
    {
    }

    void init_lost_track()
    {
        for(auto &val : track_map_){
            val.second.lost_track_ = true;
        }
    }

    void erase_lost_track()
    {
        for(auto it = std::begin(track_map_); it != std::end(track_map_);){
            auto &val = it->second;
            if(val.lost_track_){
                ++val.lost_count_;
                if(val.lost_count_ >= lost_threshold_){
                    track_map_.erase(it++);
                }else{
                    ++it;
                }
            }else{
                ++it;
            }
        }
    }

    void count_pass_directions(track_info const &last_track, cv::Point const &cur_center, std::forward_iterator auto it)
    {
        if(last_track.entered_roi_){
            auto const exist_segment = to_segment(last_track.first_point_in_rect_, cur_center);
            if(leave_from_bottom(exist_segment)){
                ++track_res_.count_bottom_pass_;
            }else if(leave_from_top(exist_segment)){
                ++track_res_.count_top_pass_;
            }else if(leave_from_left(exist_segment)){
                ++track_res_.count_left_pass_;
            }else if(leave_from_right(exist_segment)){
                ++track_res_.count_right_pass_;
            }
            track_map_.erase(it);
        }
    }

    void add_new_track(det::box_info const &cur_track)
    {
        track_info ti;
        if(scaled_roi_.contains(cur_track.center())){
            ti.first_point_in_rect_ = cur_track.center();
            ti.entered_roi_ = true;
        }

        track_map_.insert({cur_track.track_id_, ti});
    }

    track_results track(std::vector<det::box_info> const &input)
    {
        init_lost_track();
        for(auto const &cur_track : input){
            if(auto it = track_map_.find(cur_track.track_id_); it != std::end(track_map_)){
                auto &last_track = it->second;
                last_track.lost_count_ = 0;
                last_track.lost_track_ = false;
                if(scaled_roi_.contains(cur_track.center())){
                    if(!last_track.entered_roi_){
                        last_track.first_point_in_rect_ = cur_track.center();
                        last_track.entered_roi_ = true;
                        ++track_res_.count_in_center_;
                    }
                }else{
                    count_pass_directions(last_track, cur_track.center(), it);
                }
            }else{
                add_new_track(cur_track);
            }
        }
        erase_lost_track();

        return track_res_;
    }

    bool leave_from_bottom(segment const &input) const noexcept
    {
        return boost::geometry::intersects(input, seg_bottom_);
    }

    bool leave_from_top(segment const &input) const noexcept
    {
        return boost::geometry::intersects(input, seg_top_);
    }

    bool leave_from_left(segment const &input) const noexcept
    {
        return boost::geometry::intersects(input, seg_left_);
    }

    bool leave_from_right(segment const &input) const noexcept
    {
        return boost::geometry::intersects(input, seg_right_);;
    }

    size_t lost_threshold_ = 0;

    cv::Rect scaled_roi_;

    segment seg_left_;
    segment seg_right_;
    segment seg_top_;
    segment seg_bottom_;

    std::map<int, track_info> track_map_;

    track_results track_res_;
};

track_object_pass::track_object_pass(cv::Rect const &scaled_roi, size_t lost_threshold) :
    impl_{std::make_unique<impl>(scaled_roi, lost_threshold)}
{

}

track_object_pass::~track_object_pass()
{

}

track_results track_object_pass::track(std::vector<det::box_info> const &input)
{
    return impl_->track(input);
}

}
