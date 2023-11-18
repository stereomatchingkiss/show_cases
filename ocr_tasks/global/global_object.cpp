#include "global_object.hpp"

#include <utils/meyers_singleton.hpp>

#include <QMessageBox>

QMessageBox &global_get_messagebox()
{
    return flt::meyers_singleton<QMessageBox>::get_instance();
}
