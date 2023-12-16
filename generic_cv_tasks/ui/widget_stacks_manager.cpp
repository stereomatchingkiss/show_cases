#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "widget_stacks_object_tracking.hpp"
#include "widget_tasks_selection.hpp"

#include <QDebug>

#include <QJsonObject>

using namespace flt;
using namespace flt::mm;

namespace{

QString const state_stacks_object_tracking("state_stacks_object_tracking");
QString const state_tasks_selection("state_tasks_selection");

}

widget_stacks_manager::widget_stacks_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_manager),
    widget_stacks_object_tracking_{new widget_stacks_object_tracking},
    widget_tasks_selection_{new widget_tasks_selection}
{
    ui->setupUi(this);

    init_stacked_widget();

    connect(widget_stacks_object_tracking_, &widget_stacks_object_tracking::enable_next_button,
            this, &widget_stacks_manager::enable_next_button);

    setMinimumSize(QSize(600, 400));
}

widget_stacks_manager::~widget_stacks_manager()
{
    delete ui;
}

QJsonObject widget_stacks_manager::get_states() const
{
    QJsonObject obj;
    obj[state_stacks_object_tracking] = widget_stacks_object_tracking_->get_states();
    obj[state_tasks_selection] = widget_tasks_selection_->get_states();

    return obj;
}

void widget_stacks_manager::set_states(const QJsonObject &val)
{    
    if(val.contains(state_stacks_object_tracking)){
        widget_stacks_object_tracking_->set_states(val[state_stacks_object_tracking].toObject());
    }
    if(val.contains(state_tasks_selection)){
        widget_tasks_selection_->set_states(val[state_tasks_selection].toObject());
    }    
}

void widget_stacks_manager::on_pushButtonNext_clicked()
{    
    if(ui->stackedWidget->currentWidget() == widget_tasks_selection_){
        ui->pushButtonNext->setVisible(false);
        ui->stackedWidget->setCurrentWidget(widget_stacks_object_tracking_);
    }
}

void widget_stacks_manager::enable_next_button()
{
    ui->pushButtonNext->setVisible(true);
    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
}

void widget_stacks_manager::init_stacked_widget()
{    
    ui->stackedWidget->addWidget(widget_tasks_selection_);
    ui->stackedWidget->addWidget(widget_stacks_object_tracking_);

    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
}
