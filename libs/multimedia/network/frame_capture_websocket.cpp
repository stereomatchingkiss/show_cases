#include "frame_capture_websocket.hpp"

#include "frame_capture_websocket_params.hpp"

#include "../camera/frame_process_controller.hpp"

#include <QDebug>
#include <QWebSocket>

#ifndef WASM_BUILD
#include <opencv2/imgcodecs.hpp>
#endif

#include <mutex>

namespace flt::mm{

struct frame_capture_websocket::impl
{
    impl(frame_capture_websocket_params const &params) :        
        params_{params}
    {        
    }

    void process_image(cv::Mat mat)
    {
        if(!mat.empty()){
            for(auto &val : controllers_){
                val.first->predict(mat);
            }
        }else{
            qDebug()<<__func__<<":cannot decode message";
        }
    }

#ifdef WASM_BUILD
    void process_image(QImage mat)
    {
        if(!mat.isNull()){
            for(auto &val : impl_->controllers_){
                emit val.first->predict(mat);
            }
        }else{
            qDebug()<<__func__<<":cannot decode message";
        }
    }
#endif

    void binary_message_received(QByteArray message)
    {
#ifndef WASM_BUILD
        auto img = cv::imdecode(cv::Mat(1, message.length(), CV_8UC1, (uchar*)message.data()), cv::ImreadModes::IMREAD_COLOR);
        process_image(img);
#else
        process_image(QImage::fromData(message));
#endif
    }

    void text_message_received(QString message)
    {
#ifndef WASM_BUILD
        auto img = cv::imdecode(cv::Mat(1, message.length(), CV_8UC1, (uchar*)message.toLatin1().data()), cv::ImreadModes::IMREAD_COLOR);
        process_image(img);
#else
        process_image(QImage::fromData(message.toLatin1()));
#endif
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
    connect(&impl_->socket_, &QWebSocket::textMessageReceived, this, &frame_capture_websocket::text_message_received);
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
    qDebug()<<__func__<<"alert received. level = "
             <<static_cast<int>(level)<<", type = "<<static_cast<int>(type)<<", desc = "<<description;
}

void frame_capture_websocket::ssl_errors(const QList<QSslError> &errors)
{
    qDebug()<<__func__;
    for(auto const &val : errors){
        qDebug()<<"ssl errors : "<<val.errorString();
    }
}

#endif

void frame_capture_websocket::binary_message_received(QByteArray message)
{
    impl_->binary_message_received(message);
}

void frame_capture_websocket::text_message_received(QString message)
{
    impl_->text_message_received(message);
}

void frame_capture_websocket::closed()
{
    qDebug()<<__func__<<":closed";
}

void frame_capture_websocket::connected()
{
    qDebug()<<__func__<<":connected";
}

void frame_capture_websocket::socket_error(QAbstractSocket::SocketError error)
{
    qDebug()<<__func__<<":"<<error;
}

}
