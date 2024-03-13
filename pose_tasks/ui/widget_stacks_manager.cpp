#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "widget_tasks_selection.hpp"

widget_stacks_manager::widget_stacks_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_manager)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_manager::~widget_stacks_manager()
{
    delete ui;
}

void widget_stacks_manager::back_to_task_selection()
{
    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
}

void widget_stacks_manager::init_stacked_widget()
{
    widget_tasks_selection_ = new widget_tasks_selection;

    ui->stackedWidget->addWidget(widget_tasks_selection_);

    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);

    connect(widget_tasks_selection_, &widget_tasks_selection::task_selected, this, &widget_stacks_manager::switch_task);    
}

void widget_stacks_manager::switch_task(int index)
{

}
