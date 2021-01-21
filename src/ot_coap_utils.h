#ifndef __OT_COAP_UTILS_H__
#define __OT_COAP_UTILS_H__

#include "ot_common.h"
#include "ot_coap_example_1.h"
#include "ot_coap_example_2.h"
#include "ot_coap_example_3.h"

/**@brief Type definition of the function used to handle resources change.
 */

struct server_context {
	struct otInstance *ot;
	bool not_safe_enable;
	bool dodgy_pig_enable;
};

int ot_coap_init(bool not_safe_enable, bool dodgy_pig_enable, bool divorce_confirmation_enable);

#endif
