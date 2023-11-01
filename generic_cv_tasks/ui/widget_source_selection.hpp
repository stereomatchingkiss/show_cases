#ifndef WIDGET_SOURCE_SELECTION_HPP
#define WIDGET_SOURCE_SELECTION_HPP

#include <QWidget>

namespace Ui {
class widget_source_selection;
}

namespace flt::mm{

enum class stream_source_type;

struct frame_capture_params;

}

struct config_source_selection;

class widget_source_selection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_source_selection(QWidget *parent = nullptr);
    ~widget_source_selection();

    flt::mm::frame_capture_params get_frame_capture_params() const;
    flt::mm::stream_source_type get_source_type() const noexcept;

    int get_max_fps() const noexcept;
    QString get_rtsp_source() const noexcept;
    QString get_video_source() const noexcept;
    QString get_webcam() const noexcept;
    QString get_url() const noexcept;

    config_source_selection get_config() const;
    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private slots:
    void on_pushButtonOpenVideoFolder_clicked();

private:
    void update_webcam_index();

    Ui::widget_source_selection *ui;    
};

#endif // WIDGET_SOURCE_SELECTION_HPP
