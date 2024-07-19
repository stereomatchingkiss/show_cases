#pragma once

#include <QObject>

namespace flt::mm{

class single_frame_with_multi_worker_base;

}

class widget_roi_selection;
class widget_source_selection;
class widget_stream_player;

class QTimer;

/**
 * @todo Give it a better name
 */
class frame_capture_creator : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_creator(widget_source_selection *source_selection,
                                   widget_stream_player *stream_player,
                                   QObject *parent = nullptr);
    ~frame_capture_creator();

    void create_frame_capture();
    void create_roi_select_stream(widget_roi_selection *roi_selection);

    flt::mm::single_frame_with_multi_worker_base* get_sfwmw() noexcept;

    void reset();

private:    
    void update_position();

    QTimer *timer_;

    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
};
