#include "widget_multi_stream_manager.hpp"
#include "ui_widget_multi_stream_manager.h"

#include "../global/global_keywords.hpp"
#include "../global/global_object.hpp"

#include "dialog_alert_sender_settings.hpp"

#include "widget_stacks_manager.hpp"

#include <json/json_utils.hpp>

#include <QFileDialog>
#include <QGridLayout>

#include <QJsonArray>
#include <QJsonObject>

#include <format>

using namespace flt;
using namespace flt::mm;

widget_multi_stream_manager::widget_multi_stream_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_multi_stream_manager),
    glayout_{new QGridLayout(this)}
{
    ui->setupUi(this);

#ifndef WASM_BUILD
    load_settings_from_files(global_keywords().cam_config_path() + "/config.json");
#endif
}

widget_multi_stream_manager::~widget_multi_stream_manager()
{
#ifndef WASM_BUILD
    save_settings_to_file(global_keywords().cam_config_path() + "/config.json");
#endif

    for(auto w : streams_){
        qDebug()<<__func__<<": remove widget";
        glayout_->removeWidget(w);
        delete w;
    }

    delete ui;
}

void widget_multi_stream_manager::add_stream(QWidget *widget)
{
    qDebug()<<__func__<<": "<<streams_.size();
    if(streams_.size() < 4){
        qDebug()<<__func__<<": stream size < 4";
        streams_.emplace_back(widget);
        add_widget_to_grid_layout(glayout_->count(), widget);
    }else{
        qDebug()<<__func__<<": stream size > 4";

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

size_t widget_multi_stream_manager::get_max_page() const noexcept
{
    size_t max_page = streams_.size() / 4;
    if(max_page > 0 && (streams_.size() % 4 == 0)){
        --max_page;
    }

    return max_page;
}

size_t widget_multi_stream_manager::get_stream_count() const noexcept
{
    return streams_.size();
}

size_t widget_multi_stream_manager::get_stream_page() const noexcept
{
    return page_index_;
}

void widget_multi_stream_manager::load_settings()
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select settings"), "", tr("Settings (*.json)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        load_settings_from_files(fname);
    }
#else
    auto fcontent_ready = [this](QString const&, QByteArray const &fcontent) {
        load_settings_from_files(QJsonDocument::fromJson(fcontent).object());
    };
    QFileDialog::getOpenFileContent("Settings (*.json)",  fcontent_ready);
#endif
}

void widget_multi_stream_manager::save_settings()
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getSaveFileName(this, tr("Save at"));
        !fname.isEmpty())
    {
        if(fname.right(5) == ".json"){
            save_settings_to_file(fname);
        }else{
            save_settings_to_file(fname + ".json");
        }
    }
#else
    QFileDialog::saveFileContent(QJsonDocument(dump_settings()).toJson(), "config.json");
#endif
}

void widget_multi_stream_manager::next_page()
{
    qDebug()<<__func__;
    if(page_index_ < get_max_page()){
        remove_all_widgets();
        ++page_index_;
        for(size_t i = page_index_ * 4; i < (page_index_ * 4 + 4); ++i){
            if(i < streams_.size()){
                add_widget_to_grid_layout(i % 4, streams_[i]);
                streams_[i]->show();
            }
        }
    }
}

void widget_multi_stream_manager::prev_page()
{
    if(page_index_ > 0){
        remove_all_widgets();
        --page_index_;
        for(size_t i = page_index_ * 4; i < page_index_ * 4 + 4; ++i){
            if(i < streams_.size()){
                add_widget_to_grid_layout(i % 4, streams_[i]);
                streams_[i]->show();
            }
        }
    }
}

void widget_multi_stream_manager::add_widget_to_grid_layout(int count, QWidget *widget)
{
    if(count == 0){
        glayout_->addWidget(widget, 0, 0);
    }else if(count == 1){
        glayout_->addWidget(widget, 0, 1);
    }else if(count == 2){
        glayout_->addWidget(widget, 1, 0);
    }else if(count == 3){
        glayout_->addWidget(widget, 1, 1);
    }
}

void widget_multi_stream_manager::delete_streams()
{
    while(!streams_.empty()){
        delete_stream();
    }
}

void widget_multi_stream_manager::remove_all_widgets()
{
    for(auto w : streams_){
        glayout_->removeWidget(w);
        w->hide();
    }
}

void widget_multi_stream_manager::load_settings_from_files(const QString &fname)
{
    load_settings_from_files(flt::json::parse_file_to_jobj(fname));
}

void widget_multi_stream_manager::save_settings_to_file(const QString &save_at)
{
    flt::json::write_file_to_json(dump_settings(), save_at);
}

QJsonArray widget_multi_stream_manager::dump_stacks_states() const
{
    global_keywords const gk;
    QJsonArray arr;
    for(size_t i = 0; i != streams_.size(); ++i){
        QJsonObject cam_state_obj;
        cam_state_obj[gk.cam_name()] = std::format("cam{}", i).c_str();
        cam_state_obj[gk.state_stacks_manager()] = static_cast<widget_stacks_manager*>(streams_[i])->get_states();

        arr.append(cam_state_obj);
    }

    return arr;
}

QJsonObject widget_multi_stream_manager::dump_settings() const
{
    global_keywords const gk;
    QJsonObject obj_out;

    obj_out[gk.state_cam_states()] = dump_stacks_states();
    obj_out[gk.state_version()] = "2.0";
    obj_out[gk.state_widget_alert_settings()] = get_widget_alert_sender_settings().get_states();

    return obj_out;
}

void widget_multi_stream_manager::load_settings_from_files(const QJsonObject &jobj)
{
    delete_streams();

    global_keywords const gk;
    if(jobj.contains(gk.state_cam_states())){
        if(auto const arr = jobj[gk.state_cam_states()].toArray(); !arr.empty()){
            for(size_t i = 0; i != arr.size(); ++i){
                auto const &aobj = arr[i].toObject();
                if(aobj.contains(gk.state_stacks_manager())){
                    auto widget = new widget_stacks_manager;
                    widget->set_states(aobj[gk.state_stacks_manager()].toObject());
                    add_stream(widget);
                }
            }
        }
    }

    if(jobj.contains(gk.state_widget_alert_settings())){
        get_widget_alert_sender_settings().set_states(jobj[gk.state_widget_alert_settings()].toObject());
    }
}
