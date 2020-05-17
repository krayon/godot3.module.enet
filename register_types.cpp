/* register_types.cpp */

#include "register_types.h"

#include "core/error_macros.h"
#include "core/class_db.h"

#include "thirdparty/enet/enet/enet.h"

#include "gdnet_address.h"
#include "gdnet_event.h"
#include "gdnet_message.h"
#include "gdnet_peer.h"

void register_gdnet3_types() {
	ClassDB::register_virtual_class<GDNetPeer>();
	ClassDB::register_virtual_class<GDNetEvent>();
	ClassDB::register_virtual_class<GDNetMessage>();
	ClassDB::register_class<GDNetHost>();
	ClassDB::register_class<GDNetAddress>();

	if (enet_initialize() != 0)
		CRASH_NOW_MSG("Unable to initialize ENet. Can't recover.");
}

void unregister_gdnet3_types() {
	enet_deinitialize();
}
