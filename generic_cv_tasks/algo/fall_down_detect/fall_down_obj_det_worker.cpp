#include "fall_down_obj_det_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_fall_down_obj_det_worker.hpp"
#include "../../config/config_generic_obj_detector.hpp"

#include "../../global/global_keywords.hpp"

#include "../generic_obj_detector.hpp"
#include "../generic_worker_results.hpp"

#include "../convert_input_to_image.hpp"

#include "fall_down_det_alert_save.hpp"

#include <cv_algo/converter/box_type_converter.hpp>

#include <cv_algo/obj_detect/obj_det_utils.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet.hpp>

#include <cv_algo/tracker/track_results.hpp>
#include <cv_algo/tracker/byte_track/BYTETracker.hpp>

#include <cv_algo/utils/draw_rect.hpp>

#include <utils/image_utils.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <SimpleMail>

#include <QDebug>
#include <QRectF>

#include <format>
#include <mutex>

using namespace flt;
using namespace flt::cvt;
using namespace flt::cvt::det;
using namespace flt::cvt::tracker;

using namespace SimpleMail;

struct fall_down_obj_det_worker::impl
{
    impl(config_fall_down_obj_det_worker const &config) :
        alert_save_(global_keywords().coco_names()),
        config_{config},
        names_{global_keywords().coco_names()},
        tracker_{30,
                 30,
                 config_.config_object_detect_model_select_.confidence_,
                 std::min(config_.config_object_detect_model_select_.confidence_ + 0.05f, 0.8f),
                 0.8f}
    {
        config_generic_obj_detector gconfig;
        gconfig.config_object_detect_model_select_ = config_.config_object_detect_model_select_;
        gconfig.config_select_object_to_detect_.selected_object_.insert("person");
        obj_det_ = std::make_unique<generic_obj_detector>(std::move(gconfig));

        alert_save_.change_alert_sender_config(config_.config_alert_sender_);
    }

    void change_alert_sender_config(config_alert_sender const &val)
    {
        qDebug()<<__func__;
        config_.config_alert_sender_ = val;
        alert_save_.change_alert_sender_config(val);
    }

    auto create_email_alert() const
    {
        /*auto html = std::make_shared<MimeHtml>();

        html->setHtml(QLatin1String("<h1> Fall down alert </h1>"
                                    "<img src='cid:image1' />"));
        auto image1 =
            std::make_shared<MimeInlineFile>(std::make_shared<QFile>(saved_im_path_));
        image1->setContentId(QByteArrayLiteral("image1"));
        image1->setContentType(QByteArrayLiteral("image/jpeg"));

        std::vector<std::shared_ptr<SimpleMail::MimePart>> parts;
        parts.emplace_back(std::move(html));
        parts.emplace_back(std::move(image1));//*/

        auto html = std::make_shared<MimeHtml>();

        html->setHtml(QLatin1String("<h1> Fall down alert </h1>"
                                    "<img src='cid:image1' />"));

        // Create a MimeInlineFile object for each image
        auto image1 =
            std::make_shared<MimeInlineFile>(std::make_shared<QFile>(saved_im_path_));

        // An unique content id must be setted
        image1->setContentId(QByteArrayLiteral("image1"));
        image1->setContentType(QByteArrayLiteral("image/jpeg"));

        std::vector<std::shared_ptr<SimpleMail::MimePart>> parts;
        parts.emplace_back(std::move(html));
        parts.emplace_back(std::move(image1));

        return parts;
    }

    bool save_alert_info(QImage const &img)
    {
        bool can_send_alert = false;
        if(can_send_alert_ && !fall_down_counter_.empty()){
            using wt = fall_down_warning_type;
            auto const cdt = QDateTime::currentDateTime();
            for(auto it = std::begin(fall_down_counter_); it != std::end(fall_down_counter_); ++it){
                if(it->second.can_issued_){
                    auto const &config_alert = config_.config_fall_down_obj_det_alert_;
                    if(config_alert.warning_type_ == wt::issue_a_warning &&
                        it->second.warning_issued_ == false){
                        can_send_alert = true;
                    }else if(config_alert.warning_type_ == wt::issue_warning_periodic){
                        if(!it->second.current_dt_.isValid() ||
                            it->second.current_dt_.secsTo(cdt) > config_alert.warning_periodic_){
                            can_send_alert = true;
                            it->second.current_dt_ = cdt;
                        }
                    }

                    it->second.warning_issued_ = true;
                }
            }
        }

        if(can_send_alert){
            saved_im_path_ = alert_save_.save_to_json(img);
        }else{
            saved_im_path_.clear();
        }

        return can_send_alert;
    }

    bool update_fall_down_counter(int id)
    {
        active_id_.insert(id);
        if(auto it = fall_down_counter_.find(id); it != std::end(fall_down_counter_)){
            ++it->second.continuous_active_;
            it->second.continuous_non_active_ = 0;

            if(it->second.continuous_active_ >= config_.config_fall_down_condition_.number_of_consecutive_falls_){
                it->second.can_issued_ = true;
                it->second.continuous_active_ = 0;
                return true;
            }
        }else{
            fall_down_counter_.insert({id, {}});

            if(1 >= config_.config_fall_down_condition_.number_of_consecutive_falls_){
                it->second.can_issued_ = true;
                return true;
            }
        }

        return false;
    }

    void remove_id_lost_track()
    {
        for(auto it = std::begin(fall_down_counter_); it != std::end(fall_down_counter_);){
            if(!active_id_.contains(it->first)){
                ++it->second.continuous_non_active_;
                if(it->second.continuous_non_active_ > lost_track_threshold_){
                    fall_down_counter_.erase(it++);
                }else{
                    ++it;
                }
            }else{
                ++it;
            }
        }
    }

    auto track_obj(cv::Mat &mat)
    {        
        auto det_results = obj_det_->predict(mat);
        auto track_obj = box_info_to_byte_track_obj(det_results);
        auto const track_ptr_vec = tracker_.update(track_obj);

        det_results = byte_track_obj_to_box_info(track_ptr_vec);
        active_id_.clear();
        can_send_alert_ = false;
        for(auto const &val : det_results){
            det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
            auto const wh_ratio = width_height_ratio(val);
            if(wh_ratio >= config_.config_fall_down_condition_.width_height_ratio_){
                if(update_fall_down_counter(val.track_id_)){
                    can_send_alert_ = true;
                }
                det::draw_bboxes_custom(mat, val, std::format("{}:{}:{:.3f}=fall", names_[val.label_], val.track_id_, wh_ratio));
            }else{
                det::draw_bboxes_custom(mat, val, std::format("{}:{}:{:.3f}", names_[val.label_], val.track_id_, wh_ratio));
            }
        }

        if(!config_.config_fall_down_obj_det_alert_.warning_on_){
            can_send_alert_ = false;
        }

        remove_id_lost_track();

        return det_results;
    }

    float width_height_ratio(box_info const &val) const noexcept
    {
        auto const tl = val.tl();
        auto const br = val.br();
        auto const width = br.x - tl.x;
        auto const height = br.y - tl.y;

        return std::abs(width/(height + 0.001f));
    }

    struct fall_down_count
    {
        bool can_issued_ = false;
        int continuous_active_ = 1;
        int continuous_non_active_ = 0;
        QDateTime current_dt_;
        bool warning_issued_ = false;
    };    

    std::set<int> active_id_;
    fall_down_det_alert_save alert_save_;
    bool can_send_alert_ = false;
    config_fall_down_obj_det_worker config_;
    std::map<int, fall_down_count> fall_down_counter_;
    static int constexpr lost_track_threshold_ = 300;
    std::vector<std::string> names_;
    std::unique_ptr<generic_obj_detector> obj_det_;
    QString saved_im_path_;
    cv::Rect scaled_roi_;
    BYTETracker tracker_;
};

fall_down_obj_det_worker::fall_down_obj_det_worker(const config_fall_down_obj_det_worker &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(std::move(config))}
{

}

fall_down_obj_det_worker::~fall_down_obj_det_worker()
{

}

void fall_down_obj_det_worker::change_alert_sender_config(const config_alert_sender &val)
{
    impl_->change_alert_sender_config(val);
}

void fall_down_obj_det_worker::process_results(std::any frame)
{
    auto [mat, qimg] = convert_std_any_to_image(frame, impl_->config_.source_type_, impl_->config_.roi_);

    auto const det_results = impl_->track_obj(mat);
    generic_worker_results results;
    results.alarm_on_ = impl_->can_send_alert_;

    if(results.alarm_on_ && impl_->save_alert_info(qimg)){
        if(impl_->config_.config_alert_sender_.send_alert_by_websocket_){
            emit send_alert_by_text(impl_->alert_save_.get_alert_info());
        }

        if(impl_->config_.config_alert_sender_.email_alert_on_ && !impl_->saved_im_path_.isEmpty()){
            qDebug()<<__func__<<": send image by email = "<<impl_->saved_im_path_;
            if(QFile::exists(impl_->saved_im_path_)){
                emit send_alert_by_email(impl_->create_email_alert());
            }
        }
    }

    if(impl_->config_.source_type_ != flt::mm::stream_source_type::rtsp){
        results.mat_ = std::move(qimg);
    }else{
        results.mat_ = qimg.copy();
    }

    emit send_process_results(std::move(results));

}
