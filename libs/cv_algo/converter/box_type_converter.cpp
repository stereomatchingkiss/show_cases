#include "box_type_converter.hpp"

#include "../tracker/byte_track/Object.hpp"
#include "../tracker/byte_track/STrack.hpp"

namespace ocv{

byte_track::Object box_info_to_byte_track_obj(const utils::box_info &info)
{
    byte_track::Rect<float> rect(info.x1_, info.y1_, info.x2_ - info.x1_, info.y2_ - info.y1_);
    byte_track::Object obj(rect, info.label_, info.score_);

    return obj;
}

std::vector<byte_track::Object> box_info_to_byte_track_obj(const std::vector<utils::box_info> &inputs)
{
    std::vector<byte_track::Object> objs;
    for(auto const &val : inputs){
        objs.emplace_back(box_info_to_byte_track_obj(val));
    }

    return objs;
}

utils::box_info byte_track_obj_to_box_info(std::shared_ptr<byte_track::STrack> const &ptr, int label)
{
    auto const &rect = ptr->getRect();
    utils::box_info binfo(rect.x(), rect.y(), rect.br_x(), rect.br_y(), ptr->getScore(), label);
    binfo.track_id_ = static_cast<int>(ptr->getTrackId());

    return binfo;
}

std::vector<utils::box_info> byte_track_obj_to_box_info(std::vector<std::shared_ptr<byte_track::STrack>> const &obj, int label)
{
    std::vector<utils::box_info> binfo;
    for(auto const &val : obj){
        binfo.emplace_back(byte_track_obj_to_box_info(val, label));
    }

    return binfo;
}



}
