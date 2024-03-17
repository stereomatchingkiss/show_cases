#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "widget_stacks_estimate_pose_similarity.hpp"
#include "widget_stacks_pose_estimation.hpp"
#include "widget_tasks_selection.hpp"

#include "../config/config_tasks_selection.hpp"

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
    widget_stacks_estimate_pose_similarity_ = new widget_stacks_estimate_pose_similarity;
    widget_stacks_pose_estimation_ = new widget_stacks_pose_estimation;
    widget_tasks_selection_ = new widget_tasks_selection;

    ui->stackedWidget->addWidget(widget_stacks_estimate_pose_similarity_);
    ui->stackedWidget->addWidget(widget_stacks_pose_estimation_);
    ui->stackedWidget->addWidget(widget_tasks_selection_);

    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);

    connect(widget_tasks_selection_, &widget_tasks_selection::task_selected, this, &widget_stacks_manager::switch_task);
    connect(widget_stacks_estimate_pose_similarity_, &widget_stacks_estimate_pose_similarity::prev_button_clicked,
            this, &widget_stacks_manager::back_to_task_selection);
    connect(widget_stacks_pose_estimation_, &widget_stacks_pose_estimation::prev_button_clicked,
            this, &widget_stacks_manager::back_to_task_selection);
}

void widget_stacks_manager::switch_task(int index)
{
    switch(static_cast<enum_config_tasks>(index)){
    case enum_config_tasks::pose_estimation:{
        ui->stackedWidget->setCurrentWidget(widget_stacks_pose_estimation_);
        break;
    }
    case enum_config_tasks::estimate_pose_similarity:{
        ui->stackedWidget->setCurrentWidget(widget_stacks_estimate_pose_similarity_);
        break;
    }
    default:{
        ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
        break;
    }
    }
}
