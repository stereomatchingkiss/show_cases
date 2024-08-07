#ifndef WIDGET_SOURCE_SELECTION_HPP
#define WIDGET_SOURCE_SELECTION_HPP

#include <QCameraDevice>
#include <QMediaDevices>
#include <QWidget>

namespace Ui {
class widget_source_selection;
}

namespace flt::mm{

enum class stream_source_type;

struct frame_capture_params;
struct frame_capture_websocket_params;
struct frame_capture_opencv_params;
struct frame_capture_qcamera_params;
struct frame_capture_qmediaplayer_params;

}

struct config_source_selection;

class widget_source_selection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_source_selection(QWidget *parent = nullptr);
    ~widget_source_selection();
    
    flt::mm::frame_capture_qmediaplayer_params get_frame_capture_qmediaplayer_params() const;    
    flt::mm::frame_capture_qcamera_params get_frame_capture_qcamera_params() const;
    flt::mm::frame_capture_opencv_params get_frame_capture_rtsp_params() const;
    flt::mm::frame_capture_websocket_params get_frame_capture_websocket_params() const noexcept;
    flt::mm::stream_source_type get_source_type() const noexcept;    

    bool get_is_valid_source() const noexcept;
    int get_max_fps() const noexcept;
    QString get_rtsp_source() const noexcept;
    QString get_video_source() const noexcept;
    QString get_webcam() const noexcept;
    QString get_url() const noexcept;

    config_source_selection get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);
    void set_websocket_url(QString const &val);

private slots:
    void on_pushButtonOpenVideoFolder_clicked();

private:
    /**
     * dummy is a parameter for Qt signal and slot connection, true or false do not matter
     */
    void set_max_fps_visible(bool dummy = false);
    void update_webcam_box();

    Ui::widget_source_selection *ui;

    QMediaDevices devices_;
    QList<QCameraDevice> cam_devices_;
    mutable QByteArray video_contents_;
};

#endif // WIDGET_SOURCE_SELECTION_HPP
