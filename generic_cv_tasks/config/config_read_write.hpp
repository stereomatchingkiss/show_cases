#ifndef CONFIG_READ_WRITE_HPP
#define CONFIG_READ_WRITE_HPP

#include <QJsonObject>

class config_read_write
{
public:
    void set_roi(QJsonObject val);
    void set_widget_object_detect_model_select(QJsonObject val);
    void set_widget_select_object_to_detect(QJsonObject val);
    void set_widget_source_selection(QJsonObject val);
    void set_widget_tracker_alert(QJsonObject val);

    QJsonObject read(QString const &val);
    void write(QString const &val);

private:
    QJsonObject roi_;
    QJsonObject widget_object_detect_model_select_;
    QJsonObject widget_select_object_to_detect_;
    QJsonObject widget_source_selection_;
    QJsonObject widget_tracker_alert_;
};

#endif // CONFIG_READ_WRITE_HPP
