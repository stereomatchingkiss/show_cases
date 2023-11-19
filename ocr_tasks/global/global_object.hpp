#pragma once

#include <QString>

class QMessageBox;

class global_object
{
public:
    global_object();

    QMessageBox& messagebox();
    QString const& font_family() const;

private:
    QString font_family_;
};

global_object& get_gobject();
