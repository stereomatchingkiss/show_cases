#ifndef WIDGET_ROI_SELECTION_HPP
#define WIDGET_ROI_SELECTION_HPP

#include <QWidget>

#include <any>

namespace flt::ui{

class label_select_roi;

}

class QLabel;
class QVBoxLayout;

class widget_roi_selection : public QWidget
{
    Q_OBJECT
public:
    explicit widget_roi_selection(QWidget *parent = nullptr);

    void display_frame(std::any img);

    QRectF get_norm_rubber_band_rect() const;

    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private:
    QLabel *label_title_;
    QVBoxLayout *layout_vbox_;

    flt::ui::label_select_roi *roi_;
};

#endif // WIDGET_ROI_SELECTION_HPP
