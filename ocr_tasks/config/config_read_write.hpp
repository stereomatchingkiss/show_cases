#ifndef CONFIG_READ_WRITE_HPP
#define CONFIG_READ_WRITE_HPP

#include <QJsonObject>

class config_read_write
{
public:
    void set_widget_source_selection(QJsonObject val);
    void set_widget_stream_player(QJsonObject val);

    QJsonObject dumps() const;
    QJsonObject read(QString const &val);
    QJsonObject read(QByteArray const &val);
    void write(QString const &val);
    void write(QJsonObject const &obj, QString const &val);

private:
    QJsonObject widget_source_selection_;
    QJsonObject widget_stream_player_;
};

#endif // CONFIG_READ_WRITE_HPP
