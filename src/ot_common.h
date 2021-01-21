#ifndef __OT_COMMON_H__
#define __OT_COMMON_H__

#include <coap_server_client_interface.h>
#include <logging/log.h>
#include <net/net_pkt.h>
#include <net/net_l2.h>
#include <net/openthread.h>
#include <openthread/coap.h>
#include <openthread/ip6.h>
#include <openthread/message.h>
#include <openthread/thread.h>

typedef void (*function)();

struct example_srv_context {
	otCoapResource resource;
	bool enable;
	function callback; 
};

otError coap_default_response(struct otInstance *ot, otMessage *request_message, const otMessageInfo *message_info);

#endif