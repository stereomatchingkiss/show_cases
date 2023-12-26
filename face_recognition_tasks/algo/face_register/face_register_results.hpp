#ifndef FACE_REGISTER_RESULTS_HPP
#define FACE_REGISTER_RESULTS_HPP

#include <QImage>

#include <vector>

struct face_register_results
{
    std::vector<QImage> faces_;
    std::vector<std::vector<float>> features_;
};

#endif // FACE_REGISTER_RESULTS_HPP
