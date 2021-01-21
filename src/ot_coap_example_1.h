#ifndef __OT_COAP_EXAMPLE_1_H__
#define __OT_COAP_EXAMPLE_1_H__

#include "ot_common.h"

#define URI_NOT_SAFE "temperature"

otCoapResource* not_safe_init (otInstance* ot, bool enable);
// void not_safe_enable ();
// void not_safe_disbale ();
// bool not_safe_state();

#endif