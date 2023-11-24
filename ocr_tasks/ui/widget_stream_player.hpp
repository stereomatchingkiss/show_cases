#ifndef WIDGET_STREAM_PLAYER_HPP
#define WIDGET_STREAM_PLAYER_HPP

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#ifdef WASM_BUILD
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_rec_onnx.hpp>
#endif

#include <QFont>
#include <QImage>
#include <QPen>
#include <QTimer>
#include <QWidget>

#include <any>

namespace Ui {
class widget_stream_player;
}

class QTimer;

class dialog_display_details;

class widget_stream_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stream_player(QWidget *parent = nullptr);
    ~widget_stream_player();

    void display_frame(std::any results);

    QJsonObject get_states() const;
#ifdef WASM_BUILD
    void init_rec_model();
#endif

    void set_can_save_on_local(bool val);
    void set_states(QJsonObject const &val);

signals:
    void image_selected(std::any img);
    void process_done();
    void resize_window();
    void send_ocr_results(QString msg);

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonSelectImage_clicked();

    void on_pushButtonDisplayDetails_clicked();

    void on_checkBoxHideImage_stateChanged(int arg1);

    void on_checkBoxHideTable_stateChanged(int arg1);

    void on_checkBoxDrawAll_clicked();

private:
    void cell_cliked(int row, int col);
    void draw_all();
    QPolygon text_box_to_qpolygon(int row) const;
    QJsonObject text_boxes_to_json() const;    
    void update_clicked_contents(int row);
    void update_image_label(QImage const &img);
    void update_table_headers();
    void update_table_contents();

    QJsonObject table_contents_to_json() const;
    QString table_contents_to_string() const;
    void updated_text_rec_results();

    void resizeEvent(QResizeEvent *e) override;

    Ui::widget_stream_player *ui;

    bool can_save_on_local_ = true;
    dialog_display_details *dialog_display_details_;
    int last_clicked_row_ = -1;
    QFont font_;
    QPen pen_;
    QPen pen_all_;
    QImage qimg_;

    std::vector<flt::cvt::ocr::TextBox> text_boxes_;

#ifdef WASM_BUILD
    void start_text_rec_process();
    void update_rec_result();

    std::unique_ptr<flt::cvt::ocr::paddle_ocr_rec_onnx> text_rec_;

    cv::Mat cv_mat_;
    size_t process_rec_index_ = 0;
    QTimer *timer_;
    QTimer *timer_model_ready_;
#endif    
};

#endif // WIDGET_STREAM_PLAYER_HPP
