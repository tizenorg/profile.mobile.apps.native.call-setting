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

#include <Eina.h>

#include <TelSs.h>
#include <ITapiSs.h>
#include <ITapiSim.h>
#include <ITapiPhonebook.h>
#include <TapiUtility.h>
#include <tapi_event.h>

#include "cst-tapi-request.h"
#include "cst-common.h"
#include "cst-common-string.h"

static void __cst_send_ss_req_to_telephony_server(CallSettingSSReq_t *req);
static int __cst_get_ss_error_from_tapi_error(TelSsCause_t tapi_err);
static gboolean __cst_ss_forward_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data);
static gboolean __cst_ss_waiting_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_remove_ss_request(void *data);

void _cst_register_tel_event(void *data)
{
	ENTER(_cst_register_tel_event);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd == NULL) {
		ERR("ugd = NULL, so returning");
		return;
	}

#ifdef _TIZEN_LITE_CODE
	char **cp_list = NULL;

	cp_list = tel_get_cp_name_list();
	if (cp_list ==  NULL) {
		ERR("tel_get_cp_name_list() failed.");
		return;
	}

	ugd->sim1_tapi_handle = tel_init(cp_list[0]); /*SIM1 slot*/
	if (ugd->sim1_tapi_handle == NULL) {
		ERR("tel_init() for SIM1 failed.");
	}

	ugd->sim2_tapi_handle = tel_init(cp_list[1]); /*SIM2 slot*/
	if (ugd->sim2_tapi_handle == NULL) {
		ERR("tel_init() for SIM2 failed.");
	}

	ugd->tapi_handle = ugd->sim1_tapi_handle; /*By default let SIM1 be the selected slot*/

	if (cp_list) {
		g_strfreev(cp_list);
	}
#else
	ugd->tapi_handle = tel_init(NULL);
	if (ugd->tapi_handle == NULL) {
		ERR("tel_init() failed.");
	}
#endif /* _TIZEN_LITE_CODE */

	LEAVE();
}

void _cst_deregister_tel_event(void *data)
{
	ENTER(_cst_deregister_tel_event);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int ret = -1;

	if (ugd == NULL) {
		ERR("ugd = NULL, so returning");
		return;
	}

#ifdef _TIZEN_LITE_CODE
	if (ugd->sim1_tapi_handle) {
		ret = tel_deinit(ugd->sim1_tapi_handle);
		if (ret != TAPI_API_SUCCESS) {
			ERR("tel_deinit SIM1 failed (%d)\n", ret);
		}
	}
	ugd->sim1_tapi_handle = NULL;

	if (ugd->sim2_tapi_handle) {
		ret = tel_deinit(ugd->sim2_tapi_handle);
		if (ret != TAPI_API_SUCCESS) {
			ERR("tel_deinit SIM2 failed (%d)\n", ret);
		}
	}
	ugd->sim2_tapi_handle = NULL;
#else
	if (ugd->tapi_handle) {
		ret = tel_deinit(ugd->tapi_handle);
		if (ret != TAPI_API_SUCCESS) {
			ERR("tel_deinit failed (%d)\n", ret);
		}
	}
	ugd->tapi_handle = NULL;
#endif /* _TIZEN_LITE_CODE */

	LEAVE();
}

#ifdef _TIZEN_LITE_CODE
void _cst_update_tapi_handle_by_simslot(void *data, CstSimSlot_t slot_id)
{
	ENTER(_cst_update_tapi_handle_by_simslot);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd == NULL) {
		ERR("ugd = NULL, so returning");
		return;
	}
	DBG("Slot_ID = %d", slot_id);

	switch (slot_id) {
	case CST_SELECTED_SIM1:
		ugd->tapi_handle = ugd->sim1_tapi_handle;
		break;
	case CST_SELECTED_SIM2:
		ugd->tapi_handle = ugd->sim2_tapi_handle;
		break;
	default:
		ugd->tapi_handle = ugd->sim1_tapi_handle; /*By default let SIM1 be the selected slot*/
		break;
	}

	LEAVE();
}

Eina_Bool _cst_is_sim_present_by_simslot(void *data, CstSimSlot_t slot_id)
{
	ENTER(_cst_is_sim_present_by_simslot);
	CstUgData_t *ugd = (CstUgData_t *)data;
	TapiResult_t tapi_err = TAPI_API_SUCCESS;
	TelSimCardStatus_t sim_status = 0;
	int sim_changed = 0;
	Eina_Bool ret = EINA_FALSE;

	if (ugd == NULL) {
		ERR("ugd = NULL, so returning");
		return ret;
	}
	DBG("Slot_ID = %d", slot_id);
	memset(&sim_status, 0, sizeof(sim_status));

	switch (slot_id) {
	case CST_SELECTED_SIM1:
		tapi_err = tel_get_sim_init_info(ugd->sim1_tapi_handle, &sim_status,
				&sim_changed);
		if (TAPI_API_SUCCESS != tapi_err) {
			ERR("tel_get_sim_init_info failed.. tapi_err = %d", tapi_err);
			return ret;
		}

		if ((sim_status == TAPI_SIM_STATUS_CARD_ERROR) ||
			(sim_status == TAPI_SIM_STATUS_CARD_NOT_PRESENT) ||
				(sim_status == TAPI_SIM_STATUS_CARD_REMOVED) ||
				(sim_status == TAPI_SIM_STATUS_CARD_CRASHED) ||
				(sim_status == TAPI_SIM_STATUS_CARD_POWEROFF) ||
				(sim_status == TAPI_SIM_STATUS_UNKNOWN)) {
			return ret;
		}

		ret = EINA_TRUE;
		break;
	case CST_SELECTED_SIM2:
		tapi_err = tel_get_sim_init_info(ugd->sim2_tapi_handle, &sim_status,
				&sim_changed);
		if (TAPI_API_SUCCESS != tapi_err) {
			ERR("tel_get_sim_init_info failed.. tapi_err = %d", tapi_err);
			return ret;
		}

		if ((sim_status == TAPI_SIM_STATUS_CARD_ERROR) ||
			(sim_status == TAPI_SIM_STATUS_CARD_NOT_PRESENT) ||
				(sim_status == TAPI_SIM_STATUS_CARD_REMOVED) ||
				(sim_status == TAPI_SIM_STATUS_CARD_CRASHED) ||
				(sim_status == TAPI_SIM_STATUS_CARD_POWEROFF) ||
				(sim_status == TAPI_SIM_STATUS_UNKNOWN)) {
			return ret;
		}

		ret = EINA_TRUE;
		break;
	default:
		break;
	}

	return ret;
}
#endif /* _TIZEN_LITE_CODE */

static int __cst_get_tapi_req_ss_class(int basic_service_code)
{
	int ss_class = 0;

	switch (basic_service_code) {
	case CST_CALLTYPE_VOICE:
		ss_class = TAPI_SS_CLASS_VOICE;
		break;
	case CST_CALLTYPE_VOICE_LINE2:
		ss_class = TAPI_SS_CLASS_AUX_VOICE;
		break;
	case CST_CALLTYPE_VIDEO:
		ss_class = TAPI_SS_CLASS_ALL_CS_SYNC;
		break;
	case CST_CALLTYPE_ALL:
		ss_class = TAPI_SS_CLASS_ALL_TELE_BEARER;
		break;
	default:
		ss_class = 0;
		ERR("Invalid Basic Service Code");
	}
	LEAVE();
	return ss_class;
}

static int __cst_get_tapi_cf_mode(int cst_action)
{
	int mode;

	switch (cst_action) {
	case CST_ACTION_ACTIVATE:
		mode = TAPI_SS_CF_MODE_ENABLE_EV;
		break;
	case CST_ACTION_REGISTER:
		mode = TAPI_SS_CF_MODE_REGISTRATION_EV;
		break;
	case CST_ACTION_DEACTIVATE:
		mode = TAPI_SS_CF_MODE_DISABLE_EV;
		break;
	default:
		mode = 0;
		ERR("Invalid CF mode");
	}

	return mode;
}

static int __cst_get_tapi_cf_flavour(int cst_cf_flavour)
{

	int tel_cf_flavour;
	switch (cst_cf_flavour) {
	case CST_SSTYPE_CF_UNCONDITIONAL:
		tel_cf_flavour = TAPI_SS_CF_WHEN_CFU;
		break;
	case CST_SSTYPE_CF_BUSY:
		tel_cf_flavour = TAPI_SS_CF_WHEN_CFB;
		break;
	case CST_SSTYPE_CF_NO_REPLY:
		tel_cf_flavour = TAPI_SS_CF_WHEN_CFNRy;
		break;
	case CST_SSTYPE_CF_NOT_REACHABLE:
		tel_cf_flavour = TAPI_SS_CF_WHEN_CFNRc;
		break;
	default:
		tel_cf_flavour = -1;
		ERR("Wrong CF flavour");
	}
	return tel_cf_flavour;
}

static int __cst_get_tapi_cw_mode(int cst_action)
{
	int mode;

	switch (cst_action) {
	case CST_ACTION_ACTIVATE:
		mode = TAPI_SS_CW_MODE_ACTIVATE;
		break;
	case CST_ACTION_DEACTIVATE:
		mode = TAPI_SS_CW_MODE_DEACTIVATE;
		break;
	default:
		mode = 0;
		ERR("Invalid CW action");
	}
	return mode;
}

static int __cst_ss_forward_get_call_type(TelSsClass_t cfcalltype)
{
	int ret = -1;

	switch (cfcalltype) {
	case TAPI_SS_CLASS_VOICE:
		ret = CST_CALLTYPE_VOICE;
		break;
	case TAPI_SS_CLASS_AUX_VOICE:
		ret = CST_CALLTYPE_VOICE_LINE2;
		break;
	case TAPI_SS_CLASS_ALL_SYNC:
	case TAPI_SS_CLASS_ALL_CS_SYNC:
	case TAPI_SS_CLASS_ALL_CS_ASYNC:	/* GER Voda, UK EE N/W */
		ret = CST_CALLTYPE_VIDEO;
		break;
	case TAPI_SS_CLASS_ALL_TELE_BEARER:
	case TAPI_SS_CLASS_ALL_BEARER:		/* GER Voda, UK EE, ITA H3G N/W */
		ret = CST_CALLTYPE_ALL;
		break;
	default:
		ERR("Invalid call type");
		break;
	}

	return ret;
}

static int __cst_ss_forward_get_flavour(TelSsForwardWhen_t cfcondition)
{
	int ret = -1;

	switch (cfcondition) {
	case TAPI_SS_CF_WHEN_CFU:
		ret = CST_SSTYPE_CF_UNCONDITIONAL;
		break;
	case TAPI_SS_CF_WHEN_CFB:
		ret = CST_SSTYPE_CF_BUSY;
		break;
	case TAPI_SS_CF_WHEN_CFNRy:
		ret = CST_SSTYPE_CF_NO_REPLY;
		break;
	case TAPI_SS_CF_WHEN_CFNRc:
		ret = CST_SSTYPE_CF_NOT_REACHABLE;
		break;
	case TAPI_SS_CF_WHEN_CF_ALL:
		ret = CST_SSTYPE_CF_ALL;
		break;
	case TAPI_SS_CF_WHEN_CFC:
		ret = CST_SSTYPE_CF_ALL_CONDITIONAL;
		break;
	default:
		ERR("Invalid CF SS_Code");
		ret = -1;
		break;
	}

	return ret;
}

static Eina_Bool __cst_ss_forward_get_status(TelSsStatus_t cfstatus)
{
	Eina_Bool ret = EINA_FALSE;

	switch (cfstatus) {
	case TAPI_SS_STATUS_ACTIVE:
	case TAPI_SS_STATUS_REGISTERED:
		ret = EINA_TRUE;
		break;
	case TAPI_SS_STATUS_NOTHING:
	case TAPI_SS_STATUS_PROVISIONED:
	case TAPI_SS_STATUS_QUIESCENT:
		ret = EINA_FALSE;
		break;
	default:
		ERR("Invalid CF SS_status");
		break;
	}

	return ret;
}

static gboolean __cst_ss_forward_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	DBG("Enter __cst_ss_forward_query_resp_cb");
	TelSsCause_t ss_result = result;
	TelSsForwardResp_t *cf_data = NULL;
	CallSettingSSReq_t *req = NULL;
	CstUgData_t *ugd = NULL;

	req = (CallSettingSSReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return FALSE;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return FALSE;
	}

	cf_data = (TelSsForwardResp_t *)data;
	if (cf_data == NULL) {
		ERR("data is NULL, returning");
		__cst_remove_ss_request(ugd);

		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}

		return FALSE;
	}

	retv_if(req->is_requesting == EINA_FALSE, 0);
	if (req->is_canceled == EINA_TRUE) {
		DBG("Req(0x%xp,req_id=%d) was canceled. So It will be removed", req, req->req_id);
		__cst_remove_ss_request(ugd);

		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}

		return FALSE;
	}

	if (ss_result != TAPI_SS_SUCCESS) {
		DBG("Event Status is %d", ss_result);
		int error;
		error = __cst_get_ss_error_from_tapi_error(ss_result);
		DBG("req=0x%p", req);
		if (req) {
			req->func(req->call_type, req->flavour, EINA_FALSE, NULL, error, req->action, req->data, req->waiting_time);
		}
		__cst_remove_ss_request(ugd);

		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}

		return FALSE;
	}

	char number[TAPI_CALL_DIALDIGIT_LEN_MAX];

	Eina_Bool cf_state = EINA_FALSE;
	int cf_flavour = -1;
	int call_type = -1;
	int waiting_time = 30;
	int i = 0;

	DBG("*** cf_data->record_num (%d) ***", cf_data->record_num);

	if ((cf_data->record_num > 1) && (req->call_type == CST_CALLTYPE_VIDEO)) {
		for (i = 0; i < cf_data->record_num; ++i) {
			if (cf_data->record[i].Class == TAPI_SS_CLASS_ALL_CS_SYNC) {
				DBG("TAPI_SS_CLASS_ALL_CS_SYNC result exists!!!");
				DBG("SS_Code=%d", cf_data->record[i].ForwardCondition);
				DBG("SS_Status=%d", cf_data->record[i].Status);
				number[0] = '\0';

				cf_flavour = __cst_ss_forward_get_flavour(cf_data->record[i].ForwardCondition);
				if (cf_flavour == CST_SSTYPE_CF_NO_REPLY) {
					waiting_time = (int)cf_data->record[i].NoReplyWaitTime;
					DBG("******** Waiting time =%d **********", waiting_time);
				}

				cf_state = __cst_ss_forward_get_status(cf_data->record[i].Status);

				if (cf_data->record[i].bCallForwardingNumberPresent == 1) {
					snprintf(number, TAPI_CALL_DIALDIGIT_LEN_MAX, "%s", cf_data->record[i].szCallForwardingNumber);
				}

				req->func(CST_CALLTYPE_VIDEO, cf_flavour, cf_state, number, CST_ERROR_NONE, req->action, req->data, waiting_time);

				__cst_remove_ss_request(ugd);

				if (cf_flavour == CST_SSTYPE_CF_NOT_REACHABLE) {
					if (ugd->popup) {
						evas_object_del(ugd->popup);
						ugd->popup = NULL;
					}
				}

				return TRUE;
			}
		}
	}

	for (i = 0; i < cf_data->record_num; ++i) {
		number[0] = '\0';
		DBG("TeleCommService=%d", cf_data->record[i].Class);
		DBG("SS_Code=%d", cf_data->record[i].ForwardCondition);
		DBG("SS_Status=%d", cf_data->record[i].Status);

		call_type = __cst_ss_forward_get_call_type(cf_data->record[i].Class);
		if (call_type == -1) {
			continue;
		}

		/* If the call type of the record is not same with request, skip below code */
		DBG("req->call_type == %d call_type=%d", req->call_type, call_type);
		if (req->call_type != call_type && call_type != CST_CALLTYPE_ALL) {
			continue;
		}

		cf_flavour = __cst_ss_forward_get_flavour(cf_data->record[i].ForwardCondition);
		if (cf_flavour == -1) {
			continue;
		} else if (cf_flavour == CST_SSTYPE_CF_NO_REPLY) {
			waiting_time = (int)cf_data->record[i].NoReplyWaitTime;
			DBG("******** Waiting time =%d **********", waiting_time);
		}

		cf_state = __cst_ss_forward_get_status(cf_data->record[i].Status);

		if (cf_data->record[i].bCallForwardingNumberPresent == 1) {
			snprintf(number, TAPI_CALL_DIALDIGIT_LEN_MAX, "%s", cf_data->record[i].szCallForwardingNumber);
		}

		req->func(call_type, cf_flavour, cf_state, number, CST_ERROR_NONE, req->action, req->data, waiting_time);
	}

	__cst_remove_ss_request(ugd);
	if (cf_flavour == CST_SSTYPE_CF_NOT_REACHABLE) {
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
	}
	return TRUE;
}

static gboolean __cst_ss_forward_set_resp_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	DBG("Enter __cst_ss_forward_set_resp_cb");
	TelSsCause_t ss_result = result;
	TelSsForwardInfo_t cf_info;
	TelSsForwardResp_t *cf_data = NULL;
	CallSettingSSReq_t *req = NULL;
	CstUgData_t *ugd = NULL;

	req = (CallSettingSSReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return FALSE;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return FALSE;
	}

	cf_data = (TelSsForwardResp_t *)data;
	if (cf_data == NULL) {
		ERR("data is NULL, returning");
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	retv_if(req->is_requesting == EINA_FALSE, 0);
	if (req->is_canceled == EINA_TRUE) {
		DBG("Req(0x%xp,req_id=%d) was canceled. So It will be removed", req, req->req_id);
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	DBG("ss_result (0x%x), req->action (%d), cf_data->record[0].Status (0x%x)", ss_result, req->action, cf_data->record[0].Status);

	if (ss_result != TAPI_SS_SUCCESS) {
		DBG("Event Status is %d", ss_result);
		int error;
		error = __cst_get_ss_error_from_tapi_error(ss_result);
		DBG("req=0x%p", req);
		DBG("req->original_state=%d", req->original_state);
		if (req) {
			req->func(req->call_type, req->flavour, req->original_state, NULL, error, req->action, req->data, req->waiting_time);
		}
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	memset(&cf_info, 0x0, sizeof(TelSsForwardInfo_t));
	cf_info.Condition = __cst_get_tapi_cf_flavour(req->flavour);
	cf_info.Class = __cst_get_tapi_req_ss_class(req->call_type);

	__cst_remove_ss_request(ugd);

	return TRUE;
}

static gboolean __cst_ss_waiting_set_resp_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	DBG("Enter __cst_ss_waiting_set_resp_cb");
	TelSsCause_t ss_result = result;
	TelSsWaitingResp_t *cw_data = NULL;
	CallSettingSSReq_t *req = NULL;
	TelSsWaitingInfo_t cw_info;
	CstUgData_t *ugd = NULL;

	req = (CallSettingSSReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return FALSE;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return FALSE;
	}

	cw_data = (TelSsWaitingResp_t *)data;
	if (cw_data == NULL) {
		ERR("data is NULL, returning");
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	retv_if(req->is_requesting == EINA_FALSE, 0);
	if (req->is_canceled == EINA_TRUE) {
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	if (ss_result != TAPI_SS_SUCCESS) {
		DBG("Event Status is %d", ss_result);
		int error;

		error = __cst_get_ss_error_from_tapi_error(ss_result);

		if (req) {
			req->func(req->call_type, req->flavour, EINA_TRUE, NULL, error, req->action, req->data, req->waiting_time);
		}
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	memset(&cw_info, 0x0, sizeof(TelSsWaitingInfo_t));
	cw_info.Class = __cst_get_tapi_req_ss_class(req->call_type);

	__cst_remove_ss_request(ugd);
	return TRUE;
}

static gboolean __cst_ss_waiting_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	DBG("Enter __cst_ss_waiting_query_resp_cb");
	TelSsCause_t ss_result = result;
	TelSsWaitingResp_t *cw_data = NULL;
	CallSettingSSReq_t *req = NULL;
	CstUgData_t *ugd = NULL;

	req = (CallSettingSSReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return FALSE;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return FALSE;
	}

	cw_data = (TelSsWaitingResp_t *)data;
	if (cw_data == NULL) {
		ERR("data is NULL, returning");
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	retv_if(req->is_requesting == EINA_FALSE, 0);
	if (req->is_canceled == EINA_TRUE) {
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	if (ss_result != TAPI_SS_SUCCESS) {
		DBG("Event Status is %d", ss_result);
		int error;

		error = __cst_get_ss_error_from_tapi_error(ss_result);

		if (req) {
			req->func(req->call_type, req->flavour, req->original_state, NULL, error, req->action, req->data, req->waiting_time);
		}
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	DBG("CW Status = %d", cw_data->record[0].Status);
	retv_if(NULL == req->func, 0);
	switch (cw_data->record[0].Status) {
	case TAPI_SS_STATUS_ACTIVE:
	case TAPI_SS_STATUS_REGISTERED:
		req->func(req->call_type, req->flavour, EINA_TRUE, NULL, CST_ERROR_NONE, req->action, req->data, req->waiting_time);
		break;
	case TAPI_SS_STATUS_PROVISIONED:
	case TAPI_SS_STATUS_QUIESCENT:
		req->func(req->call_type, req->flavour, EINA_FALSE, NULL, CST_ERROR_NONE, req->action, req->data, req->waiting_time);
		break;
	default:
		ERR("Call waiting query error");
		if (req) {
			req->func(req->call_type, req->flavour, EINA_TRUE, NULL, CST_ERROR_UNKNOWN, req->action, req->data, req->waiting_time);
		}
		break;
	}

	__cst_remove_ss_request(ugd);
	return TRUE;
}

static void __cst_print_req_queue(Eina_List *queue)
{
	Eina_List *l;
	CallSettingSSReq_t *req;
	EINA_LIST_FOREACH(queue, l, req) {
		ret_if(req == NULL);
		DBG("req=0x%p req_id=0x%x requesting=%d canceled=%d flavour=%d", req, req->req_id, req->is_requesting, req->is_canceled, req->flavour);
	}
}

void _cst_cancel_all_ss_request(void *data)
{
	ENTER(_cst_cancel_all_ss_request);
	CstUgData_t *ugd = (CstUgData_t *)data;

	Eina_List *l;
	Eina_List *l_next;
	CallSettingSSReq_t *req;

	EINA_LIST_FOREACH_SAFE(ugd->req_queue, l, l_next, req) {
		ret_if(req == NULL);
		DBG("Cancel req=0x%p", req);
		if (req->is_requesting == EINA_TRUE) {
			req->is_canceled = EINA_TRUE;
		} else {
			free(req);
			ugd->req_queue = eina_list_remove_list(ugd->req_queue, l);
		}
	}
	__cst_print_req_queue(ugd->req_queue);
	LEAVE();
}

void _cst_change_password_ss_request(CallSettingSSReq_t *req, void *func)
{
	ENTER(_cst_change_password_ss_request);
	ret_if(NULL == req);
	ret_if(NULL == func);
	int err = TAPI_API_SUCCESS;
	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	CstUgData_t *ugd = item_data->ugd;
	req->func = func;
	err = tel_change_ss_barring_password(ugd->tapi_handle,
			req->old_password,
			req->new_password,
			req->confirm_password,
			(tapi_response_cb)req->func, item_data);
	if (TAPI_API_SUCCESS != err) {
		ERR("tel_change_ss_barring_password returned failure %d", err);
	}
}

void _cst_add_ss_request(Eina_List **queue, int action_type, int call_type, int flavour, int orig_state, char *number, void *func, void *data, int waiting_time, CstUgData_t *ugd)
{
	ENTER(_cst_add_ss_request);
	ret_if(NULL == data);
	ret_if(NULL == func);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)malloc(sizeof(CallSettingSSReq_t));
	ret_if(NULL == req);
	req->action = action_type;
	req->original_state = orig_state;
	req->call_type = call_type;
	req->flavour = flavour;
	req->data = data;
	req->func = func;
	req->waiting_time = waiting_time;
	req->ugd = ugd;
	snprintf(req->number, CST_MAX_PHONE_NUMBER_LEN, "%s", number);
	int cnt;

	DBG("Add req=0x%p", req);
	req->is_canceled = EINA_FALSE;
	req->is_requesting = EINA_FALSE;

	*queue = eina_list_append(*queue, req);
	cnt = eina_list_count(*queue);
	DBG("req count=%d", cnt);
	if (cnt == 1) {
		__cst_send_ss_req_to_telephony_server(req);
	}
	LEAVE();
}

static void __cst_remove_ss_request(void *data)
{
	ENTER(__cst_remove_ss_request);
	CstUgData_t *ugd = data;
	retm_if(!ugd, "ugd is NULL");
	CallSettingSSReq_t *req;
	Eina_List *first;
	int cnt;
	ret_if(eina_list_count(ugd->req_queue) == 0);

	first = eina_list_nth_list(ugd->req_queue, 0);
	retm_if(!first, "first is NULL");
	req = (CallSettingSSReq_t *)first->data;
	DBG("Remove req=0x%p", req);
	ugd->req_queue = eina_list_remove_list(ugd->req_queue, first);
	free(req);

	cnt = eina_list_count(ugd->req_queue);
	DBG("req count=%d", cnt);

	if (cnt > 0) {
		first = eina_list_nth_list(ugd->req_queue, 0);
		retm_if(!first, "first is NULL");
		req = (CallSettingSSReq_t *)first->data;
		__cst_send_ss_req_to_telephony_server(req);
	}
	__cst_print_req_queue(ugd->req_queue);

	LEAVE();

}

static void __cst_send_ss_req_to_telephony_server(CallSettingSSReq_t *req)
{
	ENTER(__cst_send_ss_req_to_telephony_server);
	TelSsForwardInfo_t cf_info;
	TelSsBarringInfo_t cb_info;
	TelSsWaitingInfo_t cw_info;
	int api_ret = -1;
	int req_id = -1;
	CstUgData_t *ugd = req->ugd;

	ret_if(req == NULL);
	DBG("Send req=0x%p action=%d call_type=%d flavour=%d waiting_time=%d", req, req->action, req->call_type, req->flavour, req->waiting_time);
	req->is_requesting = EINA_TRUE;

	memset(&cf_info, 0x0, sizeof(TelSsForwardInfo_t));
	memset(&cb_info, 0x0, sizeof(TelSsBarringInfo_t));
	memset(&cw_info, 0x0, sizeof(TelSsWaitingInfo_t));

	switch (req->flavour) {
	case CST_SSTYPE_CF_UNCONDITIONAL:
	case CST_SSTYPE_CF_BUSY:
	case CST_SSTYPE_CF_NO_REPLY:
	case CST_SSTYPE_CF_NOT_REACHABLE:
	case CST_SSTYPE_CF_ALL:
	case CST_SSTYPE_CF_ALL_CONDITIONAL:
		cf_info.Condition = __cst_get_tapi_cf_flavour(req->flavour);
		cf_info.Class = __cst_get_tapi_req_ss_class(req->call_type);
		if (req->action == CST_ACTION_QUERY) {
			api_ret = tel_get_ss_forward_status(ugd->tapi_handle, cf_info.Class, cf_info.Condition, (tapi_response_cb)__cst_ss_forward_query_resp_cb, (void *)req);
		} else {
			cf_info.Mode = __cst_get_tapi_cf_mode(req->action);
			if ((req->flavour == CST_SSTYPE_CF_NO_REPLY) && (req->waiting_time > 0)) {
				cf_info.NoReplyConditionTimer = req->waiting_time;
			}
			snprintf((char *)cf_info.szPhoneNumber, TAPI_CALL_DIALDIGIT_LEN_MAX, "%s", req->number);
			if (cf_info.szPhoneNumber[0] == '+') {
				cf_info.Ton = TAPI_SS_CF_TON_INTERNATIONAL;
			} else {
				cf_info.Ton = TAPI_SS_CF_TON_NATIONAL;
			}
			DBG("Ton : %d", cf_info.Ton);
			api_ret = tel_set_ss_forward(ugd->tapi_handle, &cf_info, (tapi_response_cb)__cst_ss_forward_set_resp_cb, (void *)req);
		}
		break;
	case CST_SSTYPE_CW:
		cw_info.Class = __cst_get_tapi_req_ss_class(req->call_type);
		if (req->action == CST_ACTION_QUERY) {
			api_ret = tel_get_ss_waiting_status(ugd->tapi_handle, cw_info.Class, (tapi_response_cb)__cst_ss_waiting_query_resp_cb, (void *)req);
		} else {
			cw_info.Mode = __cst_get_tapi_cw_mode(req->action);
			api_ret = tel_set_ss_waiting(ugd->tapi_handle, &cw_info, (tapi_response_cb)__cst_ss_waiting_set_resp_cb, (void *)req);
		}
		break;
	}

	if (req->req_id != -1) {
		req->req_id = req_id;
	}

	if (api_ret != TAPI_API_SUCCESS) {
		CstGlItemData_t *item_data;
		CstUgData_t *ugd;

		req->func(req->call_type, req->flavour, EINA_FALSE, NULL, CST_ERROR_INCORRECT_OPERATION, req->action, req->data, req->waiting_time);
		item_data = (CstGlItemData_t *)req->data;
		ugd = (CstUgData_t *)item_data->ugd;
		__cst_remove_ss_request(ugd);
	}
	DBG("api_ret=%d req_id=0x%p", api_ret, req_id);

	LEAVE();
	return;
}

static int __cst_get_ss_error_from_tapi_error(TelSsCause_t tapi_err)
{
	ENTER(__cst_get_ss_error_from_tapi_error);

	int error_code = -1;

	DBG("Error Code =%d", tapi_err);

	switch (tapi_err) {
	case TAPI_SS_UNKNOWNSUBSCRIBER:
	case TAPI_SS_BEARERSERVICENOTPROVISIONED:
	case TAPI_SS_TELESERVICENOTPROVISIONED:
	case TAPI_SS_CALLBARRED:
	case TAPI_SS_ILLEGALSSOPERATION:
	case TAPI_SS_ERRORSTATUS:
	case TAPI_SS_FACILITYNOTSUPPORTED:
	case TAPI_SS_MAXNOMPTYEXCEEDED:
	case TAPI_SS_RESOURCESNOTAVAILABLE:
	case TAPI_SS_PWREGISTRATIONFAILURE:
	case TAPI_SS_SUBSCRIPTIONVIOLATION:
	case TAPI_SS_NOTAVAILABLE:
	case TAPI_SS_SYSTEMFAILURE:
	case TAPI_SS_REJECTEDBYNETWORK:
		error_code = CST_ERROR_SERVICE_UNAVAILABLE;
		break;
	case TAPI_SS_INCOMPATIBILITY:
	case TAPI_SS_DATAMISSING:
	case TAPI_SS_UNEXPECTEDDATAVALUE:
		error_code = CST_ERROR_INCORRECT_OPERATION;
		break;
	case TAPI_SS_NEGATIVEPWCHECK:
		error_code = CST_ERROR_INVALID_PASSWORD;
		break;
		/*Show message password error this function has been locked, so please call customer center for Vodafone R11 */
	case TAPI_SS_NUMBEROFPWATTEMPTSVIOLATION:
		error_code = CST_ERROR_PASSWORD_BLOCKED;
		break;
	case TAPI_SS_REJECTEDBYUSER:
		error_code = CST_ERROR_REJECTED_BY_NETWORK;
		break;
	case TAPI_SS_OEM_NOT_SUPPORTED:
		error_code = CST_ERROR_OEM_NOT_SUPPORTED;
		break;
	case TAPI_SS_FIXED_DIALING_NUMBER_ONLY:
		error_code = CST_ERROR_FDN_MODE_ACTIVE;
		break;
	default:
		DBG("__cst_get_ss_error_from_tapi_error:undefined =0x%x", tapi_err);
		error_code = CST_ERROR_UNKNOWN;
		break;
	}
	LEAVE();
	return error_code;
}
