#ifndef WIDGET_SOURCE_SELECTION_HPP
#define WIDGET_SOURCE_SELECTION_HPP

#include <QWidget>

namespace Ui {
class widget_source_selection;
}

namespace flt::mm{

enum class stream_source_type;
struct frame_capture_websocket_params;

}

struct config_source_selection;

class widget_source_selection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_source_selection(QWidget *parent = nullptr);
    ~widget_source_selection();

    config_source_selection get_config() const;
    flt::mm::stream_source_type get_source_type() const noexcept;
    QJsonObject get_states() const;

    flt::mm::frame_capture_websocket_params get_frame_capture_websocket_params() const;

    void set_states(QJsonObject const &val);

private:
    Ui::widget_source_selection *ui;
};

#endif // WIDGET_SOURCE_SELECTION_HPP
