#pragma once

namespace flt::mm{

class alert_sound_manager;

}

namespace flt::net{

class websocket_client_worker;

}

flt::mm::alert_sound_manager& get_alert_sound_manager();
flt::net::websocket_client_worker& get_websocket_client();
