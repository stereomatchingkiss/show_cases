#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "widget_stacks_faces_search.hpp"

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

void widget_stacks_manager::init_stacked_widget()
{
    widget_stacks_faces_search_ = new widget_stacks_faces_search;
    ui->stackedWidget->addWidget(widget_stacks_faces_search_);
}
