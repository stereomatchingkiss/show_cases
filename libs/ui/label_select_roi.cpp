#include "label_select_roi.hpp"

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QMouseEvent>
#include <QResizeEvent>
#include <QRubberBand>

namespace ocv::ui{

QString const state_norm_roi("state_label_select_roi_norm_roi");

label_select_roi::label_select_roi(QString const &text, QWidget *parent) :
    QLabel(text, parent)
{
    rband_ = new QRubberBand(QRubberBand::Rectangle, this);    
}

label_select_roi::~label_select_roi()
{    
}

void label_select_roi::display_frame(std::any img)
{
    setPixmap(std::any_cast<QPixmap>(img).scaled(width(), height(), Qt::KeepAspectRatio));
}

QRectF label_select_roi::get_rubber_band_rect() const
{
    if(auto const rrect = rband_->rect(); rrect.width() > 0 && !mouse_press_point_.isNull()){
        auto const widget_size = size();
        QRectF norm_rect;
        norm_rect.setX(mouse_press_point_.x() / widget_size.width());
        norm_rect.setY(mouse_press_point_.y() / widget_size.height());
        norm_rect.setWidth(rrect.width() / static_cast<double>(widget_size.width()));
        norm_rect.setHeight(rrect.height() / static_cast<double>(widget_size.height()));

        return norm_rect;
    }

    return norm_roi_;
}

QJsonObject label_select_roi::get_states() const
{
    QJsonArray norm_vals;
    auto const norm_roi = get_rubber_band_rect();
    norm_vals.push_back(norm_roi.x());
    norm_vals.push_back(norm_roi.y());
    norm_vals.push_back(norm_roi.width());
    norm_vals.push_back(norm_roi.height());

    QJsonObject obj;
    obj[state_norm_roi] = norm_vals;

    return obj;
}

void label_select_roi::set_states(const QJsonObject &val)
{
    if(val.contains(state_norm_roi)){
        auto const norm_vals = val[state_norm_roi].toArray();
        norm_roi_.setX(norm_vals[0].toDouble());
        norm_roi_.setY(norm_vals[1].toDouble());
        norm_roi_.setWidth(norm_vals[2].toDouble());
        norm_roi_.setHeight(norm_vals[3].toDouble());

        auto const widget_size = size();
        auto const top_left = QPoint(int(norm_roi_.x() * widget_size.width()), int(norm_roi_.y() * widget_size.height()));
        auto const bottom_right = QPoint(int(norm_roi_.bottomRight().x() * widget_size.width()), int(norm_roi_.bottomRight().y() * widget_size.height()));
        auto const geometry = QRect(top_left, bottom_right).normalized();

        rband_->resize(geometry.size());
        rband_->setGeometry(geometry);
        rband_->show();
    }
}

void label_select_roi::show_rubber_band()
{
    if(!rband_->rect().isEmpty()){
        if(norm_roi_.isEmpty()){
            auto const norm_roi = get_rubber_band_rect();
            show_rubber_band(norm_roi);
        }else{
            show_rubber_band(norm_roi_);
        }
    }
}

void label_select_roi::show_rubber_band(const QRectF &norm_roi)
{
    auto const widget_size = size();
    auto const top_left = QPoint(int(norm_roi.x() * widget_size.width()), int(norm_roi.y() * widget_size.height()));
    auto const bottom_right = QPoint(int(norm_roi.bottomRight().x() * widget_size.width()), int(norm_roi.bottomRight().y() * widget_size.height()));
    auto const geometry = QRect(top_left, bottom_right).normalized();

    rband_->resize(geometry.size());
    rband_->setGeometry(geometry);
    rband_->show();
}

void label_select_roi::mouseMoveEvent(QMouseEvent *event)
{
    rband_->setGeometry(QRect(mouse_press_point_.toPoint(), event->position().toPoint()).normalized());    
}

void label_select_roi::mousePressEvent(QMouseEvent *event)
{
    norm_roi_ = QRectF();
    rband_->setGeometry(QRect());
    mouse_press_point_ = event->position();
    rband_->setGeometry(QRect(mouse_press_point_.toPoint(), QSize()).normalized());
    rband_->show();
}

void label_select_roi::resizeEvent(QResizeEvent *event)
{    
    show_rubber_band();
    QLabel::resizeEvent(event);
}

}
