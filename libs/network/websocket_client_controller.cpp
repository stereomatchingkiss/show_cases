#include "websocket_client_controller.hpp"

#include "websocket_client_worker.hpp"

#include <QThread>

namespace flt::net{

websocket_client_controller::websocket_client_controller(QObject *parent)
    : QObject{parent},
    thread_{std::make_unique<QThread>()},
    worker_{new websocket_client_worker}
{
    worker_->moveToThread(thread_.get());

    connect(thread_.get(), &QThread::finished, worker_, &QObject::deleteLater);
    connect(this, &websocket_client_controller::close, worker_, &websocket_client_worker::close);
    connect(this, &websocket_client_controller::create_connection, worker_, &websocket_client_worker::create_connection);
    connect(this, &websocket_client_controller::open, worker_, &websocket_client_worker::open);
    connect(this, &websocket_client_controller::restart_if_needed, worker_, &websocket_client_worker::restart_if_needed);
    connect(this, &websocket_client_controller::send_binary_message, worker_, &websocket_client_worker::send_binary_message);
    connect(this, &websocket_client_controller::send_text_message, worker_, &websocket_client_worker::send_text_message);

    thread_->start();
}

websocket_client_controller::~websocket_client_controller()
{
    worker_->close();
    thread_->quit();
    thread_->wait();
}

}
