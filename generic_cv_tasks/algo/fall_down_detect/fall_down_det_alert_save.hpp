#pragma once

#include <QString>

#include <atomic>
#include <set>

struct config_alert_sender;

class QImage;

class fall_down_det_alert_save
{
public:
    explicit fall_down_det_alert_save(std::vector<std::string> label_names);
    ~fall_down_det_alert_save();

    QByteArray const& get_alert_info() const;

    void change_alert_sender_config(config_alert_sender const &val);    
    void create_dir_path();

    QString save_to_json(QImage const &img);
    bool send_alert() const noexcept;
    bool send_by_text() const noexcept;

private:
    QString create_fname() const;
    void save_to_json() const;

    QByteArray alert_info_;
    QString dir_path_;
    std::vector<std::string> label_names_;
    size_t im_idx_ = 0;
    QString im_name_;
    std::vector<std::string> names_;    
    std::atomic<bool> save_reports_ = true;
    std::atomic<bool> send_alert_ = false;
    std::atomic<bool> send_by_text_ = true;
};
