#pragma once

#include <QString>

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

    void clear_im_name();
    void create_dir_path();
    QString create_fpath(QString const &im_name) const;
    QString create_im_name() const;

    void save_to_json(flt::cvt::tracker::track_duration const &val, QString const &im_name);
    void set_stream_name(QString const &name);

private:    
    void save_to_json() const;

    QByteArray alert_info_;
    QString dir_path_;
    QString im_name_;
    std::vector<std::string> names_;    

    QString stream_name_;
};
