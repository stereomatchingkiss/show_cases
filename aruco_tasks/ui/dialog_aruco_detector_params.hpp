#ifndef DIALOG_ARUCO_DETECTOR_PARAMS_HPP
#define DIALOG_ARUCO_DETECTOR_PARAMS_HPP

#include <QDialog>

namespace Ui {
class dialog_aruco_detector_params;
}

struct config_aruco_detect_worker;

class dialog_aruco_detector_params : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_aruco_detector_params(QWidget *parent = nullptr);
    ~dialog_aruco_detector_params();

    config_aruco_detect_worker get_config() const;

private slots:
    void on_pushButtonContourFilteringDefault_clicked();

    void on_pushButtonThresholdDefault_clicked();

    void on_pushButtonBitsExtractionDefault_clicked();

    void on_pushButtonMarkerIdentificationDefault_clicked();

private:
    void adaptive_value_change(int);

    void generate_aruco_option(int bit_size);

    void set_default_value_bits_extraction();
    void set_default_value_contour_filtering();
    void set_default_value_marker_identification();
    void set_default_value_threshold();

    void init_aruco_combo_box();
    void init_bits_extraction();
    void init_contour_filtering();
    void init_marker_identification();
    void init_threshold();

    void marker_perimeter_rate_changed(double);

    Ui::dialog_aruco_detector_params *ui;
};

#endif // DIALOG_ARUCO_DETECTOR_PARAMS_HPP
