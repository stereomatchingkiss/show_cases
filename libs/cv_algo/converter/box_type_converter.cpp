#include "box_type_converter.hpp"

#include "../tracker/byte_track/Object.hpp"
#include "../tracker/byte_track/STrack.hpp"

namespace flt{

using namespace det;

tracker::Object box_info_to_byte_track_obj(const box_info &info)
{
    tracker::Rect<float> rect(info.rect_.x, info.rect_.y, info.x2_ - info.rect_.x, info.y2_ - info.rect_.y);
    tracker::Object obj(rect, info.label_, info.score_);

    return obj;
}

std::vector<tracker::Object> box_info_to_byte_track_obj(const std::vector<box_info> &inputs)
{
    std::vector<tracker::Object> objs;
    for(auto const &val : inputs){
        objs.emplace_back(box_info_to_byte_track_obj(val));
    }

    return objs;
}

box_info byte_track_obj_to_box_info(std::shared_ptr<tracker::STrack> const &ptr, int label)
{
    auto const &rect = ptr->getRect();
    box_info binfo(rect.x(), rect.y(), rect.br_x(), rect.br_y(), ptr->getScore(), label);
    binfo.track_id_ = static_cast<int>(ptr->getTrackId());

    return binfo;
}

std::vector<box_info> byte_track_obj_to_box_info(std::vector<std::shared_ptr<tracker::STrack>> const &obj, int label)
{
    std::vector<box_info> binfo;
    for(auto const &val : obj){
        binfo.emplace_back(byte_track_obj_to_box_info(val, label));
    }

    return binfo;
}



}
