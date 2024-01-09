#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_anpr_worker;

class anpr_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit anpr_worker(config_anpr_worker const &config, QObject *parent = nullptr);
    ~anpr_worker();

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};
