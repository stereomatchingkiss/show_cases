#include "dialog_aruco_detector_params.hpp"
#include "ui_dialog_aruco_detector_params.h"

#include "../config/config_aruco_detect_worker.hpp"

#include <opencv2/aruco.hpp>

dialog_aruco_detector_params::dialog_aruco_detector_params(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_aruco_detector_params)
{
    ui->setupUi(this);

    init_aruco_combo_box();

    init_bits_extraction();
    init_contour_filtering();
    init_marker_identification();
    init_threshold();
}

dialog_aruco_detector_params::~dialog_aruco_detector_params()
{
    delete ui;
}

config_aruco_detect_worker dialog_aruco_detector_params::get_config() const
{
    config_aruco_detect_worker config;
    config.dictionary_ = cv::aruco::getPredefinedDictionary(ui->comboBoxDictionary->currentIndex());

    auto &det_param = config.detector_params_;
    det_param.minMarkerPerimeterRate = ui->doubleSpinBoxMinMarkerPerimeterRate->value();
    det_param.maxMarkerPerimeterRate = ui->doubleSpinBoxMaxMarkerPerimeterRate->value();
    det_param.polygonalApproxAccuracyRate = ui->doubleSpinBoxPolygonalApproxAccuracyRate->value();
    det_param.minCornerDistanceRate = ui->doubleSpinBoxMinCornerDistanceRate->value();
    det_param.minMarkerDistanceRate = ui->doubleSpinBoxMinMarkerDistanceRate->value();
    det_param.minDistanceToBorder = ui->spinBoxMinDistanceToBorder->value();

    det_param.adaptiveThreshConstant = ui->spinBoxAdaptiveThreshConstant->value();
    det_param.adaptiveThreshWinSizeMin = ui->spinBoxAdaptiveThreshWinSizeMin->value();
    det_param.adaptiveThreshWinSizeMax = ui->spinBoxAdaptiveThreshWinSizeMax->value();
    det_param.adaptiveThreshWinSizeStep = ui->spinBoxAdaptiveThreshWinSizeStep->value();

    det_param.markerBorderBits = ui->spinBoxMarkerBorderBits->value();
    det_param.minOtsuStdDev = ui->doubleSpinBoxMinOtsuStdDev->value();
    det_param.perspectiveRemoveIgnoredMarginPerCell = ui->spinBoxPerspectiveRemovePixelPerCell->value();
    det_param.perspectiveRemoveIgnoredMarginPerCell = ui->doubleSpinBoxPerspectiveRemoveIgnoredMarginPerCell->value();

    det_param.maxErroneousBitsInBorderRate = ui->doubleSpinBoxMaxErroneousBitsInBorderRate->value();
    det_param.errorCorrectionRate = ui->doubleSpinBoxErrorCorrectionRate->value();

    return config;
}

void dialog_aruco_detector_params::generate_aruco_option(int bit_size)
{
    auto const num = QString::number(bit_size);
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_50").arg(num));
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_100").arg(num));
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_250").arg(num));
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_1000").arg(num));
}

void dialog_aruco_detector_params::set_default_value_bits_extraction()
{
    cv::aruco::DetectorParameters const params;

    ui->spinBoxMarkerBorderBits->setValue(params.markerBorderBits);
    ui->doubleSpinBoxMinOtsuStdDev->setValue(params.minOtsuStdDev);
    ui->spinBoxPerspectiveRemovePixelPerCell->setValue(params.perspectiveRemovePixelPerCell);
    ui->doubleSpinBoxPerspectiveRemoveIgnoredMarginPerCell->setValue(params.perspectiveRemoveIgnoredMarginPerCell);
}

void dialog_aruco_detector_params::set_default_value_contour_filtering()
{
    cv::aruco::DetectorParameters const params;

    ui->doubleSpinBoxMaxMarkerPerimeterRate->setValue(params.maxMarkerPerimeterRate);
    ui->doubleSpinBoxMinMarkerPerimeterRate->setValue(params.minMarkerPerimeterRate);
    ui->doubleSpinBoxPolygonalApproxAccuracyRate->setValue(params.polygonalApproxAccuracyRate);
    ui->doubleSpinBoxMinCornerDistanceRate->setValue(params.minCornerDistanceRate);
    ui->doubleSpinBoxMinMarkerDistanceRate->setValue(params.minMarkerDistanceRate);
    ui->spinBoxMinDistanceToBorder->setValue(params.minDistanceToBorder);
}

void dialog_aruco_detector_params::set_default_value_marker_identification()
{
    cv::aruco::DetectorParameters const params;

    ui->doubleSpinBoxMaxErroneousBitsInBorderRate->setValue(params.maxErroneousBitsInBorderRate);
    ui->doubleSpinBoxErrorCorrectionRate->setValue(params.errorCorrectionRate);
}

void dialog_aruco_detector_params::set_default_value_threshold()
{
    cv::aruco::DetectorParameters const params;

    ui->spinBoxAdaptiveThreshWinSizeMin->setValue(params.adaptiveThreshWinSizeMin);
    ui->spinBoxAdaptiveThreshWinSizeMax->setValue(params.adaptiveThreshWinSizeMax);
    ui->spinBoxAdaptiveThreshWinSizeStep->setValue(params.adaptiveThreshWinSizeStep);
    ui->spinBoxAdaptiveThreshConstant->setValue(params.adaptiveThreshConstant);
}

void dialog_aruco_detector_params::init_aruco_combo_box()
{
    generate_aruco_option(4);
    generate_aruco_option(5);
    generate_aruco_option(6);
    generate_aruco_option(7);

    ui->comboBoxDictionary->addItem("DICT_ARUCO_ORIGINAL");
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_16h5");
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_25h9");
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_36h10");
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_36h11");
    ui->comboBoxDictionary->addItem("DICT_ARUCO_MIP_36h12");
}

void dialog_aruco_detector_params::init_bits_extraction()
{           
    ui->spinBoxMarkerBorderBits->setRange(0, 5);    

    ui->doubleSpinBoxMinOtsuStdDev->setRange(0, 50);
    ui->doubleSpinBoxMinOtsuStdDev->setSingleStep(0.01);    

    ui->spinBoxPerspectiveRemovePixelPerCell->setRange(1, 20);    

    ui->doubleSpinBoxPerspectiveRemoveIgnoredMarginPerCell->setRange(0, 1.0);
    ui->doubleSpinBoxPerspectiveRemoveIgnoredMarginPerCell->setSingleStep(0.01);

    set_default_value_bits_extraction();
}

void dialog_aruco_detector_params::init_contour_filtering()
{    
    ui->doubleSpinBoxMinMarkerPerimeterRate->setRange(0.01, 4);
    ui->doubleSpinBoxMinMarkerPerimeterRate->setSingleStep(0.01);   

    ui->doubleSpinBoxMaxMarkerPerimeterRate->setRange(0.01, 4);
    ui->doubleSpinBoxMaxMarkerPerimeterRate->setSingleStep(0.01);    

    ui->doubleSpinBoxPolygonalApproxAccuracyRate->setRange(0, 1);
    ui->doubleSpinBoxPolygonalApproxAccuracyRate->setSingleStep(0.01);    

    ui->doubleSpinBoxMinCornerDistanceRate->setRange(0.01, 1.0);
    ui->doubleSpinBoxMinCornerDistanceRate->setSingleStep(0.01);    

    ui->doubleSpinBoxMinMarkerDistanceRate->setRange(0.01, 5.0);
    ui->doubleSpinBoxMinMarkerDistanceRate->setSingleStep(0.01);    

    ui->spinBoxMinDistanceToBorder->setRange(0, 500);
    ui->spinBoxMinDistanceToBorder->setSingleStep(1);

    set_default_value_contour_filtering();

    connect(ui->doubleSpinBoxMinMarkerDistanceRate, &QDoubleSpinBox::valueChanged,
            this, &dialog_aruco_detector_params::marker_perimeter_rate_changed);
    connect(ui->doubleSpinBoxMaxMarkerPerimeterRate, &QDoubleSpinBox::valueChanged,
            this, &dialog_aruco_detector_params::marker_perimeter_rate_changed);
}

void dialog_aruco_detector_params::init_marker_identification()
{
    ui->doubleSpinBoxMaxErroneousBitsInBorderRate->setRange(0, 1.0);
    ui->doubleSpinBoxMaxErroneousBitsInBorderRate->setSingleStep(0.01);

    ui->doubleSpinBoxErrorCorrectionRate->setRange(0, 1.0);
    ui->doubleSpinBoxErrorCorrectionRate->setSingleStep(0.01);

    set_default_value_marker_identification();
}

void dialog_aruco_detector_params::init_threshold()
{
    ui->spinBoxAdaptiveThreshWinSizeMin->setRange(1, 51);
    ui->spinBoxAdaptiveThreshWinSizeMin->setSingleStep(2);

    ui->spinBoxAdaptiveThreshWinSizeMax->setRange(1, 51);
    ui->spinBoxAdaptiveThreshWinSizeMax->setSingleStep(2);

    ui->spinBoxAdaptiveThreshWinSizeStep->setRange(1, 15);

    ui->spinBoxAdaptiveThreshConstant->setRange(0, 127);

    set_default_value_threshold();

    connect(ui->spinBoxAdaptiveThreshWinSizeMin, &QSpinBox::valueChanged, this, &dialog_aruco_detector_params::adaptive_value_change);
    connect(ui->spinBoxAdaptiveThreshWinSizeMax, &QSpinBox::valueChanged, this, &dialog_aruco_detector_params::adaptive_value_change);
    connect(ui->spinBoxAdaptiveThreshWinSizeStep, &QSpinBox::valueChanged, this, &dialog_aruco_detector_params::adaptive_value_change);
}

void dialog_aruco_detector_params::marker_perimeter_rate_changed(double)
{
    if(ui->doubleSpinBoxMinMarkerPerimeterRate->value() > ui->doubleSpinBoxMaxMarkerPerimeterRate->value()){
        ui->doubleSpinBoxMinMarkerDistanceRate->setValue(ui->doubleSpinBoxMaxMarkerPerimeterRate->value());
    }

    if(ui->doubleSpinBoxMaxMarkerPerimeterRate->value() < ui->doubleSpinBoxMinMarkerPerimeterRate->value()){
        ui->doubleSpinBoxMaxMarkerPerimeterRate->setValue(ui->doubleSpinBoxMinMarkerPerimeterRate->value());
    }
}

void dialog_aruco_detector_params::adaptive_value_change(int)
{
    if(ui->spinBoxAdaptiveThreshWinSizeMin->value() > ui->spinBoxAdaptiveThreshWinSizeMax->value()){
        ui->spinBoxAdaptiveThreshWinSizeMin->setValue(ui->spinBoxAdaptiveThreshWinSizeMax->value());
    }

    if(ui->spinBoxAdaptiveThreshWinSizeMax->value() < ui->spinBoxAdaptiveThreshWinSizeMin->value()){
        ui->spinBoxAdaptiveThreshWinSizeMax->setValue(ui->spinBoxAdaptiveThreshWinSizeMin->value());
    }

    if(ui->spinBoxAdaptiveThreshWinSizeMax->value() % 2 == 0){
        ui->spinBoxAdaptiveThreshWinSizeMax->setValue(ui->spinBoxAdaptiveThreshWinSizeMax->value() + 1);
    }

    if(ui->spinBoxAdaptiveThreshWinSizeMin->value() % 2 == 0){
        ui->spinBoxAdaptiveThreshWinSizeMin->setValue(ui->spinBoxAdaptiveThreshWinSizeMin->value() + 1);
    }

    if(ui->spinBoxAdaptiveThreshWinSizeStep->value() >= ui->spinBoxAdaptiveThreshWinSizeMax->value()){
        ui->spinBoxAdaptiveThreshWinSizeStep->setValue(ui->spinBoxAdaptiveThreshWinSizeMax->value());
    }
}

void dialog_aruco_detector_params::on_pushButtonContourFilteringDefault_clicked()
{
    set_default_value_contour_filtering();
}


void dialog_aruco_detector_params::on_pushButtonThresholdDefault_clicked()
{
    set_default_value_threshold();
}


void dialog_aruco_detector_params::on_pushButtonBitsExtractionDefault_clicked()
{
    set_default_value_bits_extraction();
}


void dialog_aruco_detector_params::on_pushButtonMarkerIdentificationDefault_clicked()
{
    set_default_value_marker_identification();
}

