#ifndef WIDGET_ACTION_CLASSIFY_MODEL_SELECT_HPP
#define WIDGET_ACTION_CLASSIFY_MODEL_SELECT_HPP

#include <QWidget>

namespace Ui {
class widget_action_classify_model_select;
}

struct config_action_classify_model_select;

class widget_action_classify_model_select : public QWidget
{
    Q_OBJECT

public:
    explicit widget_action_classify_model_select(QWidget *parent = nullptr);
    ~widget_action_classify_model_select();

    config_action_classify_model_select get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    Ui::widget_action_classify_model_select *ui;    
};

#endif // WIDGET_ACTION_CLASSIFY_MODEL_SELECT_HPP
