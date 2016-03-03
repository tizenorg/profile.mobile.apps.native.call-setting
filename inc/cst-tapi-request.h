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

#ifndef _CST_TAPI_REQUEST_H_
#define _CST_TAPI_REQUEST_H_
#include "cst-common.h"
#include <Eina.h>
#include <Elementary.h>

typedef void (*SsReqCb)(int call_type, int flavour, Eina_Bool ss_state, char *number, int error, int req_action, void *, int waiting_time);

typedef struct {
	int call_type;
	int flavour;
	int action;
	int req_id;
	Eina_Bool is_requesting;
	Eina_Bool is_canceled;
	Eina_Bool original_state;
	SsReqCb func;
	void *data;
	char number[CST_MAX_PHONE_NUMBER_LEN];
	int waiting_time;
	CstAppData_t *ad;
	char old_password[CST_MAX_PHONE_NUMBER_LEN];
	char new_password[CST_MAX_PHONE_NUMBER_LEN];
	char confirm_password[CST_MAX_PHONE_NUMBER_LEN];
} CallSettingSSReq_t;

void _cst_register_tel_event(void *data);
void _cst_deregister_tel_event(void *data);
void _cst_add_ss_request(Eina_List **queue, int action_type, int call_type, int flavour, int original_state, char *number, void *func, void *data, int waiting_time, CstAppData_t *ad);
void _cst_cancel_all_ss_request(void *data);
void _cst_change_password_ss_request(CallSettingSSReq_t *req, void *func);

void _cst_update_tapi_handle_by_simslot(void *data, CstSimSlot_t slot_id);
Eina_Bool _cst_is_sim_present_by_simslot(void *data, CstSimSlot_t slot_id);

#endif  /* _CST_TAPI_REQUEST_H_ */

