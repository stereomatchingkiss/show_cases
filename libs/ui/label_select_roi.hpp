#ifndef LABEL_SELECT_ROI_HPP
#define LABEL_SELECT_ROI_HPP

#include <any>

#include <QLabel>
#include <QPointF>
#include <QRectF>
#include <QString>

class QJsonObject;
class QRubberBand;

namespace flt::ui{

class label_select_roi : public QLabel
{
public:
    explicit label_select_roi(QString const &text,                              
                              QWidget *parent = nullptr);
    ~label_select_roi();

    void display_frame(std::any img);

    QRectF get_norm_rubber_band_rect() const;

    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private:
    void show_rubber_band();
    void show_rubber_band(QRectF const &norm_roi);

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    QPointF mouse_press_point_;
    QRectF norm_roi_;    
    QRubberBand *rband_;    
};

}

#endif // LABEL_SELECT_ROI_HPP
