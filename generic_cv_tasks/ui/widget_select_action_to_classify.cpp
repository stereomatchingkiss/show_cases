#include "widget_select_action_to_classify.hpp"
#include "ui_widget_select_action_to_classify.h"

#include "../algo/action_classify/kinetic_400_labels.hpp"

#include <ui/utils/qtable_item_utils.hpp>

#include <QTimer>

using namespace flt::ui;

widget_select_action_to_classify::widget_select_action_to_classify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_select_action_to_classify),
    timer_{new QTimer}
{
    ui->setupUi(this);

    connect(timer_, &QTimer::timeout, this, &widget_select_action_to_classify::search_similar_keywords);

    timer_->setInterval(500);
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

void widget_select_action_to_classify::on_lineEditSearchAction_textChanged(const QString &)
{
    timer_->start();
}

void widget_select_action_to_classify::search_similar_keywords()
{
    auto const &labels = kinetic_400_labels::get_labels();
    for(size_t i = 0; i != labels.size(); ++i){
        if(labels[i].contains(ui->lineEditSearchAction->text())){
            ui->tableWidget->setRowHidden(i, false);
        }else{
            ui->tableWidget->setRowHidden(i, true);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}

