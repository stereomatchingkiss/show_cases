#include "nanodet_alert_save.hpp"

#include "../../config/config_alert_sender.hpp"

#include "../../global/global_keywords.hpp"

#include <cv_algo/tracker/track_results.hpp>

#include <utils/image_utils.hpp>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>

nanodet_alert_save::nanodet_alert_save(const std::vector<std::string> &names) :
    names_(names)
{

}

nanodet_alert_save::~nanodet_alert_save()
{

}

const QByteArray &nanodet_alert_save::get_alert_info() const
{
    return alert_info_;
}

void nanodet_alert_save::change_alert_sender_config(config_alert_sender const &val)
{
    save_reports_ = val.save_reports_;
    send_alert_ = val.activate_;
    send_by_text_ = val.send_by_text_;
    open_file();
}

void nanodet_alert_save::clear_im_name()
{
    im_name_.clear();
}

const QString &nanodet_alert_save::get_im_name() const
{
    return im_name_;
}

void nanodet_alert_save::open_file()
{
#ifndef WASM_BUILD
    if(save_reports_){
        dir_path_ = global_keywords().tracker_alert_path() + "/cam0/" +
                    QDateTime::currentDateTime().toString("yyyy_MM_dd_hh,hh_mm_ss") + "/";
        QDir().mkpath(dir_path_);
        file_.setFileName(dir_path_ + "/info.txt");

        if(file_.open(QIODevice::WriteOnly)){
            stream_.setDevice(&file_);
        }
    }else{
        file_.close();
    }
#endif
}

void nanodet_alert_save::save_to_json(const flt::cvt::tracker::track_duration &val, const QImage &img, size_t im_ids)
{
    if(file_.isOpen() || send_alert_){
        if(im_name_.isEmpty()){
            im_name_ = create_fname(im_ids);
        }
        QJsonObject jobj;
#ifdef WASM_BUILD
        jobj["image_name"] = im_name_;
#else
        jobj["image_name"] = QFileInfo(im_name_).baseName();
#endif
        jobj["track_id"] = val.id_;
        jobj["label"] = names_[val.label_].c_str();
        jobj["time"] = QDateTime::currentDateTime().toString();
        jobj["roi"] = std::format("x:{},y:{},width:{},height:{}",
                                  val.rect_.x, val.rect_.y, val.rect_.width, val.rect_.height).c_str();
        jobj["duration"] = val.duration_sec_;

#ifndef WASM_BUILD
        jobj["image"] = QString(flt::to_base64_img(img));
#endif

        alert_info_ = QJsonDocument(jobj).toJson(QJsonDocument::Compact);
#ifndef WASM_BUILD
        stream_<<alert_info_<<"\n";
#endif
    }
}

bool nanodet_alert_save::send_by_text() const
{
    return send_by_text_;
}

QString nanodet_alert_save::create_fname(size_t im_ids) const
{
    return  dir_path_ + QString("%1").arg(im_ids, 6, 10, QChar('0')) + ".jpg";
}
