#pragma once

#include "single_frame_with_multi_worker_base.hpp"

#include <memory>

class QVideoFrame;

namespace flt::mm{

struct frame_capture_qt_params;

class frame_capture_qmediaplayer : public single_frame_with_multi_worker_base
{
    Q_OBJECT
public:
    explicit frame_capture_qmediaplayer(frame_capture_qt_params const &params, QObject *parent = nullptr);
    ~frame_capture_qmediaplayer();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key) override;
    void remove_listener(void *key)  override;
    void start() override;

    bool is_seekable() const noexcept;
    int position() const noexcept;

    void set_position(qint64 position);

signals:
    void duration_changed(qint64 duration);

private:
    void tracks_changed();
    void video_frame_changed(QVideoFrame const &frame);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}
