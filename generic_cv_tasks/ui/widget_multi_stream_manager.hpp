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

private:
    Ui::widget_multi_stream_manager *ui;

    QGridLayout *glayout_;
    std::vector<QWidget*> streams_;
};
