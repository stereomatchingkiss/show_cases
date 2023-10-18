#ifndef FRAME_CAPTURE_OPENCV_HPP
#define FRAME_CAPTURE_OPENCV_HPP

#include "frame_capture_base_worker.hpp"

#include <memory>

namespace ocv{

struct frame_capture_params;

class frame_capture_opencv_worker : public frame_capture_base_worker
{
    Q_OBJECT
public:
    explicit frame_capture_opencv_worker(frame_capture_params params, QObject *parent = nullptr);
    ~frame_capture_opencv_worker();

    void stop() override;

public slots:
    virtual void start() override;    

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FRAME_CAPTURE_OPENCV_HPP
