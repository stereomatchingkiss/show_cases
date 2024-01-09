#include "qimage_to_cvmat.hpp"

#include <opencv2/imgproc.hpp>

namespace flt{

std::pair<cv::Mat, bool> qimg_convert_to_cvmat_non_copy(QImage &qimg, bool gray_to_color)
{
    switch(qimg.format()){
    case QImage::Format_RGBA64:
    case QImage::Format_RGBA64_Premultiplied:{
        cv::Mat mat = cv::Mat(qimg.height(), qimg.width(), CV_16UC4, qimg.bits(), qimg.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_RGBA2RGB);
        return {mat, false};
    }
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_RGBA8888_Premultiplied:
    case QImage::Format_ARGB32_Premultiplied:{
        cv::Mat mat = cv::Mat(qimg.height(), qimg.width(), CV_8UC4, qimg.bits(), qimg.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_RGBA2RGB);        
        return {mat, false};
    }
    case QImage::Format_RGB888:
        return {cv::Mat(qimg.height(), qimg.width(), CV_8UC3, qimg.bits(), qimg.bytesPerLine()), true};
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:{
        if(gray_to_color){
            cv::Mat mat = cv::Mat(qimg.height(), qimg.width(), CV_8UC1, qimg.bits(), qimg.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);

            return {mat, false};
        }

        return {cv::Mat(qimg.height(), qimg.width(), CV_8UC1, qimg.bits(), qimg.bytesPerLine()), true};
    }
    default:
        break;
    }

    return {};
}

}
