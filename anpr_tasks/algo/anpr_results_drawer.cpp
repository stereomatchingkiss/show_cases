#include "anpr_results_drawer.hpp"

#include "../global/global_object.hpp"

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>

#include <QPainter>

using namespace flt::cvt;

anpr_results_drawer::anpr_results_drawer()
{
    pen_plate_.setColor(Qt::darkRed);
    pen_text_.setColor(Qt::yellow);
    pen_text_.setWidth(10);
    pen_vechile_.setColor(Qt::green);
}

void anpr_results_drawer::draw(QImage &img, const anpr_algo_predict_results &result)
{
    QPainter painter(&img);

    painter.setPen(pen_vechile_);
    painter.drawRect(convert_cv_rect_to_qrect(result.vehicle_rect_));
    if(!result.plate_rect_.empty()){
        painter.setPen(pen_plate_);
        painter.drawRect(convert_cv_rect_to_qrect(result.plate_rect_));
        painter.setPen(pen_text_);

        auto font = QFont(global_object().font_family());
        font.setPixelSize(img.width() / 10);
        painter.setFont(font);

        painter.drawText(QPoint(result.plate_rect_.x, result.plate_rect_.y - 5), result.plate_num_);
    }
}

void anpr_results_drawer::draw(QImage &img, const std::vector<anpr_algo_predict_results> &results)
{
    QPainter painter(&img);

    auto font = QFont(global_object().font_family());
    font.setPixelSize(img.width() / 10);
    painter.setFont(font);

    for(auto const &val : results){
        painter.setPen(pen_vechile_);
        painter.drawRect(convert_cv_rect_to_qrect(val.vehicle_rect_));
        if(!val.plate_rect_.empty()){
            painter.setPen(pen_plate_);
            painter.drawRect(convert_cv_rect_to_qrect(val.plate_rect_));
            painter.setPen(pen_text_);
            painter.drawText(QPoint(val.plate_rect_.x, val.plate_rect_.y - 5), val.plate_num_);
        }
    }
}
