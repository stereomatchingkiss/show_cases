#include "widget_select_object_to_detect.hpp"
#include "ui_widget_select_object_to_detect.h"

#include "../config/select_object_to_detect_config.hpp"

#include <cv_algo/obj_detect/coco_object_index.hpp>

#include <ui/utils/qtable_item_utils.hpp>

#include <QDebug>

#include <QCheckBox>

#include <QJsonArray>
#include <QJsonObject>

using namespace ocv::det;
using namespace ocv::ui;

namespace{

QString const state_obj_model_select("state_obj_model_select");
QString const state_obj_det_selected_items("state_obj_det_selected_items");

}

widget_select_object_to_detect::widget_select_object_to_detect(std::vector<std::string> names, QWidget *parent) :
    QWidget(parent),
    ui(new Ui_widget_select_object_to_detect),
    names_(std::move(names))
{
    ui->setupUi(this);

    ui->tableWidget->setColumnHidden(2, true);
    for(int i = 0; i != names_.size(); ++i){
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(names_[i].c_str()));
        ui->tableWidget->setCellWidget(i, 1, create_checkbox());
    }

    //todo : make category selection easier to reuse
    ui->comboBoxSelectCategory->addItem(tr("Accessory"));
    ui->comboBoxSelectCategory->addItem(tr("Animal"));
    ui->comboBoxSelectCategory->addItem(tr("Appliance"));
    ui->comboBoxSelectCategory->addItem(tr("Electronic"));
    ui->comboBoxSelectCategory->addItem(tr("Food"));
    ui->comboBoxSelectCategory->addItem(tr("Furniture"));
    ui->comboBoxSelectCategory->addItem(tr("Indoor"));
    ui->comboBoxSelectCategory->addItem(tr("Kitchen"));
    ui->comboBoxSelectCategory->addItem(tr("Outdoor"));
    ui->comboBoxSelectCategory->addItem(tr("Person"));
    ui->comboBoxSelectCategory->addItem(tr("Sports"));
    ui->comboBoxSelectCategory->addItem(tr("Vehicle"));

    ui->comboBoxSelectCategory->setCurrentIndex(static_cast<int>(coco_object_category::Person));

    show_category(static_cast<int>(coco_object_category::Person));
}

widget_select_object_to_detect::~widget_select_object_to_detect()
{        
    delete ui;
}

select_object_to_detect_config widget_select_object_to_detect::get_config() const
{
    select_object_to_detect_config config;
    for(int i = 0; i != names_.size(); ++i){
        if(access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->isChecked()){
            config.selected_object_.insert(names_[i]);
        }
    }

    return config;
}

QJsonObject widget_select_object_to_detect::get_states() const
{
    QJsonObject obj;    
    QJsonArray selected_items;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        selected_items.push_back(access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->isChecked());
    }
    obj[state_obj_det_selected_items] = selected_items;

    return obj;
}

void widget_select_object_to_detect::set_states(const QJsonObject &val)
{    
    if(val.contains(state_obj_det_selected_items)){
        auto const arr = val[state_obj_det_selected_items].toArray();
        for(int i = 0; i != arr.size(); ++i){
            access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->setChecked(arr[i].toBool());
        }
    }
}

void widget_select_object_to_detect::on_comboBoxSelectCategory_currentIndexChanged(int index)
{
    show_category(index);
}

QVector<int> widget_select_object_to_detect::create_category_indexes(int category) const
{
    switch(static_cast<coco_object_category>(category)){
    case coco_object_category::Accessory:
        return create_indexes(24, 28);
    case coco_object_category::Animal:
        return create_indexes(14, 23);
    case coco_object_category::Appliance:
        return create_indexes(68, 72);
    case coco_object_category::Electronic:
        return create_indexes(62, 67);
    case coco_object_category::Food:
        return create_indexes(46, 55);
    case coco_object_category::Furniture:
        return create_indexes(56, 61);
    case coco_object_category::Indoor:
        return create_indexes(73, 79);
    case coco_object_category::Kitchen:
        return create_indexes(39, 45);
    case coco_object_category::Outdoor:
        return create_indexes(9, 13);
    case coco_object_category::Person:
        return create_person_index();
    case coco_object_category::Sports:
        return create_indexes(29, 38);
    case coco_object_category::Vehicle:
        return create_indexes(1, 8);
    }

    return create_person_index();
}

QVector<int> widget_select_object_to_detect::create_indexes(int begin, int end) const
{
    QVector<int> result;
    for(int i = begin; i <= end; ++i){
        result.push_back(i);
    }

    return result;
}

QVector<int> widget_select_object_to_detect::create_person_index() const
{
    QVector<int> result;
    result.push_back(0);

    return result;
}

void widget_select_object_to_detect::show_category(int index)
{    
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        ui->tableWidget->hideRow(i);
    }
    auto const index_to_show = create_category_indexes(index);
    for(int i = 0; i != index_to_show.size(); ++i){
        ui->tableWidget->showRow(index_to_show[i]);
    }
}

void widget_select_object_to_detect::on_pushButtonSelectAllCategories_clicked()
{
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->setChecked(true);
    }
}

void widget_select_object_to_detect::on_pushButtonUnSelectAllCategories_clicked()
{
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->setChecked(false);
    }
}

void widget_select_object_to_detect::on_pushButtonSelectAllItemsOfTheCategory_clicked()
{
    auto const index_to_show = create_category_indexes(ui->comboBoxSelectCategory->currentIndex());
    for(int i = 0; i != index_to_show.size(); ++i){
        access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(index_to_show[i], 1))->setChecked(true);
    }
}

void widget_select_object_to_detect::on_pushButtonUnSelectAllItemsOfTheCategory_clicked()
{
    auto const index_to_show = create_category_indexes(ui->comboBoxSelectCategory->currentIndex());
    for(int i = 0; i != index_to_show.size(); ++i){
        access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(index_to_show[i], 1))->setChecked(false);
    }
}
