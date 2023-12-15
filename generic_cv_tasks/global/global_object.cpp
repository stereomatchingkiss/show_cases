#include "global_object.hpp"

#include "../ui/widget_alert_sender_settings.hpp"

#include <multimedia/sound/alert_sound_manager.hpp>
#include <network/websocket_client_controller.hpp>
#include <utils/meyers_singleton.hpp>

flt::mm::alert_sound_manager &get_alert_sound_manager()
{
#ifndef WASM_BUILD    
    return flt::meyers_singleton<flt::mm::alert_sound_manager>::get_instance(QUrl("assets/sound/alarm.mp3"));
#else
    return flt::meyers_singleton<flt::mm::alert_sound_manager>::get_instance(QUrl("alarm.mp3"));
#endif    
}

flt::net::websocket_client_controller& get_websocket_controller()
{
    return flt::meyers_singleton<flt::net::websocket_client_controller>::get_instance();
}

widget_alert_sender_settings &get_widget_alert_sender_settings()
{
    return flt::meyers_singleton<widget_alert_sender_settings>::get_instance();
}
