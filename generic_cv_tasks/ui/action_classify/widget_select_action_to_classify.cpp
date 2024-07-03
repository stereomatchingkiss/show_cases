#include "widget_select_action_to_classify.hpp"
#include "ui_widget_select_action_to_classify.h"

#include "../../algo/action_classify/kinetic_400_labels.hpp"
#include "../../config/config_select_action_to_classify.hpp"

#include <ui/utils/qtable_item_utils.hpp>

#include <QJsonArray>
#include <QJsonObject>

#include <QCheckBox>

#include <QTimer>

using namespace flt::ui;

namespace{

inline QString state_kinetic_400_selected_items(){ return "state_kinetic_400_selected_items"; };

}

widget_select_action_to_classify::widget_select_action_to_classify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_select_action_to_classify),
    timer_{new QTimer}
{
    ui->setupUi(this);

    connect(timer_, &QTimer::timeout, this, &widget_select_action_to_classify::search_similar_keywords);

    timer_->setInterval(250);
    timer_->setSingleShot(true);

    auto const &labels = kinetic_400_labels::get_labels();
    for(int i = 0; i != labels.size(); ++i){
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(labels[i]));
        ui->tableWidget->setCellWidget(i, 1, create_checkbox());
    }
    ui->tableWidget->resizeColumnsToContents();
}

widget_select_action_to_classify::~widget_select_action_to_classify()
{
    delete ui;
}

config_select_action_to_classify widget_select_action_to_classify::get_config() const
{
    config_select_action_to_classify config;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        config.selected_object_.emplace_back(access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->isChecked());
    }

    return config;
}

QJsonObject widget_select_action_to_classify::get_states() const
{
    QJsonObject obj;
    QJsonArray selected_items;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        selected_items.push_back(access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->isChecked());
    }
    obj[state_kinetic_400_selected_items()] = selected_items;

    return obj;
}

void widget_select_action_to_classify::set_states(QJsonObject const &val)
{
    if(val.contains(state_kinetic_400_selected_items())){
        auto const arr = val[state_kinetic_400_selected_items()].toArray();
        for(int i = 0; i != arr.size(); ++i){
            access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->setChecked(arr[i].toBool());
        }
    }
}

void widget_select_action_to_classify::on_lineEditSearchAction_textChanged(const QString &)
{
    timer_->start();
}

void widget_select_action_to_classify::search_similar_keywords()
{
    auto const &labels = kinetic_400_labels::get_labels();
    for(size_t i = 0; i != labels.size(); ++i){
        if(labels[i].contains(ui->lineEditSearchAction->text())){
            ui->tableWidget->setRowHidden(static_cast<int>(i), false);
        }else{
            ui->tableWidget->setRowHidden(static_cast<int>(i), true);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}


void widget_select_action_to_classify::on_pushButtonSelectAll_clicked()
{
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->setChecked(true);
    }
}


void widget_select_action_to_classify::on_pushButtonUnSelectUnselectAll_clicked()
{
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->setChecked(false);
    }
}

