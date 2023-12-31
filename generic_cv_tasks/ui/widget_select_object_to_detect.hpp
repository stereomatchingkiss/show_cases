#pragma once

#include <QVector>
#include <QWidget>

#include <map>

class Ui_widget_select_object_to_detect;

struct config_select_object_to_detect;

class widget_select_object_to_detect : public QWidget
{
    Q_OBJECT

public:
    explicit widget_select_object_to_detect(std::vector<std::string> names, QWidget *parent = nullptr);
    ~widget_select_object_to_detect();

    config_select_object_to_detect get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private slots:
    void on_comboBoxSelectCategory_currentIndexChanged(int index);

    void on_pushButtonSelectAllCategories_clicked();

    void on_pushButtonUnSelectAllCategories_clicked();

    void on_pushButtonSelectAllItemsOfTheCategory_clicked();

    void on_pushButtonUnSelectAllItemsOfTheCategory_clicked();

private:
    QVector<int> create_category_indexes(int category) const;
    QVector<int> create_indexes(int begin, int end) const;
    QVector<int> create_person_index() const;

    void show_category(int index);

    Ui_widget_select_object_to_detect *ui;

    QVector<QVector<int>> category_indexes_;
    std::vector<std::string> names_;
};
