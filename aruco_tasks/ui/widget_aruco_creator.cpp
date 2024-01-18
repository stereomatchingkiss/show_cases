#include "widget_aruco_creator.hpp"
#include "ui_widget_aruco_creator.h"

#include <utils/image_utils.hpp>

#include <QDataStream>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>

#include <opencv2/aruco.hpp>

using namespace flt;

widget_aruco_creator::widget_aruco_creator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_aruco_creator),
    timer_generate_aruco_{new QTimer(this)}
{
    ui->setupUi(this);

    timer_generate_aruco_->setSingleShot(true);
    timer_generate_aruco_->setInterval(100);

    init_aruco_combo_box();

    ui->spinBoxMarkerBorderBit->setRange(1, 5);
    ui->horizontalSliderMarkerBorderBit->setRange(1, 5);

    ui->spinBoxMarkerSize->setRange(100, 800);
    ui->horizontalSliderMarkerSize->setRange(ui->spinBoxMarkerSize->minimum(), ui->spinBoxMarkerSize->maximum());

    ui->comboBoxDictionary->setCurrentIndex(0);

    ui->spinBoxMarkerID->setValue(1);
    ui->spinBoxMarkerBorderBit->setValue(1);

    generate_aruco_image(0);

    connect(timer_generate_aruco_, &QTimer::timeout, this, &widget_aruco_creator::generate_aruco_image_time_up);
}

widget_aruco_creator::~widget_aruco_creator()
{
    delete ui;
}

void widget_aruco_creator::on_pushButtonSaveAs_clicked()
{
    if(!qimg_aruco_.isNull()){
        auto const aruco_name = ui->comboBoxDictionary->currentText() + "_" + QString::number(ui->spinBoxMarkerID->value()) + ".png";
#ifndef WASM_BUILD
        if(auto const fname = QFileDialog::getSaveFileName(this, tr("Save at"), aruco_name, tr("Images (*.png)"));
            !fname.isEmpty())
        {
            qimg_aruco_.save(fname, "PNG");
        }
#else
        auto results = encode_qimage_to(qimg_aruco_, "PNG");
        QFileDialog::saveFileContent(results, aruco_name);
#endif
    }
}

void widget_aruco_creator::generate_aruco_option(int bit_size)
{
    aruco_size_.emplace_back(50);
    aruco_size_.emplace_back(100);
    aruco_size_.emplace_back(250);
    aruco_size_.emplace_back(1000);

    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_50").arg(QString::number(bit_size)));
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_100").arg(QString::number(bit_size)));
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_250").arg(QString::number(bit_size)));
    ui->comboBoxDictionary->addItem(QString("DICT_%1X%1_1000").arg(QString::number(bit_size)));
}

void widget_aruco_creator::init_aruco_combo_box()
{
    generate_aruco_option(4);
    generate_aruco_option(5);
    generate_aruco_option(6);
    generate_aruco_option(7);

    aruco_size_.emplace_back(1024);
    ui->comboBoxDictionary->addItem("DICT_ARUCO_ORIGINAL");

    aruco_size_.emplace_back(30);
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_16h5");

    aruco_size_.emplace_back(35);
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_25h9");

    aruco_size_.emplace_back(2320);
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_36h10");

    aruco_size_.emplace_back(587);
    ui->comboBoxDictionary->addItem("DICT_APRILTAG_36h11");

    aruco_size_.emplace_back(250);
    ui->comboBoxDictionary->addItem("DICT_ARUCO_MIP_36h12");
}

void widget_aruco_creator::set_marker_range(int min, int max)
{
    ui->spinBoxMarkerID->setRange(0, max);
    ui->horizontalSliderMarkerID->setRange(0, max);
}

void widget_aruco_creator::on_comboBoxDictionary_currentIndexChanged(int)
{
    set_marker_range(0, aruco_size_[ui->comboBoxDictionary->currentIndex()]);
    generate_aruco_image(0);
}

void widget_aruco_creator::generate_aruco_image(int id)
{
    cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(ui->comboBoxDictionary->currentIndex());
    cv::Mat output;
    cv::aruco::generateImageMarker(dict, id, ui->spinBoxMarkerSize->value(), output,
                                   std::max(ui->spinBoxMarkerBorderBit->value(), 1));

    qimg_aruco_ = QImage(output.data, output.cols, output.rows, output.step, QImage::Format_Grayscale8).copy();
    ui->labelImage->setPixmap(QPixmap::fromImage(qimg_aruco_));
}

void widget_aruco_creator::generate_aruco_image_time_up()
{
    generate_aruco_image(ui->spinBoxMarkerID->value());
}

void widget_aruco_creator::on_spinBoxMarkerID_valueChanged(int arg1)
{
    timer_generate_aruco_->start();
}

void widget_aruco_creator::on_spinBoxMarkerSize_valueChanged(int arg1)
{
    timer_generate_aruco_->start();
}

void widget_aruco_creator::on_spinBoxMarkerBorderBit_valueChanged(int arg1)
{
    timer_generate_aruco_->start();
}

