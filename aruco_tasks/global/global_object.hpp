#pragma once

#include <QString>

class QMessageBox;

class global_object
{
public:
    QMessageBox& messagebox();
};

global_object& get_gobject();
