#pragma once

namespace flt::mm{

class alert_sound_manager;

}

namespace flt::net{

class websocket_client_controller;

}

class dialog_alert_sender_settings;

flt::mm::alert_sound_manager& get_alert_sound_manager();
flt::net::websocket_client_controller& get_websocket_controller();
dialog_alert_sender_settings& get_widget_alert_sender_settings();

bool more_than_one_task() noexcept;


