#ifndef FACE_SEARCH_WORKER_RESULTS_HPP
#define FACE_SEARCH_WORKER_RESULTS_HPP

#include <QImage>

#include <QJsonObject>

struct face_search_worker_results
{
    QJsonObject obj;
    QImage img;
};

#endif // FACE_SEARCH_WORKER_RESULTS_HPP
