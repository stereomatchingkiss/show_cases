#include "widget_roi_selection.hpp"

#include <ui/label_select_roi.hpp>

#include <QLabel>
#include <QVBoxLayout>

#include <QJsonObject>

namespace{

inline QString state_roi(){return "state_roi"; };

inline QString state_version(){ return "state_version"; }

}

widget_roi_selection::widget_roi_selection(QWidget *parent)
    : QWidget{parent},
    label_title_(new QLabel),
    layout_vbox_(new QVBoxLayout),
    roi_(new flt::ui::label_select_roi(tr("Select roi")))
{
    auto policy = roi_->sizePolicy();
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    roi_->setSizePolicy(policy);
    layout_vbox_->addWidget(label_title_);
    layout_vbox_->addWidget(roi_);

    QFont font;
    font.setBold(true);
    font.setPointSize(20);
    font.setKerning(true);
    font.setUnderline(true);
    label_title_->setAlignment(Qt::AlignCenter);
    label_title_->setText(tr("Select roi"));
    label_title_->setFont(font);

    setLayout(layout_vbox_);
}

void widget_roi_selection::display_frame(std::any img)
{
    roi_->display_frame(std::move(img));
}

QRectF widget_roi_selection::get_norm_rubber_band_rect() const
{
    return roi_->get_norm_rubber_band_rect();
}

QJsonObject widget_roi_selection::get_states() const
{
    QJsonObject obj;
    obj[state_roi()] = roi_->get_states();

    obj[state_version()] = "1.0";

    return obj;
}

void widget_roi_selection::set_states(const QJsonObject &val)
{
    if(val.contains(state_roi())){
        roi_->set_states(val[state_roi()].toObject());
    }
}
