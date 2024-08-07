#include "global_object.hpp"

#include "../ui/dialog_alert_sender_settings.hpp"

#include <multimedia/sound/alert_sound_manager.hpp>
#include <network/simple_email_sender.hpp>
#include <network/websocket_client_controller.hpp>
#include <utils/meyers_singleton.hpp>
#include <utils/unique_name_generator.hpp>

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

dialog_alert_sender_settings &get_widget_alert_sender_settings()
{
    return flt::meyers_singleton<dialog_alert_sender_settings>::get_instance();
}

bool more_than_one_task() noexcept
{
    int task_count = 0;

#ifdef ACTION_CLASSIFY_IS_ON
    ++task_count;
#endif

#ifdef FALL_DOWN_DET_IS_ON
    ++task_count;
#endif

#ifdef OBJ_DET_IS_ON
    ++task_count;
#endif

    return task_count > 1;
}

flt::unique_name_generator &get_unique_name_generator()
{
    return flt::meyers_singleton<flt::unique_name_generator>::get_instance("cam_");
}

flt::net::simple_email_sender &get_simple_email_sender()
{
    return flt::meyers_singleton<flt::net::simple_email_sender>::get_instance("", "", "dudulu");
}
