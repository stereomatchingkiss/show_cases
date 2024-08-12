#include "nanodet_alert_save.hpp"

#include "../../config/config_alert_sender.hpp"

#include "../../global/global_keywords.hpp"

#include <cv_algo/tracker/track_results.hpp>

#include <utils/image_utils.hpp>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>

#ifndef WASM_BUILD
#include <QFile>
#include <QTextStream>
#endif

#include <format>

nanodet_alert_save::nanodet_alert_save(std::vector<std::string> names) :
    names_(std::move(names))
{

}

nanodet_alert_save::~nanodet_alert_save()
{

}

const QByteArray &nanodet_alert_save::get_alert_info() const
{
    return alert_info_;
}

void nanodet_alert_save::clear_im_name()
{
    im_name_.clear();
}

void nanodet_alert_save::create_dir_path()
{
#ifndef WASM_BUILD
    dir_path_ = global_keywords().tracker_alert_path() + stream_name_ + "/" +
                QDateTime::currentDateTime().toString("yyyy_MM_dd") + "/";
    QDir().mkpath(dir_path_);
#endif
}

void nanodet_alert_save::save_to_json(flt::cvt::tracker::track_duration const &val, QString const &im_name)
{
    create_dir_path();

    QJsonObject jobj;
    jobj["image_name"] = im_name;
#ifdef WASM_BUILD
    jobj["image"] = QString(flt::to_base64_img(img));
#endif
    jobj["track_id"] = val.id_;
    jobj["label"] = names_[val.label_].c_str();
    jobj["time"] = QDateTime::currentDateTime().toString("yyyy_MM_dd,hh_mm_ss");
    jobj["roi"] = std::format("x:{},y:{},width:{},height:{}",
                              val.rect_.x, val.rect_.y, val.rect_.width, val.rect_.height).c_str();
    jobj["duration"] = val.duration_sec_;

    alert_info_ = QJsonDocument(jobj).toJson(QJsonDocument::Compact);
#ifndef WASM_BUILD
    save_to_json();
#endif
}

void nanodet_alert_save::set_stream_name(const QString &name)
{
    stream_name_ = name;
}

QString nanodet_alert_save::create_im_name() const
{
    return  QString("%1").arg(QDateTime::currentDateTime().toString("hh_mm_ss"));
}

QString nanodet_alert_save::create_fpath(QString const &im_name) const
{
    return dir_path_ + "/" + im_name + ".jpg";
}

void nanodet_alert_save::save_to_json() const
{
    if(QFile file(dir_path_ + QString("/%1.txt").arg(im_name_)); file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream<<alert_info_<<"\n";
    }
}
