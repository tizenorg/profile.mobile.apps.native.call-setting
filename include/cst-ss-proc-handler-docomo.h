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

#ifndef _CST_SS_PROC_HANDLER_DOCOMO_H_
#define _CST_SS_PROC_HANDLER_DOCOMO_H_

#include "cst-common-docomo.h"

/*
 * @brief Send the SS request to the network.
 *
 * @param ss_msg The handle to the cst_req_resp_msg_docomo_t enum representing
 * the SS operation to be performed.
 *
 * @param user_data The handle to the user data which will be passed on to the
 * network and returned back in the network response.
 *
 * This function is the interface function for all the UI screens in the
 * call-setting which is responsible for placing a SS request to the telephony
 * server.
 *
 */
void _cst_ss_req_send_docomo(cst_req_resp_msg_docomo_t ss_msg,
								void *user_data);

#endif /* _CST_SS_PROC_HANDLER_DOCOMO_H_ */

