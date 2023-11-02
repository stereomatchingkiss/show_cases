#include "global_object.hpp"

#include <multimedia/sound/alert_sound_manager.hpp>
#include <utils/meyers_singleton.hpp>

flt::mm::alert_sound_manager &get_alert_sound_manager()
{
    auto &manager = flt::meyers_singleton<flt::mm::alert_sound_manager>::get_instance(QUrl("assets/sound/alarm.mp3"));

    return manager;
}
