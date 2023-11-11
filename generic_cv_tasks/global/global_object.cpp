#include "global_object.hpp"

#include <multimedia/sound/alert_sound_manager.hpp>
#include <network/websocket_client.hpp>
#include <utils/meyers_singleton.hpp>

flt::mm::alert_sound_manager &get_alert_sound_manager()
{
#ifndef WASM_BUILD    
    return flt::meyers_singleton<flt::mm::alert_sound_manager>::get_instance(QUrl("assets/sound/alarm.mp3"));
#else
    return flt::meyers_singleton<flt::mm::alert_sound_manager>::get_instance(QUrl("alarm.mp3"));
#endif    
}

flt::net::websocket_client_thread_safe &get_websocket_client_thread_safe()
{
    return flt::meyers_singleton<flt::net::websocket_client_thread_safe>::get_instance();
}
