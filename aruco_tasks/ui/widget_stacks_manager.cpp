#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "widget_stacks_aruco_creator.hpp"
#include "widget_stacks_aruco_detector.hpp"
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
    widget_stacks_aruco_creator_ = new widget_stacks_aruco_creator;
    widget_stacks_aruco_detector_ = new widget_stacks_aruco_detector;
    widget_tasks_selection_ = new widget_tasks_selection;

    ui->stackedWidget->addWidget(widget_tasks_selection_);
    ui->stackedWidget->addWidget(widget_stacks_aruco_creator_);
    ui->stackedWidget->addWidget(widget_stacks_aruco_detector_);

    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);

    connect(widget_tasks_selection_, &widget_tasks_selection::task_selected, this, &widget_stacks_manager::switch_task);
    connect(widget_stacks_aruco_creator_, &widget_stacks_aruco_creator::prev_button_clicked,
            this, &widget_stacks_manager::back_to_task_selection);
    connect(widget_stacks_aruco_detector_, &widget_stacks_aruco_detector::prev_button_clicked,
            this, &widget_stacks_manager::back_to_task_selection);
}

void widget_stacks_manager::switch_task(int index)
{
    switch(index){
    case 0:{
        ui->stackedWidget->setCurrentWidget(widget_stacks_aruco_creator_);
        break;
    }
    case 1:{
        ui->stackedWidget->setCurrentWidget(widget_stacks_aruco_detector_);
        break;
    }
    default:{
        ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
        break;
    }
    }
}
