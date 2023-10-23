#pragma once

#include <Eigen/Dense>

namespace byte_track
{

template<typename T>
using Tlwh = Eigen::Matrix<T, 1, 4, Eigen::RowMajor>;

template<typename T>
using Tlbr = Eigen::Matrix<T, 1, 4, Eigen::RowMajor>;

template<typename T>
using Xyah = Eigen::Matrix<T, 1, 4, Eigen::RowMajor>;

template<typename T>
class Rect
{
public:
    Tlwh<T> tlwh;

    Rect() = default;
    Rect(const T &x, const T &y, const T &width, const T &height) :
        tlwh({x, y, width, height})
    {
    }

    T x() const
    {
        return tlwh[0];
    }

    T y() const
    {
        return tlwh[1];
    }

    T width() const
    {
        return tlwh[2];
    }

    T height() const
    {
        return tlwh[3];
    }

    T& x()
    {
        return tlwh[0];
    }

    T& y()
    {
        return tlwh[1];
    }

    T& width()
    {
        return tlwh[2];
    }

    T& height()
    {
        return tlwh[3];
    }

    T tl_x() const
    {
        return tlwh[0];
    }

    T tl_y() const
    {
        return tlwh[1];
    }

    T br_x() const
    {
        return tlwh[0] + tlwh[2];
    }

    T br_y() const
    {
        return tlwh[1] + tlwh[3];
    }

    Tlbr<T> getTlbr() const
    {
        return {
            tlwh[0],
            tlwh[1],
            tlwh[0] + tlwh[2],
            tlwh[1] + tlwh[3],
        };
    }

    Xyah<T> getXyah() const
    {
        return {
            tlwh[0] + tlwh[2] / 2,
            tlwh[1] + tlwh[3] / 2,
            tlwh[2] / tlwh[3],
            tlwh[3],
        };
    }

    float calcIoU(Rect<T> const &other) const
    {
        float box_area = (other.tlwh[2] + 1) * (other.tlwh[3] + 1);
        float iw = std::min(tlwh[0] + tlwh[2], other.tlwh[0] + other.tlwh[2]) - std::max(tlwh[0], other.tlwh[0]) + 1;
        float iou = 0;
        if (iw > 0)
        {
            float ih = std::min(tlwh[1] + tlwh[3], other.tlwh[1] + other.tlwh[3]) - std::max(tlwh[1], other.tlwh[1]) + 1;
            if (ih > 0)
            {
                float ua = (tlwh[0] + tlwh[2] - tlwh[0] + 1) * (tlwh[1] + tlwh[3] - tlwh[1] + 1) + box_area - iw * ih;
                iou = iw * ih / ua;
            }
        }
        return iou;
    }


};

template<typename T>
Rect<T> generate_rect_by_tlbr(const byte_track::Tlbr<T>& tlbr)
{
    return byte_track::Rect<T>(tlbr[0], tlbr[1], tlbr[2] - tlbr[0], tlbr[3] - tlbr[1]);
}

template<typename T>
Rect<T> generate_rect_by_xyah(const byte_track::Xyah<T>& xyah)
{
    const auto width = xyah[2] * xyah[3];
    return byte_track::Rect<T>(xyah[0] - width / 2, xyah[1] - xyah[3] / 2, width, xyah[3]);
}

}
