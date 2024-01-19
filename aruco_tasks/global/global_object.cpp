#include "global_object.hpp"

#include <utils/meyers_singleton.hpp>

#include <QMessageBox>

QMessageBox& global_object::messagebox()
{
    return flt::meyers_singleton<QMessageBox>::get_instance();
}

global_object &get_gobject()
{
    return flt::meyers_singleton<global_object>::get_instance();
}
