#include "ot_coap_example_1.h"

LOG_MODULE_REGISTER(ot_coap_example_1, CONFIG_OT_COAP_UTILS_LOG_LEVEL);

static void not_safe_request_handler(void *context, otMessage *message, const otMessageInfo *message_info);
static otError not_safe_response(otMessage *request_message, const otMessageInfo *message_info);
static void not_safe_callback_t();

otCoapResource not_safe_resource = {
	.mUriPath = URI_NOT_SAFE,
	.mHandler = NULL,
	.mContext = NULL,
	.mNext = NULL,
};

struct example_srv_context not_safe_srv_ctx = {
    .resource = NULL,
    .enable = false,
    .callback = not_safe_callback_t,
};

otCoapResource* not_safe_init (otInstance* ot, bool enable){
    not_safe_resource.mHandler = not_safe_request_handler; 
    not_safe_resource.mContext = ot;
    not_safe_srv_ctx.enable = enable;
    return &not_safe_resource;
}

static void not_safe_request_handler(void *context, otMessage *message, const otMessageInfo *message_info){
    otError error;
	otMessageInfo msg_info;

	ARG_UNUSED(context);

	if (!not_safe_srv_ctx.enable) {
		LOG_WRN("Received temperature request but temperature "
			"is disabled");
		return;
	}


	if ((otCoapMessageGetType(message) == OT_COAP_TYPE_NON_CONFIRMABLE) &&
	    (otCoapMessageGetCode(message) == OT_COAP_CODE_GET)) {
		LOG_INF("Received temperature request");
		msg_info = *message_info;
		memset(&msg_info.mSockAddr, 0, sizeof(msg_info.mSockAddr));

		error = not_safe_response(message, &msg_info);
		if (error == OT_ERROR_NONE) {
			not_safe_srv_ctx.callback();
		}
	}
	else{
		LOG_WRN("Received temperature request but doens't match expected Type or Code");
		coap_default_response(not_safe_resource.mContext, message, &msg_info);
	}
}

static otError not_safe_response(otMessage *request_message, const otMessageInfo *message_info){
	otError error = OT_ERROR_NO_BUFS;
	otMessage *response;
	const void *payload;
	uint16_t payload_size;

	response = otCoapNewMessage(not_safe_resource.mContext, NULL);
	if (response == NULL) {
		goto end;
	}

	otCoapMessageInit(response, OT_COAP_TYPE_NON_CONFIRMABLE,
			  OT_COAP_CODE_CONTENT);

	error = otCoapMessageSetToken(
		response, otCoapMessageGetToken(request_message),
		otCoapMessageGetTokenLength(request_message));
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	error = otCoapMessageSetPayloadMarker(response);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	payload = otThreadGetMeshLocalEid(not_safe_resource.mContext);
	payload_size = sizeof(otIp6Address);

	error = otMessageAppend(response, payload, payload_size);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	error = otCoapSendResponse(not_safe_resource.mContext, response, message_info);
	if (error != OT_ERROR_NONE) {
		goto end;
	}
	
	LOG_HEXDUMP_INF(payload, payload_size, "Sent temperature response:");

end:
	if (error != OT_ERROR_NONE && response != NULL) {
		otMessageFree(response);
	}

	return error;
}

static void not_safe_callback_t(){
	LOG_INF("Temperature callback");
}