#include "ot_coap_utils.h"

LOG_MODULE_REGISTER(ot_coap_utils, CONFIG_OT_COAP_UTILS_LOG_LEVEL);

static struct server_context srv_context = {
	.ot = NULL,
	.not_safe_enable = true,
	.dodgy_pig_enable = true,
};

static void coap_default_handler(void *context, otMessage *message, const otMessageInfo *message_info){
	ARG_UNUSED(context);
    
	otError error;
	otMessageInfo msg_info;

	LOG_INF("Received CoAP message that does not match any request or resource");

	msg_info = *message_info;
	memset(&msg_info.mSockAddr, 0, sizeof(msg_info.mSockAddr));
	error = coap_default_response(srv_context.ot, message, &msg_info);
}

int ot_coap_init(bool not_safe_enable, bool dodgy_pig_enable, bool divorce_confirmation_enable){
	otError error;
	
	srv_context.ot = openthread_get_default_instance();
	if (!srv_context.ot) {
		LOG_ERR("There is no valid OpenThread instance");
		error = OT_ERROR_FAILED;
		goto end;
	}

	otCoapResource* not_safe_resource = not_safe_init(srv_context.ot, not_safe_enable);
	otCoapResource* dodgy_pig_resource = dodgy_pig_init(srv_context.ot, dodgy_pig_enable);
	otCoapResource* divorce_confirmation_resource = divorce_confirmation_init(srv_context.ot, divorce_confirmation_enable);

	otCoapSetDefaultHandler(srv_context.ot, coap_default_handler, NULL);
	otCoapAddResource(srv_context.ot, not_safe_resource);
	otCoapAddResource(srv_context.ot, dodgy_pig_resource);
	otCoapAddResource(srv_context.ot, divorce_confirmation_resource);

	error = otCoapStart(srv_context.ot, COAP_PORT);
	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to start OT CoAP. Error: %d", error);
		goto end;
	}

end:
	return error == OT_ERROR_NONE ? 0 : 1;
}