#ifndef WIDGET_ESTIMATE_MANY_POSE_SIMILARITY_PARAMS_HPP
#define WIDGET_ESTIMATE_MANY_POSE_SIMILARITY_PARAMS_HPP

#include <QWidget>

namespace Ui {
class widget_estimate_many_pose_similarity_params;
}

struct config_widget_estimate_many_pose_similarity_params;

class widget_estimate_many_pose_similarity_params : public QWidget
{
    Q_OBJECT

public:
    explicit widget_estimate_many_pose_similarity_params(QWidget *parent = nullptr);
    ~widget_estimate_many_pose_similarity_params();

    config_widget_estimate_many_pose_similarity_params get_config() const;

private slots:
    void on_pushButtonImages_clicked();

    void on_pushButtonData_clicked();

private:
    Ui::widget_estimate_many_pose_similarity_params *ui;
};

#endif // WIDGET_ESTIMATE_MANY_POSE_SIMILARITY_PARAMS_HPP
