#ifndef WIDGET_POSE_ESTIMATION_PARAMS_HPP
#define WIDGET_POSE_ESTIMATION_PARAMS_HPP

#include <QWidget>

namespace Ui {
class widget_pose_estimation_params;
}

struct config_psoe_estimation_worker;

class widget_pose_estimation_params : public QWidget
{
    Q_OBJECT

public:
    explicit widget_pose_estimation_params(QWidget *parent = nullptr);
    ~widget_pose_estimation_params();

    config_psoe_estimation_worker get_config() const;

private:
    Ui::widget_pose_estimation_params *ui;
};

#endif // WIDGET_POSE_ESTIMATION_PARAMS_HPP
