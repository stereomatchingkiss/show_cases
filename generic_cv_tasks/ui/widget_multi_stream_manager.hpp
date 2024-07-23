#pragma once

#include <QWidget>

#include <vector>

namespace Ui {
class widget_multi_stream_manager;
}

class QGridLayout;

class widget_multi_stream_manager : public QWidget
{
    Q_OBJECT

public:
    explicit widget_multi_stream_manager(QWidget *parent = nullptr);
    ~widget_multi_stream_manager();

    void add_stream(QWidget *widget);
    void delete_stream();

    int get_stream_count() const;

    void load_settings();
    void save_settings();

private:
    void delete_streams();
    QJsonArray dump_stacks_states() const;
    QJsonObject dump_settings() const;

    //These functions need to give them different names because action signal from main window need to call
    //load_settings and save_settings
    //QJsonObject is needed because of wasm
    void load_settings_from_files(QJsonObject const &jobj);
    void load_settings_from_files(QString const &fname);
    void save_settings_to_file(QString const &save_at);

    Ui::widget_multi_stream_manager *ui;

    QGridLayout *glayout_;
    std::vector<QWidget*> streams_;
};
