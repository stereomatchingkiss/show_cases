#pragma once

namespace flt{

class unique_name_generator;

}

namespace flt::mm{

class alert_sound_manager;

}

namespace flt::net{

class simple_email_sender;
class websocket_client_controller;

}

class dialog_alert_sender_settings;

flt::mm::alert_sound_manager& get_alert_sound_manager();
flt::net::simple_email_sender& get_simple_email_sender();
flt::net::websocket_client_controller& get_websocket_controller();
dialog_alert_sender_settings& get_widget_alert_sender_settings();
flt::unique_name_generator& get_unique_name_generator();

bool more_than_one_task() noexcept;


