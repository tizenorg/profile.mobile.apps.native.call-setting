/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef _CST_USSD_PROC_HANDLER_DOCOMO_H_
#define _CST_USSD_PROC_HANDLER_DOCOMO_H_

#include "cst-common-docomo.h"

/* Signature of the callback function which is called from the n/w */
typedef void (*cst_ussd_resp_docomo_cb)(void *user_data, char *resp_str);

/*
 * @brief Set the handle of the callback function from the UI.
 *
 * @param resp_func The handle to the function of type
 * 'cst_ussd_resp_ui_docomo_cb'.
 *
 * @param data The handle to the user data.
 *
 * This function sets the passed handle of type 'cst_ussd_resp_ui_docomo_cb'
 * such that the handle from the UI is called once the response from the
 * network is received.
 *
 * @see cst_ussd_unset_docomo_cb()
 */
void _cst_ussd_set_docomo_cb(cst_ussd_resp_ui_docomo_cb resp_func, void *data);

/*
 * @brief Unset the handle of the callback function if any.
 *
 * @param data The handle to the user data.
 *
 * This function unset's the handle is any from the UI if set, such that no
 * response is sent to any UI handle after receiving the USSD response from the
 * network.
 *
 * @see cst_ussd_set_docomo_cb()
 */
void _cst_ussd_unset_docomo_cb(void *data);

/*
 * @brief Send the USSD request to the network.
 *
 * @param ussd_req_data The handle to the cst_req_ussd_data_docomo_t structure
 * containing all the information required to generate the USSD string.
 *
 * @param user_data The handle to the user data which will be passed on to the
 * network and returned back in the network response.
 *
 * This function is the interface function for all the UI screens in the
 * call-setting which is responsible for placing a request to generate the USSD
 * string and call the telephony API to send the request to the network.
 *
 */
void _cst_ussd_req_send_docomo(cst_req_ussd_data_docomo_t *ussd_req_data,
							   void *user_data);

#endif /* _CST_USSD_PROC_HANDLER_DOCOMO_H_ */

