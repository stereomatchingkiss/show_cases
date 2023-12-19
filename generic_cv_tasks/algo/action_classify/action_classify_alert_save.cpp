#include "action_classify_alert_save.hpp"

#include "../../config/config_alert_sender.hpp"

#include "../../global/global_keywords.hpp"

#ifdef WASM_BUILD

#include <utils/image_utils.hpp>

#endif

#include <QDateTime>
#include <QDir>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QImage>

#include <QFile>
#include <QTextStream>

action_classify_alert_save::action_classify_alert_save()
{

}

action_classify_alert_save::~action_classify_alert_save()
{

}

QByteArray const& action_classify_alert_save::get_alert_info() const
{
    return alert_info_;
}

void action_classify_alert_save::change_alert_sender_config(config_alert_sender const &val)
{
    save_reports_ = val.save_reports_;
    send_alert_ = val.activate_;
    send_by_text_ = val.send_by_text_;
}

void action_classify_alert_save::clear_im_name()
{
    im_name_.clear();
}

void action_classify_alert_save::create_dir_path()
{
#ifndef WASM_BUILD
    if(save_reports_){
        dir_path_ = global_keywords().action_classify_alert_path() + "/cam0/" +
                    QDateTime::currentDateTime().toString("yyyy_MM_dd,hh_mm_ss") + "/";
        QDir().mkpath(dir_path_);
    }
#endif
}

void action_classify_alert_save::save_to_json(std::vector<std::tuple<float, size_t> > const &predict_results,
                                              QImage const &img,
                                              int duration_msec)
{
    if(save_reports_ || send_alert_){
        QJsonArray label_arr;
        for(auto const &val : predict_results){
            QJsonObject obj;
            obj["label"] = static_cast<int>(std::get<1>(val));
            label_arr.append(obj);
        }

        QJsonObject output;
        auto const fname = create_fname();
        output["image_name"] = fname;
        output["results"] = label_arr;
        output["duration_msec"] = duration_msec;

#ifdef WASM_BUILD
        output["image"] = QString(flt::to_base64_img(img));
#endif
        if(save_reports_){
            save_to_json();
            img.save(dir_path_ + "/" + im_name_ + ".jpg");
        }

        alert_info_ = QJsonDocument(output).toJson(QJsonDocument::Compact);
    }else{
        alert_info_.clear();
    }
}

bool action_classify_alert_save::send_alert() const noexcept
{
    return send_alert_;
}

bool action_classify_alert_save::send_by_text() const noexcept
{
    return send_by_text_;
}

QString action_classify_alert_save::create_fname() const
{
    return  QString("%1").arg(QDateTime::currentDateTime().toString("hh_mm_ss"));
}

void action_classify_alert_save::save_to_json() const
{
    if(QFile file(dir_path_ + QString("/%1.txt").arg(im_name_)); file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream<<alert_info_<<"\n";
    }
}
