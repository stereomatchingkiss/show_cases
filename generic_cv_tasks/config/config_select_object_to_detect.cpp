#include "config_select_object_to_detect.hpp"

#include <QJsonArray>
#include <QJsonObject>

namespace{

QString const state_selected_object_("state_selected_object");

}

QJsonObject config_select_object_to_detect::get_states() const
{    
    QJsonArray labels;
    for(auto const &val : selected_object_){
        labels.push_back(val.c_str());
    }

    QJsonObject obj;
    obj[state_selected_object_] = labels;

    return obj;
}

void config_select_object_to_detect::set_states(const QJsonObject &val)
{
    if(val.contains(state_selected_object_)){
        auto const labels = val[state_selected_object_].toArray();
        for(auto const &v : labels){
            selected_object_.insert(v.toString().toStdString());
        }
    }    
}
