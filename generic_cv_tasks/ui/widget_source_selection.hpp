#ifndef WIDGET_SOURCE_SELECTION_HPP
#define WIDGET_SOURCE_SELECTION_HPP

#include <QWidget>

namespace Ui {
class widget_source_selection;
}

namespace ocv{

enum class stream_source_type;

}

class widget_source_selection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_source_selection(QWidget *parent = nullptr);
    ~widget_source_selection();

    ocv::stream_source_type get_source_type() const noexcept;

    QString get_rtsp_source() const;
    QString get_video_source() const;
    QString get_webcam() const;

private slots:
    void on_pushButtonOpenVideoFolder_clicked();

private:
    Ui::widget_source_selection *ui;    
};

#endif // WIDGET_SOURCE_SELECTION_HPP
