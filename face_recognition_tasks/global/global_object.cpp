#include "global_object.hpp"

#include <network/websocket_client_controller.hpp>
#include <utils/meyers_singleton.hpp>

flt::net::websocket_client_controller& get_websocket_controller()
{
    return flt::meyers_singleton<flt::net::websocket_client_controller>::get_instance();
}
