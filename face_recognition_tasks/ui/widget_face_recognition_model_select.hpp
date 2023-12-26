#ifndef WIDGET_FACE_RECOGNITION_MODEL_SELECT_HPP
#define WIDGET_FACE_RECOGNITION_MODEL_SELECT_HPP

#include <QWidget>

namespace Ui {
class widget_face_recognition_model_select;
}

struct config_face_recognition_model_select;

class widget_face_recognition_model_select : public QWidget
{
    Q_OBJECT

public:
    explicit widget_face_recognition_model_select(QWidget *parent = nullptr);
    ~widget_face_recognition_model_select();

    config_face_recognition_model_select get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    Ui::widget_face_recognition_model_select *ui;
};

#endif // WIDGET_FACE_RECOGNITION_MODEL_SELECT_HPP
