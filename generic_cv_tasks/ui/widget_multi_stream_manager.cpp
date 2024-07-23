#include "widget_multi_stream_manager.hpp"
#include "ui_widget_multi_stream_manager.h"

#include <QGridLayout>

widget_multi_stream_manager::widget_multi_stream_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_multi_stream_manager),
    glayout_{new QGridLayout(this)}
{
    ui->setupUi(this);    
}

widget_multi_stream_manager::~widget_multi_stream_manager()
{
    delete ui;
}

void widget_multi_stream_manager::add_stream(QWidget *widget)
{
    if(glayout_->count() == 0){
        glayout_->addWidget(widget, 0, 0);
    }else if(glayout_->count() == 1){
        glayout_->addWidget(widget, 0, 1);
    }else if(glayout_->count() == 2){
        glayout_->addWidget(widget, 1, 0);
    }else if(glayout_->count() == 3){
        glayout_->addWidget(widget, 1, 1);
    }

    if(glayout_->count() <= 4){
        streams_.emplace_back(widget);
    }
}

void widget_multi_stream_manager::delete_stream()
{
    if(glayout_->count() > 0){
        auto *widget = streams_.back();
        streams_.pop_back();
        glayout_->removeWidget(widget);
        delete widget;
    }
}

int widget_multi_stream_manager::get_stream_count() const
{
    return glayout_->count();
}
