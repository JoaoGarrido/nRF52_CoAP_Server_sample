#include "ot_coap_example_2.h"

LOG_MODULE_REGISTER(ot_coap_example_2, CONFIG_OT_COAP_UTILS_LOG_LEVEL);

static void dodgy_pig_request_handler(void *context, otMessage *message, const otMessageInfo *message_info);
static otError dodgy_pig_response(otMessage *request_message, const otMessageInfo *message_info);
static void dodgy_pig_callback_t();

otCoapResource dodgy_pig_resource = {
	.mUriPath = URI_DODGY_PIG,
	.mHandler = NULL,
	.mContext = NULL,
	.mNext = NULL,
};

struct example_srv_context dodgy_pig_srv_ctx = {
    .resource = NULL,
    .enable = false,
    .callback = dodgy_pig_callback_t,
};

otCoapResource* dodgy_pig_init (otInstance* ot, bool enable){
    dodgy_pig_resource.mHandler = dodgy_pig_request_handler; 
    dodgy_pig_resource.mContext = ot;
    dodgy_pig_srv_ctx.enable = enable;
    return &dodgy_pig_resource;
}

static void dodgy_pig_request_handler(void *context, otMessage *message, const otMessageInfo *message_info){
    otError error;
	otMessageInfo msg_info;

	ARG_UNUSED(context);

	if (!dodgy_pig_srv_ctx.enable) {
		LOG_WRN("Received luminosity request but luminosity "
			"is disabled");
		return;
	}


	if ((otCoapMessageGetType(message) == OT_COAP_TYPE_CONFIRMABLE) &&
	    (otCoapMessageGetCode(message) == OT_COAP_CODE_GET)) {
        LOG_INF("Received luminosity request");
		msg_info = *message_info;
		memset(&msg_info.mSockAddr, 0, sizeof(msg_info.mSockAddr));

		error = dodgy_pig_response(message, &msg_info);
		if (error == OT_ERROR_NONE) {
			dodgy_pig_srv_ctx.callback();
		}
	}
	else{
		LOG_WRN("Received luminosity request but doens't match expected Type or Code");
		coap_default_response(dodgy_pig_resource.mContext, message, &msg_info);
	}
}

static otError dodgy_pig_response(otMessage *request_message, const otMessageInfo *message_info){
	otError error = OT_ERROR_NO_BUFS;
	otMessage *response;
	const void *payload;
	uint16_t payload_size;

	response = otCoapNewMessage(dodgy_pig_resource.mContext, NULL);
	if (response == NULL) {
		goto end;
	}

    //Init a response message with same message ID and token
    error = otCoapMessageInitResponse(
        response, 
        request_message,
        OT_COAP_TYPE_ACKNOWLEDGMENT,
        OT_COAP_CODE_CONTENT
    );
    if (error != OT_ERROR_NONE) {
		goto end;
	}
    
	error = otCoapMessageSetPayloadMarker(response);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	payload = otThreadGetMeshLocalEid(dodgy_pig_resource.mContext);
	payload_size = sizeof(otIp6Address);

	error = otMessageAppend(response, payload, payload_size);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	error = otCoapSendResponse(dodgy_pig_resource.mContext, response, message_info);
	if (error != OT_ERROR_NONE) {
		goto end;
	}
    
	LOG_HEXDUMP_INF(payload, payload_size, "Sent luminosity response:");

end:
	if (error != OT_ERROR_NONE && response != NULL) {
		otMessageFree(response);
	}

	return error;
}

static void dodgy_pig_callback_t(){
	LOG_INF("Luminosity callback");
}