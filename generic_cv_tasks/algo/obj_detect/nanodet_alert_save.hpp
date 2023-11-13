#pragma once

#include <QString>

#ifndef WASM_BUILD
#include <QFile>
#include <QTextStream>
#endif

#include <atomic>

class QImage;

namespace flt::cvt::tracker{

struct track_duration;

}

struct config_alert_sender;

class nanodet_alert_save
{
public:
    nanodet_alert_save(std::vector<std::string> const &names);
    ~nanodet_alert_save();

    QByteArray const& get_alert_info() const;
    QString const& get_im_name() const;

    void change_alert_sender_config(const config_alert_sender &val);
    void clear_im_name();    
    void open_file();
    void save_to_json(flt::cvt::tracker::track_duration const &val, QImage const &img, size_t im_ids);

    bool send_by_text() const;

private:
    QString create_fname(size_t im_ids) const;

    QByteArray alert_info_;
    QString dir_path_;
    QString im_name_;
    size_t im_ids_ = 0;
    std::vector<std::string> const &names_;
    std::atomic<bool> save_reports_ = true;
    std::atomic<bool> send_alert_ = false;
    std::atomic<bool> send_by_text_ = true;

#ifndef WASM_BUILD
    QFile file_;
    QTextStream stream_;
#endif
};
