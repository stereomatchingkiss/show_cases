#include "widget_stacks_estimate_pose_similarity.hpp"
#include "ui_widget_stacks_estimate_pose_similarity.h"

#include "widget_estimate_many_pose_similarity_params.hpp"
#include "widget_estimate_similar_poses_player.hpp"
#include "widget_image_pair_player.hpp"
#include "widget_pose_estimation_params.hpp"
#include "widget_source_selection.hpp"

#include "../algo/estimate_many_pose_similarity_worker.hpp"
#include "../algo/estimate_many_pose_similarity_worker_results.hpp"
#include "../algo/estimate_pose_similarity_worker.hpp"
#include "../algo/estimate_pose_similarity_worker_input.hpp"
#include "../algo/pose_estimation_common_func.hpp"

#include "../config/config_pose_estimation_worker.hpp"
#include "../config/config_estimate_many_pose_similarity_worker.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

#include <QJsonDocument>
#include <QJsonObject>

using namespace flt::mm;

using stype = stream_source_type;

namespace{

QString create_mode(QString const &mode)
{
    QJsonObject obj;
    obj["mode"] = mode;

    return QJsonDocument(obj).toJson();
}

}

widget_stacks_estimate_pose_similarity::widget_stacks_estimate_pose_similarity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_estimate_pose_similarity)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_estimate_pose_similarity::~widget_stacks_estimate_pose_similarity()
{
    delete ui;
}

void widget_stacks_estimate_pose_similarity::init_stacked_widget()
{
    widget_estimate_many_pose_similarity_params_ = new widget_estimate_many_pose_similarity_params;
    widget_estimate_similar_poses_player_ = new widget_estimate_similar_poses_player;
    widget_image_pair_player_ = new widget_image_pair_player;
    widget_pose_estimation_params_ = new widget_pose_estimation_params;
    widget_source_selection_ = new widget_source_selection;

    ui->stackedWidget->addWidget(widget_estimate_many_pose_similarity_params_);
    ui->stackedWidget->addWidget(widget_estimate_similar_poses_player_);
    ui->stackedWidget->addWidget(widget_image_pair_player_);
    ui->stackedWidget->addWidget(widget_pose_estimation_params_);
    ui->stackedWidget->addWidget(widget_source_selection_);

    ui->stackedWidget->setCurrentWidget(widget_source_selection_);

    connect(widget_estimate_similar_poses_player_, &widget_estimate_similar_poses_player::image_selected,
            this, &widget_stacks_estimate_pose_similarity::image_selected);
    connect(widget_estimate_similar_poses_player_, &widget_estimate_similar_poses_player::similar_img_clicked,
            this, &widget_stacks_estimate_pose_similarity::similar_img_clicked);
}

void widget_stacks_estimate_pose_similarity::next_page_is_estimate_pose_similarity_display()
{    
    ui->pushButtonNext->setVisible(false);

    if(widget_source_selection_->get_source_type() == stype::websocket){
        widget_estimate_similar_poses_player_->set_similar_pose_visible(false);
        ui->stackedWidget->setCurrentWidget(widget_estimate_similar_poses_player_);        

        auto const config = widget_estimate_many_pose_similarity_params_->get_config();
        auto worker = new estimate_many_pose_similarity_worker(config);
        connect(worker, &estimate_many_pose_similarity_worker::send_msg,
                this, &widget_stacks_estimate_pose_similarity::received_process_msg);
        connect(worker, &estimate_many_pose_similarity_worker::send_request_image,
                widget_estimate_similar_poses_player_, &widget_estimate_similar_poses_player::set_request_image);
        connect(worker, &estimate_many_pose_similarity_worker::send_similar_pose,
                widget_estimate_similar_poses_player_, &widget_estimate_similar_poses_player::set_similar_pose);

        process_controller_ = std::make_shared<frame_process_controller>(worker);

        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                this, &widget_stacks_estimate_pose_similarity::send_process_results);

        emit process_controller_->start();

        auto wsocket_config = widget_source_selection_->get_frame_capture_websocket_params();
        wsocket_config.not_encoded_image_ = true;
        sfwmw_ = std::make_unique<frame_capture_websocket>(wsocket_config);
        sfwmw_->add_listener(process_controller_, this);
        sfwmw_->start();

        setEnabled(false);
    }else{
        auto config = widget_pose_estimation_params_->get_config();
        config.source_type_ = widget_source_selection_->get_source_type();
        auto worker = new estimate_pose_similarity_worker(config);
        process_controller_ = std::make_shared<frame_process_controller>(worker);
        sfwmw_ = nullptr;

        ui->stackedWidget->setCurrentWidget(widget_image_pair_player_);        
        connect(widget_image_pair_player_, &widget_image_pair_player::source_image_selected,
                this, &widget_stacks_estimate_pose_similarity::process_source_image);
        connect(widget_image_pair_player_, &widget_image_pair_player::target_image_selected,
                this, &widget_stacks_estimate_pose_similarity::process_target_image);
        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                widget_image_pair_player_, &widget_image_pair_player::display_frame);

        emit process_controller_->start();
    }
}

void widget_stacks_estimate_pose_similarity::process_source_image(QImage img)
{
    estimate_pose_similarity_worker_input input;
    input.qimg_ = std::move(img);
    input.is_target_ = false;

    process_controller_->predict(std::move(input));
}

void widget_stacks_estimate_pose_similarity::process_target_image(QImage img)
{
    estimate_pose_similarity_worker_input input;
    input.qimg_ = std::move(img);
    input.is_target_ = true;

    process_controller_->predict(std::move(input));
}

void widget_stacks_estimate_pose_similarity::received_process_msg(QString msg)
{
    if(msg == "start"){
        auto const config = widget_estimate_many_pose_similarity_params_->get_config();
        static_cast<frame_capture_websocket*>(sfwmw_.get())->send_text_message(config.generate_json_info());
    }else if(msg == "next"){
        static_cast<frame_capture_websocket*>(sfwmw_.get())->send_text_message(create_mode("next"));
    }else if(msg == "add_done"){
        setEnabled(true);
        widget_estimate_similar_poses_player_->set_label_text(tr("Select the image you want to compare similarity"));
    }
}

void widget_stacks_estimate_pose_similarity::similar_img_clicked(QString const &path)
{
    qDebug()<<"similar_img_clicked = "<<path;
    QJsonObject obj;
    obj["mode"] = "get_image";
    obj["im_path"] = path;
    static_cast<frame_capture_websocket*>(sfwmw_.get())->send_text_message(QJsonDocument(obj).toJson());
}

void widget_stacks_estimate_pose_similarity::send_process_results(std::any results)
{
    auto const val = std::any_cast<estimate_many_pose_similarity_worker_results>(results);
    auto jobj = convert_to_json(val.points_);
    jobj["mode"] = "store_json";
    jobj["im_path"] = val.im_path_;
    static_cast<frame_capture_websocket*>(sfwmw_.get())->send_text_message(QJsonDocument(jobj).toJson());
    widget_estimate_similar_poses_player_->display_frame(val);
}

void widget_stacks_estimate_pose_similarity::on_pushButtonPrev_clicked()
{
    ui->pushButtonNext->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_pose_estimation_params_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }else if(ui->stackedWidget->currentWidget() == widget_image_pair_player_){
        ui->stackedWidget->setCurrentWidget(widget_pose_estimation_params_);
    }else if(ui->stackedWidget->currentWidget() == widget_estimate_similar_poses_player_){
        ui->stackedWidget->setCurrentWidget(widget_estimate_many_pose_similarity_params_);
    }else{
        emit prev_button_clicked();
    }
}

void widget_stacks_estimate_pose_similarity::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        if(widget_source_selection_->get_source_type() == stype::image){
            ui->stackedWidget->setCurrentWidget(widget_pose_estimation_params_);
        }else{
            ui->stackedWidget->setCurrentWidget(widget_estimate_many_pose_similarity_params_);
        }
    }else if(ui->stackedWidget->currentWidget() == widget_pose_estimation_params_ ||
               ui->stackedWidget->currentWidget() == widget_estimate_many_pose_similarity_params_){
        next_page_is_estimate_pose_similarity_display();
    }
}

void widget_stacks_estimate_pose_similarity::image_selected(QString info)
{
    process_controller_->predict(info);
}

