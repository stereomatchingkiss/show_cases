#ifndef OBJECT_DETECT_ALGO_WIDGET_HPP
#define OBJECT_DETECT_ALGO_WIDGET_HPP

#include <QVector>
#include <QWidget>

#include <map>

namespace Ui {
class object_detect_algo_widget;
}

struct object_detect_config;

class object_detect_algo_widget : public QWidget
{
    Q_OBJECT

public:
    explicit object_detect_algo_widget(std::vector<std::string> names, QWidget *parent = nullptr);
    ~object_detect_algo_widget();

    object_detect_config get_config() const;
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

    QStringList create_coco_names() const;
    QStringList create_raw_coco_names() const;
    void show_category(int index);

    Ui::object_detect_algo_widget *ui;

    QVector<QVector<int>> category_indexes_;
    std::vector<std::string> names_;
};

#endif // OBJECT_DETECT_ALGO_WIDGET_HPP
