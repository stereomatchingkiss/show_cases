#pragma once

#include <QString>

#include <atomic>

class QImage;

namespace flt::cvt::tracker{

struct track_duration;

}

struct config_alert_sender;

class nanodet_alert_save
{
public:
    nanodet_alert_save(std::vector<std::string> names);
    ~nanodet_alert_save();

    QByteArray const& get_alert_info() const;    

    void change_alert_sender_config(const config_alert_sender &val);
    void clear_im_name();
    void create_dir_path();
    void save_to_json(flt::cvt::tracker::track_duration const &val, QImage const &img);

    bool send_by_text() const;

private:
    QString create_fname() const;
    void save_to_json() const;

    QByteArray alert_info_;
    QString dir_path_;
    size_t im_idx_ = 0;
    QString im_name_;
    std::vector<std::string> names_;
    std::atomic<bool> save_reports_ = true;
    std::atomic<bool> send_alert_ = false;
    std::atomic<bool> send_by_text_ = true;
};
