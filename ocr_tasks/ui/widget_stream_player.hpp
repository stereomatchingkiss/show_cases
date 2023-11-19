#ifndef WIDGET_STREAM_PLAYER_HPP
#define WIDGET_STREAM_PLAYER_HPP

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <QFont>
#include <QImage>
#include <QPen>
#include <QWidget>

#include <any>

namespace Ui {
class widget_stream_player;
}

class dialog_display_details;

class widget_stream_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stream_player(QWidget *parent = nullptr);
    ~widget_stream_player();

    void display_frame(std::any results);

    void set_can_save_on_local(bool val);

signals:
    void image_selected(std::any img);    

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
    void update_clicked_contents(int row);
    void update_table_headers();
    void update_table_contents();

    QJsonObject table_contents_to_json() const;
    QString table_contents_to_string() const;

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
};

#endif // WIDGET_STREAM_PLAYER_HPP
