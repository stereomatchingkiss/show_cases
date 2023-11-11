#pragma once

namespace flt::mm{

class alert_sound_manager;

}

namespace flt::net{

class websocket_client_thread_safe;

}

flt::mm::alert_sound_manager& get_alert_sound_manager();
flt::net::websocket_client_thread_safe& get_websocket_client_thread_safe();
