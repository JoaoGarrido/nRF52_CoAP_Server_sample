#include "ot_common.h"

LOG_MODULE_REGISTER(ot_common, CONFIG_OT_COAP_UTILS_LOG_LEVEL);

otError coap_default_response(struct otInstance *ot, otMessage *request_message, const otMessageInfo *message_info){
	otError error = OT_ERROR_NO_BUFS;
	otMessage *response;

	response = otCoapNewMessage(ot, NULL);
	if (response == NULL) {
		goto end;
	}

    //Init a response message with same message ID and token
    error = otCoapMessageInitResponse(
        response, 
        request_message,
        OT_COAP_TYPE_RESET,
        OT_COAP_CODE_EMPTY
    );
    if (error != OT_ERROR_NONE) {
		goto end;
	}

	error = otCoapSendResponse(ot, response, message_info);
	if (error != OT_ERROR_NONE) {
		goto end;
	}
    
	LOG_INF("Sent CoAP Reset message");

end:
	if (error != OT_ERROR_NONE && response != NULL) {
		otMessageFree(response);
	}

	return error;
}