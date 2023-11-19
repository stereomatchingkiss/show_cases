#include "global_object.hpp"

#include <utils/meyers_singleton.hpp>

#include <QFontDatabase>
#include <QMessageBox>

QMessageBox& global_object::messagebox()
{
    return flt::meyers_singleton<QMessageBox>::get_instance();
}

const QString &global_object::font_family() const
{
    return font_family_;
}

global_object::global_object()
{
#ifdef WASM_BUILD
    if(int const id = QFontDatabase::addApplicationFont("GB2312.ttf"); id >= 0){
        font_family_ = QFontDatabase::applicationFontFamilies(id).at(0);        
    }
#else
    font_family_ = "";
#endif
}

global_object &get_gobject()
{
    return flt::meyers_singleton<global_object>::get_instance();
}
