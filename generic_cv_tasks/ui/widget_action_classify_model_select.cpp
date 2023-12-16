#include "widget_action_classify_model_select.hpp"
#include "ui_widget_action_classify_model_select.h"

#include "../config/config_action_classify_model_select.hpp"

#include <QJsonObject>

namespace{

QString const state_action_classify_confidence("state_action_classify_confidence");
QString const state_action_classify_model_type("state_action_classify_model_type");
QString const state_action_classify_top_k("state_action_classify_top_k");

QString const state_version("state_version");

struct model_types
{
    QString const pptsm_v2_ = "pptsm_v2_";

    int get_ids(QString const &val) const
    {
        if(val == pptsm_v2_){
            return 0;
        }

        return 0;
    }
};

}

widget_action_classify_model_select::widget_action_classify_model_select(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_action_classify_model_select)
{
    ui->setupUi(this);

    ui->comboBoxSelectModel->addItem("ppstm_v2");
    ui->groupBoxSelectModel->setVisible(false);

    ui->labelConfidence->setToolTip(tr("The higher the value, the more likely the model is to correctly identify the correct results.\n"
                                       "However, this can also lead to fewer of the correct results being identified."));
    ui->labelTopK->setToolTip(tr("Check if the target label is one of your top 5 predictions"));
}

widget_action_classify_model_select::~widget_action_classify_model_select()
{
    delete ui;
}

config_action_classify_model_select widget_action_classify_model_select::get_config() const
{
    config_action_classify_model_select config;
    config.confidence_ = ui->spinBoxConfidence->value() / 100.0f;
    config.model_ = action_classify_model_enum::pptsm_v2;
    config.top_k_ = ui->spinBoxTopK->value();

    return config;
}

QJsonObject widget_action_classify_model_select::get_states() const
{
    QJsonObject obj;
    obj[state_action_classify_confidence] = ui->spinBoxConfidence->value();
    obj[state_action_classify_model_type] = ui->comboBoxSelectModel->currentText();
    obj[state_action_classify_top_k] = ui->spinBoxTopK->value();

    return obj;
}
