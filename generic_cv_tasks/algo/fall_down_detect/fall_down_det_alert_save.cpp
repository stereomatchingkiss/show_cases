#include "fall_down_det_alert_save.hpp"

#include "../../config/config_alert_sender.hpp"

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

fall_down_det_alert_save::fall_down_det_alert_save(std::vector<std::string> label_names) :
    label_names_{std::move(label_names)}
{

}

fall_down_det_alert_save::~fall_down_det_alert_save()
{

}

const QByteArray &fall_down_det_alert_save::get_alert_info() const
{
    return alert_info_;
}

void fall_down_det_alert_save::change_alert_sender_config(config_alert_sender const &val)
{
    save_reports_ = val.save_reports_;
    send_alert_ = val.activate_;
    send_by_text_ = val.send_by_text_;
    create_dir_path();
}

void fall_down_det_alert_save::create_dir_path()
{
#ifndef WASM_BUILD
    if(save_reports_){
        dir_path_ = global_keywords().fall_down_obj_det_alert_path() + "/cam0/" +
                    QDateTime::currentDateTime().toString("yyyy_MM_dd") + "/";
        QDir().mkpath(dir_path_);
    }
#endif
}

void fall_down_det_alert_save::save_to_json(QImage const &img)
{    
    if(save_reports_ || send_alert_){        
        create_dir_path();
        im_name_ = create_fname();
        QJsonObject jobj;
        jobj["image_name"] = im_name_;
        jobj["image"] = QString(flt::to_base64_img(img));
        jobj["time"] = QDateTime::currentDateTime().toString("yyyy_MM_dd,hh_mm_ss");

        alert_info_ = QJsonDocument(jobj).toJson(QJsonDocument::Compact);
#ifndef WASM_BUILD
        if(save_reports_){            
            save_to_json();
            img.save(dir_path_ + "/" + im_name_ + ".jpg");
        }
#endif
    }
}

bool fall_down_det_alert_save::send_alert() const noexcept
{
    return send_alert_;
}

bool fall_down_det_alert_save::send_by_text() const noexcept
{
    return send_by_text_;
}

QString fall_down_det_alert_save::create_fname() const
{
    return  QString("%1").arg(QDateTime::currentDateTime().toString("hh_mm_ss"));
}

void fall_down_det_alert_save::save_to_json() const
{
    if(QFile file(dir_path_ + QString("/%1.txt").arg(im_name_)); file.open(QIODevice::WriteOnly)){        
        QTextStream stream(&file);
        stream<<alert_info_<<"\n";
    }
}

