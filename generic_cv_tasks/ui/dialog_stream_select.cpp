#include "dialog_stream_select.hpp"
#include "ui_dialog_stream_select.h"

#include <ui/utils/qtable_item_utils.hpp>

#include <QDebug>

#include <QCheckBox>

using namespace flt::ui;

dialog_stream_select::dialog_stream_select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_stream_select)
{
    ui->setupUi(this);
}

dialog_stream_select::~dialog_stream_select()
{
    delete ui;
}

std::vector<QString> dialog_stream_select::get_selected_streams() const
{
    std::vector<QString> results;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        if(access_cell_widget<QCheckBox>(ui->tableWidget->cellWidget(i, 1))->isChecked()){
            results.emplace_back(ui->tableWidget->item(i, 0)->text());
        }
    }

    return results;
}

void dialog_stream_select::set_streams(std::vector<QString> const &names)
{
    ui->tableWidget->setRowCount(names.size());
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(names[i]));
        ui->tableWidget->setCellWidget(i, 1, create_checkbox());
    }
}
