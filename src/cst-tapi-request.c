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
#include "cst-fdn-list.h"
#include "cst-fdn-setting.h"

static void __cst_send_ss_req_to_telephony_server(CallSettingSSReq_t *req);
static int __cst_get_ss_error_from_tapi_error(TelSsCause_t tapi_err);
static gboolean __cst_ss_forward_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data);
static gboolean __cst_ss_barring_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data);
static gboolean __cst_ss_waiting_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_remove_ss_request(void *data);
#ifdef _CALL_SET_FDN_SUPPORT
static void __cst_on_sim_get_sim_lock_info_resp(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_sim_get_facility_resp(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_sim_facility_op_resp(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_sim_security_op_resp(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_pb_get_sim_pb_count(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_pb_get_sim_pb_meta_info(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_pb_read_sim_pb_record(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_pb_update_sim_pb_record(TapiHandle *handle, int result, void *data, void *user_data);
static void __cst_on_pb_delete_sim_pb_record(TapiHandle *handle, int result, void *data, void *user_data);
#endif
#ifdef _CALL_SET_BARRING_SUPPORT
static int __cst_ss_barring_get_call_type(TelSsClass_t cbcalltype);
#endif

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

static int __cst_get_tapi_cb_mode(int cst_action)
{
	int mode;

	switch (cst_action) {
	case CST_ACTION_ACTIVATE:
		mode = TAPI_SS_CB_MODE_ACTIVATE;
		break;
	case CST_ACTION_DEACTIVATE:
		mode = TAPI_SS_CB_MODE_DEACTIVATE;
		break;
	default:
		mode = 0;
		ERR("Invalid CB action");
	}
	return mode;
}

static int __cst_get_tapi_cb_flavour(int cst_cb_flavour)
{

	int tel_cb_flavour;
	switch (cst_cb_flavour) {
	case CST_SSTYPE_CB_OC:
		tel_cb_flavour = TAPI_SS_CB_TYPE_BAOC;
		break;
	case CST_SSTYPE_CB_OIC:
		tel_cb_flavour = TAPI_SS_CB_TYPE_BOIC;
		break;
	case CST_SSTYPE_CB_OICEH:
		tel_cb_flavour = TAPI_SS_CB_TYPE_BOIC_NOT_HC;
		break;
	case CST_SSTYPE_CB_IC:
		tel_cb_flavour = TAPI_SS_CB_TYPE_BAIC;
		break;
	case CST_SSTYPE_CB_ICR:
		tel_cb_flavour = TAPI_SS_CB_TYPE_BIC_ROAM;
		break;
	default:
		tel_cb_flavour = -1;
		ERR("Wrong CB flavour");
	}
	return tel_cb_flavour;
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

static gboolean __cst_ss_barring_set_resp_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	ENTER(__cst_ss_barring_set_resp_cb);
#ifdef _CALL_SET_BARRING_SUPPORT
	TelSsCause_t ss_result = result;
	TelSsBarringResp_t *cb_data = NULL;
	TelSsBarringInfo_t cb_info;
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

	cb_data = (TelSsBarringResp_t *)data;
	if (cb_data == NULL) {
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
			req->func(req->call_type, req->flavour, EINA_FALSE, NULL, error,
					req->action, req->data, req->waiting_time);
		}
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	memset(&cb_info, 0x0, sizeof(TelSsBarringInfo_t));
	cb_info.Type = __cst_get_tapi_cb_flavour(req->flavour);
	cb_info.Class = __cst_get_tapi_req_ss_class(req->call_type);

	__cst_remove_ss_request(ugd);

	return TRUE;
#else
	DBG("Call Barring Service is not supported in Generic version...");
	return TRUE;
#endif  /* _CALL_SET_BARRING_SUPPORT */

}

#ifdef _CALL_SET_BARRING_SUPPORT
static int __cst_ss_barring_get_call_type(TelSsClass_t cbcalltype)
{
	int ret = -1;

	switch (cbcalltype) {
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

static int __cst_ss_barring_get_flavour(TelSsBarringType_t cbcondition)
{
	int ret = -1;

	switch (cbcondition) {
	case TAPI_SS_CB_TYPE_BAOC:
		ret = CST_SSTYPE_CB_OC;
		break;
	case TAPI_SS_CB_TYPE_BOIC:
		ret = CST_SSTYPE_CB_OIC;
		break;
	case TAPI_SS_CB_TYPE_BOIC_NOT_HC:
		ret = CST_SSTYPE_CB_OICEH;
		break;
	case TAPI_SS_CB_TYPE_BAIC:
		ret = CST_SSTYPE_CB_IC;
		break;
	case TAPI_SS_CB_TYPE_BIC_ROAM:
		ret = CST_SSTYPE_CB_ICR;
		break;
	default:
		ERR("Invalid CB SS_Code");
		break;
	}

	return ret;
}

static Eina_Bool __cst_ss_barring_get_status(TelSsStatus_t cbstatus)
{
	Eina_Bool ret = EINA_FALSE;

	switch (cbstatus) {
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
#endif /* _CALL_SET_BARRING_SUPPORT */

static gboolean __cst_ss_barring_query_resp_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	ENTER(__cst_ss_barring_query_resp_cb);
#ifdef _CALL_SET_BARRING_SUPPORT
	TelSsCause_t ss_result = result;
	TelSsBarringResp_t *cb_data = NULL;
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

	cb_data = (TelSsBarringResp_t *)data;
	if (cb_data == NULL) {
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
			req->func(req->call_type, req->flavour, EINA_FALSE, NULL, error, req->action, req->data, req->waiting_time);
		}
		__cst_remove_ss_request(ugd);
		return FALSE;
	}

	Eina_Bool cb_state = EINA_FALSE;
	int cb_flavour = -1;
	int call_type = -1;
	int i = 0;

	if ((cb_data->record_num > 1) && (req->call_type == CST_CALLTYPE_VIDEO)) {
		for (i = 0; i < cb_data->record_num; ++i) {
			if (cb_data->record[i].Class == TAPI_SS_CLASS_ALL_CS_SYNC) {
				DBG("TAPI_SS_CLASS_ALL_CS_SYNC result exists!!!");
				DBG("SS_Code=%d", cb_data->record[i].Flavour);
				DBG("SS_Status=%d", cb_data->record[i].Status);

				cb_flavour = __cst_ss_barring_get_flavour(cb_data->record[i].Flavour);
				cb_state = __cst_ss_barring_get_status(cb_data->record[i].Status);

				req->func(CST_CALLTYPE_VIDEO, cb_flavour, cb_state, NULL, CST_ERROR_NONE, req->action, req->data, req->waiting_time);

				__cst_remove_ss_request(ugd);
				return TRUE;
			}
		}
	}

	for (i = 0; i < cb_data->record_num; ++i) {
		DBG("TeleCommService=0x%x", cb_data->record[i].Class);
		DBG("SS_Code=0x%x", cb_data->record[i].Flavour);
		DBG("SS_Status=0x%x", cb_data->record[i].Status);

		if (0 == cb_data->record[i].Class) {
			DBG("Because TeleCommService is 0, call_type is restored with local backup(%d)", req->call_type);
			call_type = req->call_type;
		} else {
			call_type = __cst_ss_barring_get_call_type(cb_data->record[i].Class);
		}

		if (call_type == -1) {
			continue;
		}

		/* If the call type of the record is not same with request, skip below code */
		if (req->call_type != call_type && call_type != CST_CALLTYPE_ALL) {
			continue;
		}

		cb_flavour = __cst_ss_barring_get_flavour(cb_data->record[i].Flavour);
		if (cb_flavour == -1) {
			continue;
		}

		cb_state = __cst_ss_barring_get_status(cb_data->record[i].Status);
	}

	req->func(call_type, cb_flavour, cb_state, NULL, CST_ERROR_NONE, req->action, req->data, req->waiting_time);
	__cst_remove_ss_request(ugd);
	return TRUE;
#else
	DBG("Call Barring Service is not supported in Generic version...");
	return TRUE;
#endif  /* _CALL_SET_BARRING_SUPPORT */
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
	case CST_SSTYPE_CB_OC:
	case CST_SSTYPE_CB_OIC:
	case CST_SSTYPE_CB_OICEH:
	case CST_SSTYPE_CB_IC:
	case CST_SSTYPE_CB_ICR:
		cb_info.Type = __cst_get_tapi_cb_flavour(req->flavour);
		cb_info.Class = __cst_get_tapi_req_ss_class(req->call_type);

		DBG("%d <= %d <= %d calltype", TAPI_SS_CLASS_VOICE, cb_info.Class, TAPI_SS_CLASS_ALL_TELE);
		DBG("%d <= %d <= %d type", TAPI_SS_CB_TYPE_AB, cb_info.Type, TAPI_SS_CB_TYPE_BIC_NOT_SIM);
		if (req->action == CST_ACTION_QUERY) {
			/*To do - add callback function*/
			api_ret = tel_get_ss_barring_status(ugd->tapi_handle, cb_info.Class, cb_info.Type, (tapi_response_cb)__cst_ss_barring_query_resp_cb, (void *)req);
		} else {
			cb_info.Mode = __cst_get_tapi_cb_mode(req->action);
			DBG("%d <= %d <= %d mode", TAPI_SS_CB_MODE_ACTIVATE, cb_info.Mode, TAPI_SS_CB_MODE_DEACTIVATE);
			DBG("%d == %d pwd length", strnlen(cb_info.szPassword, 4), TAPI_SS_GSM_BARR_PW_LEN_MAX);
			memcpy(cb_info.szPassword, req->number, TAPI_SS_GSM_BARR_PW_LEN_MAX);
			api_ret = tel_set_ss_barring(ugd->tapi_handle, &cb_info, (tapi_response_cb)__cst_ss_barring_set_resp_cb, (void *)req);
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

#ifdef _CALL_SET_FDN_SUPPORT
void _cst_add_sim_request(GSList **list, void *data)
{
	ENTER(_cst_add_sim_request);
	ret_if(NULL == data || !list);

	DBG("[Before append]sim req count=%d", g_slist_length(*list));

	*list = g_slist_append(*list, data);

	DBG("[After append]sim req count=%d", g_slist_length(*list));

	LEAVE();
	return;
}

void _cst_remove_sim_request(GSList **list, void *data)
{
	ENTER(__cst_remove_ss_request);
	ret_if(!list);
	ret_if(g_slist_length(*list) == 0);
	ret_if(!data);
	CallSettingSimReq_t *sim_req = NULL;

	sim_req = data;

	DBG("[Before remove]sim req count=%d", g_slist_length(*list));

	*list = g_slist_remove(*list, (gconstpointer)sim_req);

	if (sim_req->next_req_params != NULL) {
		if (sim_req->next_req_params->name != NULL) {
			g_free(sim_req->next_req_params->name);
			sim_req->next_req_params->name = NULL;
		}
		if (sim_req->next_req_params->number != NULL) {
			g_free(sim_req->next_req_params->number);
			sim_req->next_req_params->number = NULL;
		}
		if (sim_req->next_req_params->pin2 != NULL) {
			g_free(sim_req->next_req_params->pin2);
			sim_req->next_req_params->pin2 = NULL;
		}
		g_free(sim_req->next_req_params);
		sim_req->next_req_params = NULL;
	}
	g_free(sim_req);
	sim_req = NULL;

	DBG("[After remove]sim req count=%d", g_slist_length(*list));

	LEAVE();
	return;
}

static void __cst_on_sim_get_sim_lock_info_resp(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *sim_data = NULL;
	CstUgData_t *ugd = (CstUgData_t *)user_data;

	if (data == NULL) {
		ERR("TelSimLockInfo_t Data is NULL, returning");
		if (ugd->fdn_gl_item_data) {
			ugd->sim_lock_status = TAPI_SIM_LOCK_PERM_BLOCKED;
			ugd->fdn_status_update_func(ugd->fdn_gl_item_data);
		}
		return;
	}

	if (TAPI_SIM_PIN_OPERATION_SUCCESS != sec_rt) {
		ERR("sec_rt[%d]", sec_rt);
		if (ugd->fdn_gl_item_data) {
			ugd->sim_lock_status = TAPI_SIM_LOCK_PERM_BLOCKED;
			ugd->fdn_status_update_func(ugd->fdn_gl_item_data);
		}
		return;
	}
	sim_data = (TelSimLockInfo_t *)data;

	if (ugd == NULL) {
		ERR("ugd = NULL,  returning");
		return;
	}

	if (sim_data->lock_type == TAPI_SIM_LOCK_FD) {
		DBG("sim_data->lock_status[%d]-0:not need, 1:pin, 2:puk, 3:pin2, 4:puk2, 5:blocked", sim_data->lock_status);
		DBG("sim_data->retry_count[%d]", sim_data->retry_count);
		ugd->sim_lock_status = (int)sim_data->lock_status;
		ugd->sim_lock_retry_account = (int)sim_data->retry_count;
		if ((sim_data->lock_status == TAPI_SIM_LOCK_KEY_PUK2) ||
				(sim_data->lock_status == TAPI_SIM_LOCK_PERM_BLOCKED)) {
			if (ugd->fdn_gl_item_data) {
				ugd->fdn_status_update_func(ugd->fdn_gl_item_data);
			}
		}
	} else {
		DBG("sim_data->lock_type[%d]", sim_data->lock_type);
	}
}

void _cst_verify_pin2(void *data, CallSettingSimNxtReqParams_t *nxt_req_params, SimReqCb cb, void *cb_data)
{
	ENTER(_cst_verify_pin2);
	ret_if(!nxt_req_params);

	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimSecPw_t	sim_sec_pwd = {0,};

	SEC_DBG("pin2=%s; pin2 len = %d", nxt_req_params->pin2, nxt_req_params->pin2_len);
	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sim_req is NULL");
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_VERIFY_PIN2;
	sim_req->next_req_params =  g_malloc0(sizeof(CallSettingSimNxtReqParams_t));

	if (!sim_req->next_req_params) {
		ERR("sim_req->next_req_params is NULL");
		g_free(sim_req);
		return;
	}

	memcpy(sim_req->next_req_params, nxt_req_params, sizeof(CallSettingSimNxtReqParams_t));
	if (nxt_req_params->pin2) {
		sim_req->next_req_params->pin2 = (unsigned char *)g_strdup((gchar *)nxt_req_params->pin2);
	}
	if (nxt_req_params->name) {
		sim_req->next_req_params->name = (unsigned char *)g_strdup((gchar *)nxt_req_params->name);
	}
	if (nxt_req_params->number) {
		sim_req->next_req_params->number = (unsigned char *)g_strdup((gchar *)nxt_req_params->number);
	}

	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	sim_sec_pwd.pw = (unsigned char *)nxt_req_params->pin2;
	sim_sec_pwd.pw_len = nxt_req_params->pin2_len;
	sim_sec_pwd.type = TAPI_SIM_PTYPE_PIN2;
	ret_val = tel_verify_sim_pins(ugd->tapi_handle, &sim_sec_pwd, __cst_on_sim_security_op_resp, (void *)sim_req);

	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_verify_sim_pins failed. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}

#if 0	/* Do not delete this. Its for future use. */
void _cst_unblock_pin2(void *data, const char *puk2, const char *new_pin, SimReqCb cb, void *cb_data)
{
	ENTER(_cst_unblock_pin2);
	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimSecPw_t	puk2_sim_sec_pwd = {0,};
	TelSimSecPw_t	new_sim_sec_pwd = {0,};

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_UNBLOCK_PIN2;
	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	puk2_sim_sec_pwd.pw = (unsigned char *)puk2;
	puk2_sim_sec_pwd.pw_len = strlen(puk2);
	puk2_sim_sec_pwd.type = TAPI_SIM_PTYPE_PUK2;

	new_sim_sec_pwd.pw = (unsigned char *)new_pin;
	new_sim_sec_pwd.pw_len = strlen(new_pin);
	new_sim_sec_pwd.type = TAPI_SIM_PTYPE_PIN2;
	SEC_DBG("puk2 = %s; puk2_len = %d; new_pin = %s; new_pin_len = %d", puk2, puk2_sim_sec_pwd.pw_len, new_pin, new_sim_sec_pwd.pw_len);

	ret_val = tel_verify_sim_puks(ugd->tapi_handle, &puk2_sim_sec_pwd, &new_sim_sec_pwd, __cst_on_sim_security_op_resp, (void *)sim_req);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_verify_sim_puks. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}

void _cst_change_pins(void *data, const char *old_pin, const char *new_pin, SimReqCb cb, void *cb_data)
{
	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimSecPw_t	old_sim_sec_pwd = {0,};
	TelSimSecPw_t	new_sim_sec_pwd = {0,};

	ENTER(_cst_change_pins);
	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_CHANGE_PIN2;
	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	old_sim_sec_pwd.pw = (unsigned char *)old_pin;
	old_sim_sec_pwd.pw_len = strlen(old_pin);
	old_sim_sec_pwd.type = TAPI_SIM_PTYPE_PIN2;

	new_sim_sec_pwd.pw = (unsigned char *)new_pin;
	new_sim_sec_pwd.pw_len = strlen(new_pin);
	new_sim_sec_pwd.type = TAPI_SIM_PTYPE_PIN2;
	SEC_DBG("old_pin = %s; old_pin_len = %d; new_pin = %s; new_pin_len = %d", old_pin, old_sim_sec_pwd.pw_len, new_pin, new_sim_sec_pwd.pw_len);

	ret_val = tel_change_sim_pins(ugd->tapi_handle, &old_sim_sec_pwd, &new_sim_sec_pwd, __cst_on_sim_security_op_resp, (void *)sim_req);

	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_change_sim_pins. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}
#endif

void _cst_update_sim_fdn_status(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;
	if (ugd == NULL) {
		return;
	}
	int ret_val = TAPI_API_SUCCESS;

	ENTER(_cst_update_sim_fdn_status);
	ret_val = tel_get_sim_facility(ugd->tapi_handle, TAPI_SIM_LOCK_FD, __cst_on_sim_get_facility_resp, data);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_get_sim_facility for FDN status failed. err = %d !", ret_val);
	}
	LEAVE();
	return;
}

static void __cst_on_sim_get_facility_resp(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPinOperationResult_t sec_rt = result;
	TelSimFacilityInfo_t *sim_data = NULL;
	CstUgData_t *ugd = (CstUgData_t *)user_data;

	if (TAPI_SIM_PIN_OPERATION_SUCCESS != sec_rt) {
		ERR("Fatal: SIM Get Facility Operation Failed");
		if (ugd->fdn_gl_item_data) {
			ugd->fdn_status = CST_FDN_STATUS_FAIL;
			ugd->fdn_status_update_func(ugd->fdn_gl_item_data);
		}
		return;
	}

	if (data == NULL) {
		ERR("TelSimFacilityInfo_t Data is NULL, returning");
		if (ugd->fdn_gl_item_data) {
			ugd->fdn_status = CST_FDN_STATUS_FAIL;
			ugd->fdn_status_update_func(ugd->fdn_gl_item_data);
		}
		return;
	}
	sim_data = (TelSimFacilityInfo_t *)data;

	if (ugd == NULL) {
		DBG("ugd = NULL,  returning");
		return;
	}

	if (sim_data->type == TAPI_SIM_LOCK_FD) {
		DBG("sim_data->type[%d]", sim_data->type);
		DBG("sim_data->f_status[%d]", sim_data->f_status);
		ugd->fdn_status = (Eina_Bool)sim_data->f_status;
		if (ugd->fdn_gl_item_data) {
			ugd->fdn_status_update_func(ugd->fdn_gl_item_data);
		}
	} else {
		DBG("sim_data->type[%d]", sim_data->type);
	}
}

void _cst_set_sim_fdn_status(void *data, const unsigned char *pin2, int *pin2_len, SimReqCb cb, void *cb_data)
{
	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimFacilityPw_t fdn_data;

	ENTER(_cst_set_sim_fdn_status);

	if ((ugd == NULL) || (pin2 == NULL)) {
		ERR("ugd = NULL or pin2 = NULL, so returning");
		return;
	}

	memset(&fdn_data, 0, sizeof(fdn_data));
	fdn_data.lock_type = TAPI_SIM_LOCK_FD;
	fdn_data.pw_len = *pin2_len;
	fdn_data.pw = (unsigned char *)pin2;

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sim_req is NULL");
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_SET_FDN;
	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	SEC_DBG("pin2 = %s; pin2_len = %d", pin2, pin2_len);

	if (CST_FDN_STATUS_DISABLED == ugd->fdn_status) {
		ret_val = tel_enable_sim_facility(ugd->tapi_handle, &fdn_data, (tapi_response_cb)__cst_on_sim_facility_op_resp, (void *)sim_req);
	} else {
		ret_val = tel_disable_sim_facility(ugd->tapi_handle, &fdn_data, (tapi_response_cb)__cst_on_sim_facility_op_resp, (void *)sim_req);
	}

	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_enable_sim_facility / tel_disable_sim_facility for FDN failed. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}

static void __cst_on_sim_facility_op_resp(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPinOperationResult_t sec_rt = result;
	TelSimFacilityResult_t *sim_data = NULL;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;

	DBG("TAPI_SERVICE_SIM facility response received");

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		ERR("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	if (TAPI_SIM_PIN_OPERATION_SUCCESS != sec_rt) {
		ERR("Fatal: SIM Get Facility Operation Failed");
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	if (data == NULL) {
		ERR("TelSimFacilityResult_t Data is NULL, returning");
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}
	sim_data = (TelSimFacilityResult_t *)data;

	DBG("sim_data->type[%d]", sim_data->type);
	DBG("sim_data->retry_count[%d]", sim_data->retry_count);

	switch (sim_data->type) {
	case TAPI_SIM_LOCK_FD:
		DBG("sim_data->type = TAPI_SIM_LOCK_FD, for enable/disable FDN");
		if (ugd->fdn_status == CST_FDN_STATUS_DISABLED) {
			ugd->fdn_status = CST_FDN_STATUS_ENABLED;
		} else {
			ugd->fdn_status = CST_FDN_STATUS_DISABLED;
		}
		req->func(ugd, req, NULL);
		break;
	default:
		ERR("Invalid SimLock type[%d] received!!", sim_data->type);
		break;
	}

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

static void __cst_on_sim_security_op_resp(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPinOperationResult_t sec_rt = result;
	TelSimSecResult_t *sim_data = NULL;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;
	CallSettingSimResp_t	resp = {0, };

	DBG("TAPI_SERVICE_SIM security response received");

	DBG("sec_rt[%d]", sec_rt);

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		DBG("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	if (data == NULL) {
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		ERR("TelSimSecResult_t Data is NULL, returning");
		return;
	}
	sim_data = (TelSimSecResult_t *)data;
	DBG("sim_data->type[%d]", sim_data->type);
	DBG("sim_data->retry_count[%d]", sim_data->retry_count);

	switch (sim_data->type) {
	case TAPI_SIM_PTYPE_PIN2:
		if (req->req_id == TAPI_SIM_REQ_VERIFY_PIN2) {
			DBG("sim_data->type = TAPI_SIM_PTYPE_PIN2, for verifying PIN2");
#if 0	/* Do not delete this. Its for future use. */
		} else if (req->req_id == TAPI_SIM_REQ_CHANGE_PIN2) {
			DBG("sim_data->type = TAPI_SIM_PTYPE_PIN2, for changing the PIN2");
#endif
		} else {
			ERR("Wrong req->req_id = %d", req->req_id);
			break;
		}

		if (sec_rt == TAPI_SIM_PIN_OPERATION_SUCCESS) {
			resp.result = TAPI_SIM_OPERATION_SUCCESS;
		} else {
			resp.result = TAPI_SIM_OPERATION_FAILURE;
			resp.retry_count = sim_data->retry_count;
		}
		req->func(ugd, req, &resp);
		break;

#if 0	/* Do not delete this. Its for future use. */
	case TAPI_SIM_PTYPE_PUK2:
		DBG("sim_data->type = TAPI_SIM_PTYPE_PUK2, for verifying SIM PUKs");
		if (sec_rt == TAPI_SIM_PIN_OPERATION_SUCCESS) {
			resp.result = TAPI_SIM_OPERATION_SUCCESS;
		} else {
			resp.result = TAPI_SIM_OPERATION_FAILURE;
			resp.retry_count = sim_data->retry_count;
		}
		req->func(ugd, req, &resp);
		break;
#endif

	default:
		ERR("Invalid SimPin type[%d] received!!", sim_data->type);
		break;
	}

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

void _cst_get_fdn_phnbuk_metadata(void *data, SimReqCb cb, void *cb_data)
{
	ENTER(_cst_get_fdn_phnbuk_metadata);

	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimPbType_t pb_type = TAPI_SIM_PB_UNKNOWNN;

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sim_req is NULL");
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_GET_PHNBUK_METADATA;
	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;

	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	pb_type = TAPI_SIM_PB_FDN;
	ret_val = tel_get_sim_pb_meta_info(ugd->tapi_handle, pb_type, (tapi_response_cb)__cst_on_pb_get_sim_pb_meta_info, (void *)sim_req);

	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_get_sim_pb_meta_info. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}

void _cst_get_fdn_phnbuk_cnt(void *data, SimReqCb cb, void *cb_data)
{
	ENTER(_cst_get_fdn_phnbuk_cnt);

	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimPbType_t pb_type = TAPI_SIM_PB_UNKNOWNN;

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sim_req == NULL");
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_GET_PHNBUK_CNT;
	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	pb_type = TAPI_SIM_PB_FDN;
	ret_val = tel_get_sim_pb_count(ugd->tapi_handle, pb_type, (tapi_response_cb)__cst_on_pb_get_sim_pb_count, (void *)sim_req);

	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_get_sim_pb_count. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}

static void __cst_on_pb_get_sim_pb_count(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPbAccessResult_t access_rt = result;
	TelSimPbStorageInfo_t *sim_pb_data = NULL;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;
	CallSettingSimResp_t	resp = {0, };

	DBG("__cst_on_pb_get_sim_pb_count received");

	DBG("access_rt[%d]", access_rt);

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	if (data == NULL) {
		ERR("TelSimPbStorageInfo_t Data is NULL, returning");
		return;
	}
	sim_pb_data = (TelSimPbStorageInfo_t *)data;
	DBG("StorageFileType[%d] 0:fdn, 1:adn, 2:sdn, 3:3gsim, 4:aas, 5:gas", sim_pb_data->StorageFileType);
	DBG("TotalRecordCount[%d]", sim_pb_data->TotalRecordCount);
	DBG("UsedRecordCount[%d]", sim_pb_data->UsedRecordCount);

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		ERR("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		ERR("request is cancelled");
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	switch (sim_pb_data->StorageFileType) {
	case TAPI_SIM_PB_FDN:
		DBG("sim_pb_data->StorageFileType = TAPI_SIM_PB_FDN, which gets the storage count of the FDN");
		if (access_rt == TAPI_SIM_PB_SUCCESS) {
			resp.result = TAPI_SIM_OPERATION_SUCCESS;
			resp.phnbuk_used_rec_cnt = sim_pb_data->UsedRecordCount;
			resp.phnbuk_total_rec_cnt = sim_pb_data->TotalRecordCount;
		} else {
			resp.result = TAPI_SIM_OPERATION_FAILURE;
		}
		req->func(ugd, req, &resp);
		break;
	default:
		ERR("Invalid SimPhoneBook type[%d] received!!", sim_pb_data->StorageFileType);
		break;
	}

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

static void __cst_on_pb_get_sim_pb_meta_info(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPbAccessResult_t access_rt = result;
	TelSimPbEntryInfo_t *sim_pb_data = NULL;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;
	CallSettingSimResp_t	resp = {0, };

	DBG("access_rt[%d]", access_rt);

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	if (data == NULL) {
		ERR("TelSimPbEntryInfo_t Data is NULL, returning");
		return;
	}
	sim_pb_data = (TelSimPbEntryInfo_t *)data;
	DBG("StorageFileType[%d] 0:fdn, 1:adn, 2:sdn, 3:3gsim, 4:aas, 5:gas", sim_pb_data->StorageFileType);
	DBG("PbIndexMin[%d]", sim_pb_data->PbIndexMin);
	DBG("PbIndexMax[%d]", sim_pb_data->PbIndexMax);
	DBG("PbNumLenMax[%d]", sim_pb_data->PbNumLenMax);
	DBG("PbTextLenMax[%d]", sim_pb_data->PbTextLenMax);

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		ERR("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	ugd->fdn_contact_number_len_max = sim_pb_data->PbNumLenMax;
	ugd->fdn_contact_name_len_max = sim_pb_data->PbTextLenMax;

	switch (sim_pb_data->StorageFileType) {
	case TAPI_SIM_PB_FDN:
		if (access_rt == TAPI_SIM_PB_SUCCESS) {
			resp.result = TAPI_SIM_OPERATION_SUCCESS;
		} else {
			resp.result = TAPI_SIM_OPERATION_FAILURE;
		}
		req->func(ugd, req, &resp);
		break;
	default:
		ERR("Invalid StorageFileType type[%d] received!!", sim_pb_data->StorageFileType);
		break;
	}

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

static void __cst_on_pb_read_sim_pb_record(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPbAccessResult_t access_rt = result;
	TelSimPbRecord_t *sim_pb_rec = NULL;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;
	CallSettingSimResp_t	resp = {0, };

	DBG("__cst_on_pb_read_sim_pb_record received");

	DBG("access_rt[%d]", access_rt);

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	if (data == NULL) {
		ERR("TelSimPbRecord_t Data is NULL, returning");
		return;
	}
	sim_pb_rec = (TelSimPbRecord_t *)data;
	DBG("phonebook_type[%d] 0:fdn, 1:adn, 2:sdn, 3:3gsim, 4:aas, 5:gas", sim_pb_rec->phonebook_type);
	DBG("index[%d]", sim_pb_rec->index);
	DBG("next_index[%d]", sim_pb_rec->next_index);
	SEC_DBG("name[%s]", sim_pb_rec->name);
	DBG("dcs[%d]", sim_pb_rec->dcs);
	SEC_DBG("number[%s]", sim_pb_rec->number);
	DBG("ton[%d]", sim_pb_rec->ton);

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		ERR("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		ERR("Request is cancelled");
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	switch (sim_pb_rec->phonebook_type) {
	case TAPI_SIM_PB_FDN:
		if (access_rt == TAPI_SIM_PB_SUCCESS) {
			resp.result = TAPI_SIM_OPERATION_SUCCESS;
			resp.fdn_rec.index = sim_pb_rec->index;
			resp.fdn_rec.next_index = sim_pb_rec->next_index;
			resp.fdn_rec.name = sim_pb_rec->name;
			resp.fdn_rec.number = sim_pb_rec->number;
			resp.fdn_rec.ton = sim_pb_rec->ton;
		} else {
			resp.result = TAPI_SIM_OPERATION_FAILURE;
		}
		req->func(ugd, req, &resp);
		break;
	default:
		ERR("Invalid TelSimPbType_t type[%d] received!!", sim_pb_rec->phonebook_type);
		break;
	}

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

static void __cst_on_pb_update_sim_pb_record(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPbAccessResult_t access_rt = result;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;
	CallSettingSimResp_t	resp = {0, };

	DBG("access_rt[%d]", access_rt);

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		ERR("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		ERR("Request is cancelled");
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	if (access_rt == TAPI_SIM_PB_SUCCESS) {
		resp.result = TAPI_SIM_OPERATION_SUCCESS;
	} else if (access_rt == TAPI_SIM_PB_FAIL) {
		resp.result = TAPI_SIM_OPERATION_FAILURE;
	} else if (access_rt == TAPI_SIM_PB_INVALID_INDEX) {
		resp.result = TAPI_SIM_OPERATION_INVALID_INDEX;
	} else if (access_rt == TAPI_SIM_PB_INVALID_NUMBER_LENGTH) {
		resp.result = TAPI_SIM_OPERATION_INVALID_NUMBER_LENGTH;
	} else if (access_rt == TAPI_SIM_PB_INVALID_NAME_LENGTH) {
		resp.result = TAPI_SIM_OPERATION_INVALID_NAME_LENGTH;
	} else if (access_rt == TAPI_SIM_PB_ACCESS_CONDITION_NOT_SATISFIED) {
		resp.result = TAPI_SIM_OPERATION_ACCESS_CONDITION_NOT_SATISFIED;
	} else {
		resp.result = TAPI_SIM_OPERATION_FAILURE;
	}
	req->func(ugd, req, &resp);

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

static void __cst_on_pb_delete_sim_pb_record(TapiHandle *handle, int result, void *data, void *user_data)
{
	TelSimPbAccessResult_t access_rt = result;
	CallSettingSimReq_t	*req = NULL;
	CstUgData_t *ugd = NULL;
	CallSettingSimResp_t	resp = {0, };
	CallSettingSimNxtReqParams_t *index_params = NULL;

	DBG("access_rt[%d]", access_rt);

	req = (CallSettingSimReq_t *)user_data;
	if (req == NULL) {
		ERR("User_data is NULL, returning");
		return;
	}

	ugd = (CstUgData_t *)req->ugd;
	if (ugd == NULL) {
		ERR("ugd is NULL, returning");
		return;
	}

	if (ugd->sim_req_list == NULL) {
		ERR("ugd->sim_req_list == NULL");
		return;
	}

	if (req->req_state == SIM_REQUEST_CANCELLED) {
		_cst_remove_sim_request(&(ugd->sim_req_list), req);
		return;
	}

	index_params = req->next_req_params;
	if (index_params == NULL) {
		ERR("index_params is NULL, returning");
		return;
	}

	if (access_rt == TAPI_SIM_PB_SUCCESS) {
		DBG("Index deleted = [%d]", index_params->index);
		resp.result = TAPI_SIM_OPERATION_SUCCESS;
		resp.fdn_rec.index = index_params->index;
	} else {
		resp.result = TAPI_SIM_OPERATION_FAILURE;
	}
	req->func(ugd, req, &resp);

	_cst_remove_sim_request(&(ugd->sim_req_list), req);
	return;
}

void _cst_get_sim_lock_info(void *data)
{
	ENTER(_cst_get_sim_lock_info);
	CstUgData_t *ugd = (CstUgData_t *)data;
	if (ugd == NULL) {
		return;
	}
	int ret_val = TAPI_API_SUCCESS;

	ret_val = tel_get_sim_lock_info(ugd->tapi_handle, TAPI_SIM_LOCK_FD, __cst_on_sim_get_sim_lock_info_resp, data);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_get_sim_lock_info for FDN status failed. err = %d !", ret_val);
	}
	LEAVE();
	return;
}

Eina_Bool _cst_is_sim_phnbuk_ready(void *data, unsigned short *first_index)
{
	ENTER(_cst_is_sim_phnbuk_ready);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int ret_val = TAPI_API_SUCCESS;
	int init_completed = 0;
	TelSimPbList_t pb_list = {0, };
	Eina_Bool fdn_phn_buk_status = FALSE;

	if (ugd == NULL) {
		ERR("ugd = NULL, so returning");
		return fdn_phn_buk_status;
	}

	ret_val = tel_get_sim_pb_init_info(ugd->tapi_handle, &init_completed, &pb_list);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_get_sim_pb_init_info failed. err = %d !", ret_val);
	} else {
		DBG("tel_get_sim_pb_init_info success. init_completed = %d; pb_list.b_fdn = %d; first_index = %d", init_completed, pb_list.b_fdn, *first_index);
		if (1 == init_completed && pb_list.b_fdn == TRUE) {
			DBG("Able to access FDN phone book");
			fdn_phn_buk_status = TRUE;
		}
	}
	LEAVE();
	return fdn_phn_buk_status;
}

void _cst_get_phnbuk_record(void *data, unsigned short index, SimReqCb cb, void *cb_data)
{
	ENTER(_cst_get_phnbuk_record);

	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimPbType_t pb_type = TAPI_SIM_PB_UNKNOWNN;

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sim_req is NULL");
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_GET_PHNBUK_REC;
	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	pb_type = TAPI_SIM_PB_FDN;
	ret_val = tel_read_sim_pb_record(ugd->tapi_handle, pb_type, index, (tapi_response_cb)__cst_on_pb_read_sim_pb_record, (void *)sim_req);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_read_sim_pb_record. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}

void _cst_update_phnbuk_record(void *data, bool is_add_req, CallSettingFDNRec_t *fdn_rec, SimReqCb cb)
{
	ENTER(_cst_update_phnbuk_record);

	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimPbRecord_t contact_record;

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sime_req is NULL");
	sim_req->func = cb;
	sim_req->data = fdn_rec;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	if (TRUE == is_add_req) {
		sim_req->req_id = TAPI_SIM_REQ_ADD_PHNBUK_REC;
	} else {
		sim_req->req_id = TAPI_SIM_REQ_UPDATE_PHNBUK_REC;
	}

	sim_req->next_req_params =  NULL;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	memset(&contact_record, 0, sizeof(contact_record));
	contact_record.phonebook_type = TAPI_SIM_PB_FDN;
	contact_record.index = fdn_rec->index;
	contact_record.next_index = fdn_rec->next_index;
	contact_record.dcs = TAPI_SIM_TEXT_ENC_GSM7BIT;
	g_strlcpy((gchar *)contact_record.name, (gchar *)fdn_rec->name, sizeof(contact_record.name));
	g_strlcpy((gchar *)contact_record.number, (gchar *)fdn_rec->number, sizeof(contact_record.number));

	if (contact_record.number[0] == '+')
		contact_record.ton = TAPI_SIM_TON_INTERNATIONAL;
	else
		contact_record.ton = TAPI_SIM_TON_NATIONAL;

	ret_val = tel_update_sim_pb_record(ugd->tapi_handle, &contact_record, __cst_on_pb_update_sim_pb_record, (void *)sim_req);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_update_sim_pb_record. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
		g_free(fdn_rec);
	}
	LEAVE();
	return;
}

void _cst_delete_phnbuk_record(void *data, unsigned short index, SimReqCb cb, void *cb_data)
{
	ENTER(_cst_delete_phnbuk_record);

	int ret_val = TAPI_API_SUCCESS;
	CstUgData_t *ugd = (CstUgData_t *)data;
	CallSettingSimReq_t *sim_req = NULL;
	TelSimPbType_t pb_type = TAPI_SIM_PB_UNKNOWNN;

	sim_req = g_malloc0(sizeof(CallSettingSimReq_t));
	retm_if(!sim_req, "sim_req is NULL");
	sim_req->func = cb;
	sim_req->data = cb_data;
	sim_req->req_state = SIM_REQUEST_WAITING_FOR_RESP;
	sim_req->req_id = TAPI_SIM_REQ_DELETE_PHNBUK_REC;
	sim_req->next_req_params =  g_malloc0(sizeof(CallSettingSimNxtReqParams_t));

	if (!sim_req->next_req_params) {
		ERR("sim_req->next_req_params is NULL");
		g_free(sim_req);
		return;
	}

	sim_req->next_req_params->index = index;
	sim_req->ugd = data;
	_cst_add_sim_request(&(ugd->sim_req_list), sim_req);

	DBG("Request to delete index [%d]", index);
	pb_type = TAPI_SIM_PB_FDN;
	ret_val = tel_delete_sim_pb_record(ugd->tapi_handle, pb_type, index, (tapi_response_cb)__cst_on_pb_delete_sim_pb_record, (void *)sim_req);
	if (TAPI_API_SUCCESS != ret_val) {
		ERR("tel_delete_sim_pb_record. err = %d !", ret_val);
		_cst_remove_sim_request(&(ugd->sim_req_list), sim_req);
	}
	LEAVE();
	return;
}
#endif  /* _CALL_SET_FDN_SUPPORT */
