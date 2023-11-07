#include "frame_capture_websocket.hpp"

#include "frame_capture_websocket_params.hpp"

#include "../camera/frame_process_controller.hpp"

#include "../../utils/error_log.hpp"
#include "../../utils/image_utils.hpp"

#include <QDebug>
#include <QWebSocket>

#include <mutex>

namespace flt::mm{

struct frame_capture_websocket::impl
{
    impl(frame_capture_websocket_params const &params) :
        params_{params}
    {
        socket_.open(params.url_);
    }

    void remove(void *key)
    {
        auto func = [key](auto const &val)
        {
            return key == val.second;
        };
        if(auto it = std::ranges::find_if(controllers_, func); it != std::end(controllers_)){
            controllers_.erase(it);
        }
    }

    std::vector<std::pair<std::shared_ptr<frame_process_controller>, void*>> controllers_;
    frame_capture_websocket_params params_;
    QWebSocket socket_;
};

flt::mm::frame_capture_websocket::frame_capture_websocket(frame_capture_websocket_params const &params, QObject *parent) :
    single_frame_with_multi_worker_base{parent},
    impl_{std::make_unique<impl>(params)}
{
    connect(&impl_->socket_, &QWebSocket::binaryMessageReceived, this, &frame_capture_websocket::binary_message_received);
    connect(&impl_->socket_, &QWebSocket::connected, this, &frame_capture_websocket::connected);
    connect(&impl_->socket_, &QWebSocket::disconnected, this, &frame_capture_websocket::closed);
    connect(&impl_->socket_, &QWebSocket::errorOccurred, this, &frame_capture_websocket::socket_error);
#ifndef WASM_BUILD
    connect(&impl_->socket_, &QWebSocket::sslErrors, this, &frame_capture_websocket::ssl_errors);
#endif
}

frame_capture_websocket::~frame_capture_websocket()
{

}

void frame_capture_websocket::add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key)
{    
    impl_->controllers_.emplace_back(process_controller, key);
}

void frame_capture_websocket::remove_listener(void *key)
{
    impl_->remove(key);
}

void frame_capture_websocket::start()
{
    impl_->socket_.open(impl_->params_.url_);
}

void frame_capture_websocket::stop()
{
    impl_->socket_.close();
}

#ifndef WASM_BUILD
void frame_capture_websocket::alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description)
{
    qDebug()<<flt::create_error_msg(std::source_location()).c_str()<<"alert received. level = "
             <<static_cast<int>(level)<<", type = "<<static_cast<int>(type)<<", desc = "<<description;
}

void frame_capture_websocket::ssl_errors(const QList<QSslError> &errors)
{
    qDebug()<<flt::create_error_msg(std::source_location()).c_str();
    for(auto const &val : errors){
        qDebug()<<"ssl errors : "<<val.errorString();
    }
}

#endif

void frame_capture_websocket::binary_message_received(QByteArray message)
{
    auto img = decode_qbyte_array_to_qimg(message, "JPG");
    for(auto &val : impl_->controllers_){
        emit val.first->process_results(img);
    }
}

void frame_capture_websocket::closed()
{    
    qDebug()<<flt::create_error_msg(std::source_location()).c_str()<<"closed";
}

void frame_capture_websocket::connected()
{
    qDebug()<<flt::create_error_msg(std::source_location()).c_str()<<"connected";
}

void frame_capture_websocket::socket_error(QAbstractSocket::SocketError error)
{
    qDebug()<<flt::create_error_msg(std::source_location()).c_str()<<error;
}

}
