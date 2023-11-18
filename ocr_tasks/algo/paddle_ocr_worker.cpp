#include "paddle_ocr_worker.hpp"

#include "../config/config_paddle_ocr_worker.hpp"

using namespace flt::mm;

struct paddle_ocr_worker::impl
{
    impl(config_paddle_ocr_worker params) :
        params_{std::move(params)}
    {

    }

    config_paddle_ocr_worker params_;
};

paddle_ocr_worker::paddle_ocr_worker(config_paddle_ocr_worker params, QObject *parent) :
    frame_process_base_worker(2, parent)
{

}

paddle_ocr_worker::~paddle_ocr_worker()
{

}

void paddle_ocr_worker::process_results(std::any frame)
{

}
