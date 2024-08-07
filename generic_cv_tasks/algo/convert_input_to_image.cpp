#include "convert_input_to_image.hpp"

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>
#include <multimedia/stream_enum.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QImage>
#include <QRectF>

using namespace flt::cvt;

namespace{

QImage crop_qimg(QImage const &img, QRectF const &roi)
{
    if(roi.isValid()){
        QRect const roi_scaled(roi.x() * img.width(), roi.y() * img.height(),
                               roi.width() * img.width(), roi.height() * img.height());
        return img.copy(roi_scaled);
    }

    return img.copy();
}

}

std::pair<cv::Mat, QImage> convert_std_any_to_image(std::any frame, flt::mm::stream_source_type source_type, QRectF const &roi)
{
    cv::Mat mat;
    QImage qimg;
    if(source_type != flt::mm::stream_source_type::rtsp){
        qimg = std::any_cast<QImage>(frame).convertToFormat(QImage::Format_RGB888);
        qimg = crop_qimg(qimg, roi);
        mat = cv::Mat(qimg.height(), qimg.width(), CV_8UC3, qimg.bits(), qimg.bytesPerLine());
    }else{
        mat = std::any_cast<cv::Mat>(frame);
        if(mat.channels() == 3){
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        }else if(mat.channels() == 1){
            cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);
        }

        if(roi.isValid()){
            mat = mat(convert_qrectf_to_cv_rect(roi, mat.cols, mat.rows)).clone();
        }
        qimg = QImage((uchar*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    }

    return {mat, qimg};
}
