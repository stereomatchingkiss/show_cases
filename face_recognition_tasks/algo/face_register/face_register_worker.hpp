#ifndef FACE_REGISTER_WORKER_HPP
#define FACE_REGISTER_WORKER_HPP

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

class face_register_worker : public flt::mm::frame_process_base_worker
{
public:
    face_register_worker();
    ~face_register_worker();

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};

#endif // FACE_REGISTER_WORKER_HPP
