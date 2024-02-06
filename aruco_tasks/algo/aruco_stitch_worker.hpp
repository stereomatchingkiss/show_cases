#ifndef ARUCO_STITCH_WORKER_HPP
#define ARUCO_STITCH_WORKER_HPP

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_aruco_stitch_worker;

class aruco_stitch_worker : public flt::mm::frame_process_base_worker
{
public:
    aruco_stitch_worker(config_aruco_stitch_worker const &config, QObject *parent = nullptr);
    ~aruco_stitch_worker();

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};

#endif // ARUCO_STITCH_WORKER_HPP
