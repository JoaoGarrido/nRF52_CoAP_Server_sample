#include "ot_coap_example_3.h"

LOG_MODULE_REGISTER(ot_coap_example_3, CONFIG_OT_COAP_UTILS_LOG_LEVEL);

static void divorce_confirmation_request_handler(void *context, otMessage *message, const otMessageInfo *message_info);
static otError divorce_confirmation_response(otMessage *request_message, const otMessageInfo *message_info);
static void divorce_confirmation_callback_t();

otCoapResource divorce_confirmation_resource = {
	.mUriPath = URI_DIVORCE_CONFIRMATION,
	.mHandler = NULL,
	.mContext = NULL,
	.mNext = NULL,
};

struct example_srv_context divorce_confirmation_srv_ctx = {
    .resource = NULL,
    .enable = false,
    .callback = divorce_confirmation_callback_t,
};

otCoapResource* divorce_confirmation_init (otInstance* ot, bool enable){
    divorce_confirmation_resource.mHandler = divorce_confirmation_request_handler; 
    divorce_confirmation_resource.mContext = ot;
    divorce_confirmation_srv_ctx.enable = enable;
    return &divorce_confirmation_resource;
}

static void divorce_confirmation_request_handler(void *context, otMessage *message, const otMessageInfo *message_info){
    otError error;
	otMessageInfo msg_info;

	ARG_UNUSED(context);

	if (!divorce_confirmation_srv_ctx.enable) {
		LOG_WRN("Received air quality request but air quality "
			"is disabled");
		return;
	}


	if ((otCoapMessageGetType(message) == OT_COAP_TYPE_CONFIRMABLE) &&
	    (otCoapMessageGetCode(message) == OT_COAP_CODE_GET)) {
        LOG_INF("Received air quality request");
		msg_info = *message_info;
		memset(&msg_info.mSockAddr, 0, sizeof(msg_info.mSockAddr));

		error = divorce_confirmation_response(message, &msg_info);
		if (error == OT_ERROR_NONE) {
			divorce_confirmation_srv_ctx.callback();
		}
	}
	else if(otCoapMessageGetType(message) == OT_COAP_TYPE_ACKNOWLEDGMENT){
		LOG_INF("Received air quality response ack");
	}
	else{
		LOG_WRN("Received air quality request but doens't match expected Type or Code");
		coap_default_response(divorce_confirmation_resource.mContext, message, &msg_info);
	}
}

static otError divorce_confirmation_response(otMessage *request_message, const otMessageInfo *message_info){
	otError error = OT_ERROR_NO_BUFS;
	otMessage *response;
	otMessage *ack;
	const void *payload;
	uint16_t payload_size;

	ack = otCoapNewMessage(divorce_confirmation_resource.mContext, NULL);
	response = otCoapNewMessage(divorce_confirmation_resource.mContext, NULL);
	if (ack == NULL || response == NULL) {
		goto end;
	}

    //ACK Response
    error = otCoapMessageInitResponse(
        ack, 
        request_message,
        OT_COAP_TYPE_ACKNOWLEDGMENT,
        OT_COAP_CODE_EMPTY
    );
    if (error != OT_ERROR_NONE) {
		goto end;
	}
    
	error = otCoapSendResponse(divorce_confirmation_resource.mContext, ack, message_info);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	LOG_INF("Sent air quality ack");

	//Confirmable Content Message Response
	otCoapMessageInit(
		response, 
		OT_COAP_TYPE_CONFIRMABLE,
		OT_COAP_CODE_CONTENT
	);

	error = otCoapMessageSetToken(
		response, 
		otCoapMessageGetToken(request_message),
		otCoapMessageGetTokenLength(request_message)
	);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	error = otCoapMessageSetPayloadMarker(response);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	payload = otThreadGetMeshLocalEid(divorce_confirmation_resource.mContext);
	payload_size = sizeof(otIp6Address);

	error = otMessageAppend(response, payload, payload_size);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	error = otCoapSendResponse(divorce_confirmation_resource.mContext, response, message_info);
	if (error != OT_ERROR_NONE) {
		goto end;
	}

	LOG_HEXDUMP_INF(payload, payload_size, "Sent air quality response:");

end:
	if (error != OT_ERROR_NONE && ack != NULL) {
		otMessageFree(ack);
	}

	if (error != OT_ERROR_NONE && response != NULL) {
		otMessageFree(response);
	}

	return error;
}

static void divorce_confirmation_callback_t(){
	LOG_INF("air quality callback");
}