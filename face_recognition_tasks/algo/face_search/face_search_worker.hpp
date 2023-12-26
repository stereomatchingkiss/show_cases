#ifndef FACE_SEARCH_WORKER_HPP
#define FACE_SEARCH_WORKER_HPP

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

class face_search_worker : public flt::mm::frame_process_base_worker
{
public:
    face_search_worker();
    ~face_search_worker();

signals:
    void send_msg(QString msg);

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};

#endif // FACE_SEARCH_WORKER_HPP
