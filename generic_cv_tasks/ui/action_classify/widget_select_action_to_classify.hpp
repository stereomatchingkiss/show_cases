#ifndef WIDGET_SELECT_ACTION_TO_CLASSIFY_HPP
#define WIDGET_SELECT_ACTION_TO_CLASSIFY_HPP

#include <QWidget>

namespace Ui {
class widget_select_action_to_classify;
}

struct config_select_action_to_classify;

class QTimer;

class widget_select_action_to_classify : public QWidget
{
    Q_OBJECT

public:
    explicit widget_select_action_to_classify(QWidget *parent = nullptr);
    ~widget_select_action_to_classify();

    config_select_action_to_classify get_config() const;

    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private slots:
    void on_lineEditSearchAction_textChanged(const QString&);

    void on_pushButtonSelectAll_clicked();

    void on_pushButtonUnSelectUnselectAll_clicked();

private:
    void search_similar_keywords();

    Ui::widget_select_action_to_classify *ui;

    QTimer *timer_;        
};

#endif // WIDGET_SELECT_ACTION_TO_CLASSIFY_HPP
