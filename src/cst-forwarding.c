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

#include <Elementary.h>
#include <tapi_event.h>
#include <tapi_common.h>

#include "cst-common-string.h"
#include "cst-call-setting.h"

#include "cst-forwarding.h"
#include "cst-forwarding-ime.h"
#include "cst-widget.h"
#include "cst-common.h"
#include "cst-tapi-request.h"
#include "cst-util.h"

enum {
	CST_CF_ALWAYS_SEL = 0,
	CST_CF_IF_BUSY_SEL,
	CST_CF_IF_NO_REPLY_SEL,
	CST_CF_IF_OUT_OF_REACH_SEL,
	CST_CF_FORWARD_TO_TEXT,
	CST_CF_WAITING_TIME_SEL,

	CST_CF_NONE,
};

typedef struct {
	Elm_Genlist_Item_Class *itc_2text_1icon;
	Elm_Genlist_Item_Class *itc_1text_1icon;
	Elm_Genlist_Item_Class *itc_forward_to;
	Elm_Genlist_Item_Class *itc_waiting_time;
	Elm_Genlist_Item_Class *itc_waiting_time_expand;
	Evas_Object * ec_item[3];
	CstGlItemData_t *cf_sel_item;
	CstGlItemData_t *forwarding_item;
	CstGlItemData_t *forwardto_item;
	CstGlItemData_t *waitingtime_item;
	Evas_Object *waitingtime_radio_btn;
	int cf_wait_time_index;
	int cf_wait_time;
	Ecore_Timer *tapi_notify_timer;
} CstCallForward_info_t;

static void __cst_gl_on_click_waitingtime_expand(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_gl_collapse_req(void *data, Evas_Object *obj, void *event_info);
static void __cst_gl_collapse(int selected_time, Evas_Object *obj, void *event_info);
static void __cst_gl_expand_req(void *data, Evas_Object *obj, void *event_info);
static void __cst_gl_expand(void *data, Evas_Object *obj, void *event_info);
static void __cst_gl_sel_waiting_time(void *data, Evas_Object *obj, void *event_info);
static void __cst_update_wait_time_and_index(int wait_time_index, int wait_time);
static void __cst_gl_update_waiting_time_ss_request(void);
static void __cst_on_click_cf_onoff_forward_to(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_cf_destroy_genlist_item_class(void);
static Eina_Bool __cst_on_click_cf_voice_video_back_button(void *data, Elm_Object_Item *it);

static CstCallForward_info_t cst_forward_data = {0, };

static gboolean b_notify_cb_enabled = EINA_TRUE;

static CstGlItemData_t *cf_item_data[4];
static CstGlItemData_t *g_item_data = NULL;

static CstGlItemDisplayInfo_t list_call_forwarding[] = {
	{1, CST_STR_ALWAYS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_2TEXT_ONOFF_PROGRESS, __cst_on_click_cf_onoff_forward_to},
	{1, CST_STR_IF_BUSY, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_2TEXT_ONOFF_PROGRESS, __cst_on_click_cf_onoff_forward_to},
	{1, CST_STR_IF_NO_REPLY, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_2TEXT_ONOFF_PROGRESS, __cst_on_click_cf_onoff_forward_to},
	{1, CST_STR_IF_OUT_OF_REACH, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_2TEXT_ONOFF_PROGRESS, __cst_on_click_cf_onoff_forward_to},

	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static CstGlItemDisplayInfo_t list_call_forwarding_dep2_waiting_time[] = {
	{2, CST_CF_WAIT_TIME_5_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_5_SEC},
	{2, CST_CF_WAIT_TIME_10_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_10_SEC},
	{2, CST_CF_WAIT_TIME_15_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_15_SEC},
	{2, CST_CF_WAIT_TIME_20_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_20_SEC},
	{2, CST_CF_WAIT_TIME_25_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_25_SEC},
	{2, CST_CF_WAIT_TIME_30_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_30_SEC},

	{2, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static int keypad_enabled = 0;

static void __cst_cf_destroy_genlist_item_class(void)
{
	ENTER(__cst_cf_destroy_genlist_item_class);
	DBG(" *** Testing: CF screen *** ");

	if (cst_forward_data.itc_2text_1icon) {
		_cst_destroy_genlist_item_class(cst_forward_data.itc_2text_1icon);
		cst_forward_data.itc_2text_1icon = NULL;
	}
	if (cst_forward_data.itc_1text_1icon) {
		_cst_destroy_genlist_item_class(cst_forward_data.itc_1text_1icon);
		cst_forward_data.itc_1text_1icon = NULL;
	}
	if (cst_forward_data.itc_forward_to) {
		_cst_destroy_genlist_item_class(cst_forward_data.itc_forward_to);
		cst_forward_data.itc_forward_to = NULL;
	}
	if (cst_forward_data.itc_waiting_time) {
		_cst_destroy_genlist_item_class(cst_forward_data.itc_waiting_time);
		cst_forward_data.itc_waiting_time = NULL;
	}
	if (cst_forward_data.itc_waiting_time_expand) {
		_cst_destroy_genlist_item_class(cst_forward_data.itc_waiting_time_expand);
		cst_forward_data.itc_waiting_time_expand = NULL;
	}
}

static Eina_Bool __cst_on_click_cf_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_cf_back_button);

	__cst_on_click_cf_voice_video_back_button(data, NULL);
	__cst_cf_destroy_genlist_item_class();

	return EINA_TRUE;
}

static Eina_Bool __cst_on_click_cf_voice_video_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_cf_voice_video_back_button);

	retv_if(data == NULL, EINA_TRUE);

	CstAppData_t *ad = (CstAppData_t *)data;

	ad->back_button = _cst_get_navifr_prev_btn(ad->nf);
	if (cst_forward_data.tapi_notify_timer) {
		ecore_timer_del(cst_forward_data.tapi_notify_timer);
		cst_forward_data.tapi_notify_timer = NULL;
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	tel_deregister_noti_event(ad->tapi_handle, TAPI_NOTI_SS_FORWARD_STATUS);
	_cst_cancel_all_ss_request(ad);

	return EINA_TRUE;
}

static Eina_Bool __cst_is_exist_cf_ss_request_pending(CstAppData_t *ad)
{
	retv_if(NULL == ad, EINA_FALSE);
	int  i = 0;

	for (i = 0; i < 4; i++) {
		if (ad->cf_state[i] == CST_SS_STATE_PROGRESS)
			return EINA_TRUE;
	}

	return EINA_FALSE;
}

static void __cst_disable_cf_options(CstGlItemData_t *item_data)
{
	ret_if(NULL == item_data);
	CstAppData_t *ad = item_data->ad;
	Elm_Object_Item *next_item;
	Elm_Object_Item *prev_item;
	int  i = 0;

	/*Cancel any pending requests*/
	_cst_cancel_all_ss_request(ad);

	/*Disable the CF options*/
	for (i = 0; i < 4; i++) {
		ad->cf_state[i] = CST_SS_STATE_OFF;
	}
	next_item = prev_item = item_data->gl_item;
	while (NULL != next_item) {
		elm_object_item_disabled_set(next_item, EINA_TRUE);
		next_item = elm_genlist_item_next_get(next_item);
	}
	while (NULL != prev_item) {
		elm_object_item_disabled_set(prev_item, EINA_TRUE);
		prev_item = elm_genlist_item_prev_get(prev_item);
	}
}

static void __cst_update_cf_state(int call_type, int cf_flavour,
								  Eina_Bool cf_state, char *number,
								  int error, int req_action, void *data, int waiting_time)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstAppData_t *ad = item_data->ad;
	ret_if((call_type != ad->call_type && call_type != CST_CALLTYPE_ALL));
	DBG("call_type=%d (0:voice 1:video)", call_type);
	DBG("cf_flavour=%d item_data->index=%d(0:CFU 1:CFB 2:CFNR 3:CFOR)", cf_flavour, item_data->index);
	DBG("cf_state=%d (1:on 0:off)", cf_state);
	DBG("number=%s, error=%d", number, error);
	DBG("req_action=%d(0: Act, 1:Deact, 2:Register, 3:Query)", req_action);
	DBG("waiting_time=%d", waiting_time);

	if (cf_flavour == CST_SSTYPE_CF_NO_REPLY &&
			(waiting_time >= 5)) { /* If no reply then update the Waiting time */
		cst_forward_data.cf_wait_time = waiting_time;
	}

	if (error == CST_ERROR_NONE) {
		if ((item_data->cf_cb_request_by == CST_CF_CB_INTERROGATE_BY_ACTIVATE) && (cf_state == EINA_FALSE)) {
			_cst_create_error_popup(CST_ERROR_OEM_NOT_SUPPORTED);
		} else if ((item_data->cf_cb_request_by == CST_CF_CB_INTERROGATE_BY_DEACTIVATE) && (cf_state == EINA_TRUE)) {
			int errormsg = CST_ERROR_OEM_NOT_SUPPORTED;

			switch (cf_flavour) {
			case CST_SSTYPE_CF_BUSY:
				errormsg = CST_ERROR_OEM_NOT_SUPPORTED_USER_BUSY;
				break;
			case CST_SSTYPE_CF_NO_REPLY:
				errormsg = CST_ERROR_OEM_NOT_SUPPORTED_NO_REPLY;
				break;
			case CST_SSTYPE_CF_NOT_REACHABLE:
				errormsg = CST_ERROR_OEM_NOT_SUPPORTED_OUT_OF_REACH;
				break;
			default:
				DBG("cf_flavour = %d", cf_flavour);
				break;
			}
			_cst_create_error_popup(errormsg);
		}

		if (cf_state == EINA_TRUE) {
			ad->cf_state[item_data->index] = CST_SS_STATE_ON;
			if (strlen(number) > 0) {
				snprintf(item_data->number, sizeof(item_data->number), "%s", number);
			} else {
				snprintf(item_data->number, sizeof(item_data->number), "%s", T_(CST_STR_UNKNOWN));
			}
			_cst_update_genlist_item_class(item_data->gl_item, cst_forward_data.itc_2text_1icon);
		} else {
			if (strlen(number) > 0) {
				snprintf(item_data->number, sizeof(item_data->number), "%s", number);
			} else {
				strncpy(item_data->number, "", sizeof(item_data->number));
			}
			ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
			_cst_update_genlist_item_class(item_data->gl_item, cst_forward_data.itc_1text_1icon);
		}
	} else {
		if (error == CST_ERROR_FDN_MODE_ACTIVE) {
			__cst_disable_cf_options(item_data);
			if (ad->popup) {
				evas_object_del(ad->popup);
				ad->popup = NULL;
			}
			ad->popup = (Evas_Object *)_cst_create_error_popup_with_ok_btn(ad,
					error);
			evas_object_show(ad->popup);
			return;
		}

		if (req_action != CST_ACTION_QUERY) {
			if (error != CST_ERROR_SERVICE_UNAVAILABLE &&
					error != CST_ERROR_REJECTED_BY_NETWORK &&
					error != CST_ERROR_OEM_NOT_SUPPORTED &&
					error != CST_ERROR_UNKNOWN) {
				DBG("User error : %d(Restore previous state)", error);
				if (req_action == CST_ACTION_DEACTIVATE) {
					ad->cf_state[item_data->index] = CST_SS_STATE_ON;
				} else if (req_action == CST_ACTION_REGISTER) {
					if (cf_state == EINA_TRUE)
						ad->cf_state[item_data->index] = CST_SS_STATE_ON;
					else if (cf_state == EINA_FALSE)
						ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
				} else {
					ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
				}
			} else {
				DBG("Network error : %d", error);
				if (req_action == CST_ACTION_DEACTIVATE) {
					ad->cf_state[item_data->index] = CST_SS_STATE_ON;
				} else if (req_action == CST_ACTION_REGISTER) {
					if (cf_state == EINA_TRUE)
						ad->cf_state[item_data->index] = CST_SS_STATE_ON;
					else if (cf_state == EINA_FALSE)
						ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
				} else {
					ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
				}
				if (error == CST_ERROR_OEM_NOT_SUPPORTED) {
					switch (cf_flavour) {
						case CST_SSTYPE_CF_BUSY:
							error = CST_ERROR_OEM_NOT_SUPPORTED_USER_BUSY;
							break;
						case CST_SSTYPE_CF_NO_REPLY:
							error = CST_ERROR_OEM_NOT_SUPPORTED_NO_REPLY;
							break;
						case CST_SSTYPE_CF_NOT_REACHABLE:
							error = CST_ERROR_OEM_NOT_SUPPORTED_OUT_OF_REACH;
							break;
						default:
							DBG("cf_flavour = %d", cf_flavour);
					}
				}
			}

 		} else {
			ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
		}

		if (ad->cf_state[item_data->index] == CST_SS_STATE_ON) {
			_cst_update_genlist_item_class(item_data->gl_item, cst_forward_data.itc_2text_1icon);
		} else {
			_cst_update_genlist_item_class(item_data->gl_item, cst_forward_data.itc_1text_1icon);
		}
	}

	if (__cst_is_exist_cf_ss_request_pending(ad) == EINA_FALSE) {
		Elm_Object_Item *next_item;
		Elm_Object_Item *prev_item;
		next_item = prev_item = item_data->gl_item;

		if (ad->cf_state[0] == CST_SS_STATE_ON) {
			DBG("first item's state is CST_SS_STATE_ON, hence disable the other items in the genlist");
			next_item = elm_genlist_first_item_get(ad->backup_genlist);
			if (next_item) {
				elm_object_item_disabled_set(next_item, EINA_FALSE);
			}
		} else {
			while (NULL != next_item) {
				elm_object_item_disabled_set(next_item, EINA_FALSE);
				next_item = elm_genlist_item_next_get(next_item);
			}
			while (NULL != prev_item) {
				elm_object_item_disabled_set(prev_item, EINA_FALSE);
				prev_item = elm_genlist_item_prev_get(prev_item);
			}
		}
	}

	return;
}

static void __cst_query_cf_status(CstGlItemData_t *item_data)
{
	ret_if(NULL == item_data);
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	int cf_flavour = -1;

	ad->original_state = EINA_FALSE;
	switch (item_data->index) {
	case 0:
		cf_flavour = CST_SSTYPE_CF_UNCONDITIONAL;
		DBG("CST_SSTYPE_CF_UNCONDITIONAL");
		break;
	case 1:
		cf_flavour = CST_SSTYPE_CF_BUSY;
		DBG("CST_SSTYPE_CF_BUSY");
		break;
	case 2:
		cf_flavour = CST_SSTYPE_CF_NO_REPLY;
		DBG("CST_SSTYPE_CF_NO_REPLY");
		break;
	case 3:
		cf_flavour = CST_SSTYPE_CF_NOT_REACHABLE;
		DBG("CST_SSTYPE_CF_NOT_REACHABLE");
		break;
	default:
		DBG("Error type");
		break;
	}

	if (cf_flavour != -1) {
		_cst_add_ss_request(&ad->req_queue, CST_ACTION_QUERY,
							ad->call_type, cf_flavour, EINA_FALSE, NULL,
							__cst_update_cf_state, item_data, -1, ad);
	}
}

static void __cst_on_click_cf_ime_done_btn(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_cf_ime_done_btn);
	ret_if(!data);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;
	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ad->dg_entry);
	char buffer[CST_MAX_PHONE_NUMBER_LEN];

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_call_forwarding_input_panel_event_callback);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}
	evas_object_smart_callback_del(ad->dg_entry, "changed", _cst_call_forward_entry_changed_cb);
	evas_object_smart_callback_del(ad->dg_entry, "preedit,changed", _cst_call_forward_entry_changed_cb);

	_cst_cf_ime_destroy_genlist_item_class();

	keypad_enabled = 0;

	ad->b_expanded = EINA_FALSE;
	memset(ad->entry_string, 0, sizeof(ad->entry_string));

	if (_cst_get_input_number(buffer, ad) > 0) {
		b_notify_cb_enabled = EINA_FALSE;
		g_item_data = item_data;
		g_item_data->cf_cb_request_by = CST_CF_CB_INTERROGATE_BY_ACTIVATE;

		DBG("ad->cf_state[item_data->index]=%d", ad->cf_state[item_data->index]);
		if (ad->cf_state[item_data->index] == CST_SS_STATE_ON) {
			req->original_state = EINA_TRUE;
		} else if (ad->cf_state[item_data->index] == CST_SS_STATE_OFF) {
			req->original_state = EINA_FALSE;
		}

		ad->cf_state[item_data->index] = CST_SS_STATE_PROGRESS;
		elm_genlist_item_update(item_data->gl_item);
		DBG("activate CF num=%s", buffer);
		if (!strcmp(buffer, T_(CST_STR_UNKNOWN))) {
			strncpy(buffer, "", CST_MAX_PHONE_NUMBER_LEN);
		}
		snprintf(req->number, CST_MAX_PHONE_NUMBER_LEN, "%s", buffer);
		cst_forward_data.cf_wait_time = _cst_cf_ime_fetch_latest_wait_time();
		_cst_add_ss_request(&ad->req_queue, req->action,
							req->call_type, req->flavour, req->original_state, req->number,
							__cst_update_cf_state, item_data, cst_forward_data.cf_wait_time, ad);
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
	}

	free(req);
}

static Eina_Bool __cst_on_click_cf_ime_back_btn(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_cf_ime_back_btn);
	retv_if(!data, EINA_TRUE);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;
	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	CstAppData_t *ad = item_data->ad;
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ad->dg_entry);

	DBG("original state : %d", req->original_state);
	if (req->original_state == CST_SS_STATE_ON) {
		ad->cf_state[item_data->index] = CST_SS_STATE_ON;
	} else {
		ad->cf_state[item_data->index] = CST_SS_STATE_OFF;
	}
	elm_genlist_item_update(item_data->gl_item);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_call_forwarding_input_panel_event_callback);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}
	evas_object_smart_callback_del(ad->dg_entry, "changed", _cst_call_forward_entry_changed_cb);
	evas_object_smart_callback_del(ad->dg_entry, "preedit,changed", _cst_call_forward_entry_changed_cb);

	keypad_enabled = 0;

	ad->b_expanded = EINA_FALSE;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	_cst_cf_ime_destroy_genlist_item_class();

	free(req);

	return EINA_TRUE;
}

static void __cst_on_click_cf_ime_cancel_btn(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_cf_ime_cancel_btn);
	__cst_on_click_cf_ime_back_btn(data, NULL);
}

static char *__cst_gl_label_get_cf(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	char *ret = NULL;
	char *text = NULL;

	if (strcmp(part, "elm.text") == 0) {
		if (list_call_forwarding[item_data->index].str_id != -1) {
			ret = strdup(T_(list_call_forwarding[item_data->index].str_id));
		}
	} else if (strcmp(part, "elm.text.sub") == 0) {
		switch (ad->cf_state[item_data->index]) {
		case CST_SS_STATE_ON:
			if (list_call_forwarding[item_data->index].str_id ==
					CST_STR_IF_NO_REPLY) {
				int waiting_time_str_id = CST_CF_WAIT_TIME_30_SEC;

				switch (cst_forward_data.cf_wait_time) {
				case 5:
					waiting_time_str_id = CST_CF_WAIT_TIME_5_SEC;
					break;
				case 10:
					waiting_time_str_id = CST_CF_WAIT_TIME_10_SEC;
					break;
				case 15:
					waiting_time_str_id = CST_CF_WAIT_TIME_15_SEC;
					break;
				case 20:
					waiting_time_str_id = CST_CF_WAIT_TIME_20_SEC;
					break;
				case 25:
					waiting_time_str_id = CST_CF_WAIT_TIME_25_SEC;
					break;
				case 30:
					waiting_time_str_id = CST_CF_WAIT_TIME_30_SEC;
					break;
				default:
					ERR("Invalid waiting time: %d", cst_forward_data.cf_wait_time);
					waiting_time_str_id = CST_CF_WAIT_TIME_30_SEC;
					break;
				}
				text = g_strdup_printf(T_(CST_STR_WAITING_TIME_PERCENT_S_SECONDS), waiting_time_str_id);
				ret = g_strdup_printf("<color=#%02x%02x%02x%02x>%s,%s</color>", 61, 185, 204, 255, item_data->number, text);
				g_free(text);
			} else {
				ret = g_strdup_printf("<color=#%02x%02x%02x%02x>%s</color>", 61, 185, 204, 255, item_data->number);
			}
			break;

		default:
			ret = NULL;
			break;
		}
	}

	return ret;
}

static void __cst_gl_del_cf(void *data, Evas_Object *obj)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

Evas_Object *_cst_create_cf_popup(Evas_Object *parent, void *data,
		const char *info_text, char *editstring, Evas_Smart_Cb cb, void *cb_data)
{
	retv_if((parent == NULL) || (data == NULL), NULL);
	Evas_Object *btn1, *btn2;
	Evas_Object *layout = NULL;
	Evas_Object *eo = NULL;
	const char *entry_input = NULL;

	CallSettingSSReq_t *req = (CallSettingSSReq_t *) cb_data;
	CstAppData_t *ad = (CstAppData_t *)data;
	ad->popup = elm_popup_add(parent);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);

	layout = elm_layout_add(ad->popup);
	elm_layout_theme_set(layout, "layout", "popup", "callforwardingview");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	cst_util_domain_translatable_part_text_set(ad->popup, "title,text", info_text);

	eo = _cst_create_cf_ime(ad->nf, editstring,
										cst_forward_data.cf_wait_time, req->flavour, (void *)ad);
	elm_object_part_content_set(layout, "elm.swallow.content", eo);
	elm_object_content_set(ad->popup, layout);


	btn1 = elm_button_add(ad->popup);
	elm_object_style_set(btn1, "popup");
	cst_util_domain_translatable_text_set(btn1, I_(CST_STR_POP_CANCEL));
	elm_object_part_content_set(ad->popup, "button1", btn1);
	if (cb) {
		evas_object_smart_callback_add(btn1, "clicked", cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn1, "clicked", __cst_on_click_cf_ime_cancel_btn,
				cb_data);
	}

	btn2 = elm_button_add(ad->popup);
	elm_object_style_set(btn2, "popup");
	cst_util_domain_translatable_text_set(btn2, I_(CST_STR_ENABLE));
	elm_object_part_content_set(ad->popup, "button2", btn2);
	if (cb) {
		evas_object_smart_callback_add(btn2, "clicked", cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn2, "clicked", __cst_on_click_cf_ime_done_btn,
				cb_data);
	}

	entry_input = elm_entry_entry_get(ad->dg_entry);
	if (entry_input && strlen(entry_input) > 0) {
		elm_object_disabled_set(btn2, EINA_FALSE);
	} else {
		elm_object_disabled_set(btn2, EINA_TRUE);
	}
	ad->cf_btn = btn2;

	eext_object_event_callback_add(ad->popup, EEXT_CALLBACK_BACK,
									__cst_on_click_cf_ime_cancel_btn, cb_data);
	evas_object_show(ad->popup);
	return ad->popup;
}

static void __cst_update_change_cf_req(CstGlItemData_t *item_data, Eina_Bool req_state, Eina_Bool b_register_new_number)
{
	ret_if(NULL == item_data);
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	int title = CST_STR_FORWARD_TO;
	CallSettingSSReq_t *req = NULL;
	req = (CallSettingSSReq_t *)calloc(1, sizeof(CallSettingSSReq_t));
	ret_if(req == NULL);

	memset(cst_forward_data.ec_item, 0, sizeof(cst_forward_data.ec_item));

	ad->popup = NULL;
	req->data = item_data;
	req->call_type = ad->call_type;
	req->flavour = item_data->index;
	DBG("req->flavour = %d", req->flavour);

	ad->original_state = !req_state;
	if (req_state == EINA_TRUE) {
		if (b_register_new_number == EINA_TRUE) {
			req->action = CST_ACTION_REGISTER;
			char *edit_string = NULL;
			Eina_Bool check_state = EINA_FALSE;
			if (ad->cf_state[item_data->index] == CST_SS_STATE_ON) {
				check_state = EINA_TRUE;
			}
			if (check_state == EINA_TRUE) {
				req->original_state = CST_SS_STATE_ON;
				edit_string = item_data->number;
			} else {
				req->original_state = CST_SS_STATE_OFF;
			}
			_cst_create_cf_ime(ad->nf, edit_string,
								cst_forward_data.cf_wait_time, req->flavour, (void *)ad);

			switch (req->flavour) {
				case CST_SSTYPE_CF_UNCONDITIONAL:
					title = CST_STR_ALWAYS;
					break;
				case CST_SSTYPE_CF_BUSY:
					title = CST_STR_IF_BUSY;
					break;
				case CST_SSTYPE_CF_NO_REPLY:
					title = CST_STR_IF_NO_REPLY;
					break;
				case CST_SSTYPE_CF_NOT_REACHABLE:
					title = CST_STR_IF_OUT_OF_REACH;
					break;
				default:
					DBG("req->flavour = %d", req->flavour);
			}

			ad->popup = _cst_create_cf_popup(ad->nf, ad, I_(title), edit_string, NULL, req);

		} else {
			b_notify_cb_enabled = EINA_FALSE;
			g_item_data = item_data;
			g_item_data->cf_cb_request_by = CST_CF_CB_INTERROGATE_BY_ACTIVATE;

			ad->cf_state[item_data->index] = CST_SS_STATE_PROGRESS;
			_cst_update_genlist_item_class(item_data->gl_item, cst_forward_data.itc_1text_1icon);
			elm_genlist_item_update(item_data->gl_item);
			_cst_add_ss_request(&ad->req_queue, CST_ACTION_ACTIVATE,
								ad->call_type, req->flavour, EINA_FALSE, "",
								__cst_update_cf_state, item_data, -1, ad);
			free(req);
		}
	} else {
		b_notify_cb_enabled = EINA_FALSE;
		g_item_data = item_data;
		g_item_data->cf_cb_request_by = CST_CF_CB_INTERROGATE_BY_DEACTIVATE;

		ad->cf_state[item_data->index] = CST_SS_STATE_PROGRESS;
		_cst_update_genlist_item_class(item_data->gl_item, cst_forward_data.itc_1text_1icon);
		elm_genlist_item_update(item_data->gl_item);
		_cst_add_ss_request(&ad->req_queue, CST_ACTION_DEACTIVATE,
							ad->call_type, req->flavour, EINA_FALSE, NULL,
							__cst_update_cf_state, item_data, -1, ad);
		free(req);
	}
}

static void __cst_on_click_cf_onoff_forward_to(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	__cst_update_change_cf_req(item_data, EINA_TRUE, EINA_TRUE);
}

static void __cst_gl_sel_cf(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
	if (list_call_forwarding[item_data->index].func) {
		list_call_forwarding[item_data->index].func((void *)item_data, NULL, obj, item_data->gl_item);
	}
	return;
}

static void __cst_on_changed_cf_check(void *data, Evas_Object *obj, void *event_info)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	Eina_Bool check_state = elm_check_state_get(obj);
	if (elm_object_item_disabled_get(item_data->gl_item) == EINA_TRUE) {
		elm_check_state_set(obj, !check_state);
		return;
	}
	elm_check_state_set(obj, !check_state);
	__cst_update_change_cf_req(item_data, check_state, EINA_TRUE);
}

static Evas_Object *__cst_gl_icon_get_cf(void *data, Evas_Object *obj, const char *part)
{
	ENTER(__cst_gl_icon_get_cf);
	retv_if(NULL == data, NULL);
	Evas_Object *ly = NULL;
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	Eina_Bool check_state = EINA_FALSE;
	Elm_Object_Item *next_item;
	Elm_Object_Item *prev_item;

	DBG("ad->cf_state : %d(%d)", ad->cf_state[item_data->index], item_data->index);
	if (!strcmp(part, "elm.swallow.end")) {
		switch (ad->cf_state[item_data->index]) {
		case CST_SS_STATE_OFF:
			ly = item_data->eo_check = _cst_create_onoff_button(obj, check_state,
										 __cst_on_changed_cf_check, item_data);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DEFAULT);
			break;
		case CST_SS_STATE_PROGRESS:
			ly = _cst_create_progressbar(obj);
			elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_NONE);
			next_item = elm_genlist_item_next_get(item_data->gl_item);
			prev_item = elm_genlist_item_prev_get(item_data->gl_item);
			while (NULL != next_item) {
				elm_object_item_disabled_set(next_item, EINA_TRUE);
				next_item = elm_genlist_item_next_get(next_item);
			}
			while (NULL != prev_item) {
				elm_object_item_disabled_set(prev_item, EINA_TRUE);
				prev_item = elm_genlist_item_prev_get(prev_item);
			}
			item_data->eo_check = NULL;
			break;
		case CST_SS_STATE_ON:
			check_state = EINA_TRUE;
			ly = item_data->eo_check = _cst_create_onoff_button(obj, check_state,
										 __cst_on_changed_cf_check, item_data);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DEFAULT);
			break;
		default:
			return NULL;
		}
	}
	return ly;
}

static Eina_Bool __cst_forwarding_tapi_notify_timer_cb(void *data)
{
	ENTER(__cst_forwarding_tapi_notify_timer_cb);

	if (cst_forward_data.tapi_notify_timer) {
		ecore_timer_del(cst_forward_data.tapi_notify_timer);
		cst_forward_data.tapi_notify_timer = NULL;
	}

	int i = 0;
	for (; i < 4; i++) {
		__cst_query_cf_status(cf_item_data[i]);
	}

	return ECORE_CALLBACK_CANCEL;
}
static void __cst_forwarding_tapi_notify_cb(TapiHandle *handle, const char *noti_id, void *data, void *user_data)
{
	ENTER(__cst_forwarding_tapi_notify_cb);

	if (b_notify_cb_enabled) {

	if (cst_forward_data.tapi_notify_timer) {
		ecore_timer_del(cst_forward_data.tapi_notify_timer);
		cst_forward_data.tapi_notify_timer = NULL;
	}

		cst_forward_data.tapi_notify_timer = ecore_timer_add(5.0,
				__cst_forwarding_tapi_notify_timer_cb, NULL);
	} else {
		b_notify_cb_enabled = EINA_TRUE;
		DBG("cf_cb_request_by: (%d)", g_item_data->cf_cb_request_by);

		__cst_query_cf_status(g_item_data);
	}
}

static Evas_Object *__cst_create_genlist_cf(void *data)
{
	retv_if(NULL == data, NULL);
	CstAppData_t *ad = (CstAppData_t *)data;
	int index = 0;
	Evas_Object *genlist;
	CstGlItemData_t *item_data;
	b_notify_cb_enabled = EINA_TRUE;
	g_item_data = NULL;
	TapiResult_t err = TAPI_API_SUCCESS;
	ad->popup = _cst_create_processing_popup(ad->nf);
	evas_object_show(ad->popup);

	genlist = elm_genlist_add(ad->nf);
	for (index = 0; list_call_forwarding[index].style != CST_GL_ITEM_NONE; ++index) {
		if (list_call_forwarding[index].style == CST_GL_ITEM_2TEXT_ONOFF_PROGRESS) {
			item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
			retv_if(item_data == NULL, NULL);
			item_data->index = index;
			item_data->ad = ad;

			ad->cf_state[index] = CST_SS_STATE_PROGRESS;
			item_data->gl_item = elm_genlist_item_append(genlist, cst_forward_data.itc_1text_1icon,
								 (const void *)item_data, NULL, list_call_forwarding[index].flags,
								 __cst_gl_sel_cf, item_data);
			__cst_query_cf_status(item_data);
			cf_item_data[index] = item_data;
			err = tel_register_noti_event(ad->tapi_handle, TAPI_NOTI_SS_FORWARD_STATUS, (__cst_forwarding_tapi_notify_cb), (void *)item_data);
			if (err != TAPI_API_SUCCESS) {
				DBG("tel_register_noti_event failed: err(%d)", err);
			}
		} else {
			DBG("No style");
			return NULL;
		}
		ad->cf_gl_item[index] = item_data->gl_item;
	}

	return genlist;
}

static void __cst_cf_create_genlist_item_class(void *data)
{
	ENTER(__cst_cf_create_genlist_item_class);
	ret_if(NULL == data);

	cst_forward_data.itc_2text_1icon = NULL;
	cst_forward_data.itc_1text_1icon = NULL;
	cst_forward_data.itc_forward_to = NULL;
	cst_forward_data.itc_waiting_time = NULL;
	cst_forward_data.itc_waiting_time_expand = NULL;
	cst_forward_data.forwardto_item = NULL;
	cst_forward_data.waitingtime_item = NULL;
	cst_forward_data.waitingtime_radio_btn = NULL;
	cst_forward_data.cf_sel_item = NULL;

	if (!cst_forward_data.itc_2text_1icon) {
		cst_forward_data.itc_2text_1icon = _cst_create_genlist_item_class("type1",
					__cst_gl_label_get_cf, __cst_gl_icon_get_cf,
					NULL, __cst_gl_del_cf);
	}
	if (!cst_forward_data.itc_1text_1icon) {
		cst_forward_data.itc_1text_1icon = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_cf,
				__cst_gl_icon_get_cf, NULL, __cst_gl_del_cf);
	}
	if (!cst_forward_data.itc_forward_to) {
		cst_forward_data.itc_forward_to = _cst_create_genlist_item_class(NULL, NULL, NULL, NULL, NULL);
	}
	if (!cst_forward_data.itc_waiting_time) {
		cst_forward_data.itc_waiting_time = _cst_create_genlist_item_class(NULL, NULL, NULL, NULL, NULL);
	}
	if (!cst_forward_data.itc_waiting_time_expand) {
		cst_forward_data.itc_waiting_time_expand = _cst_create_genlist_item_class(NULL, NULL, NULL, NULL, NULL);
	}
}

static void __cst_gl_on_click_waitingtime_expand(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_on_click_waitingtime_expand);
	ret_if(NULL == data);
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	static int selected_waittime = -1;
	CstAppData_t *ad = (CstAppData_t *)((CstGlItemData_t *)data)->ad;

	if (ad->cf_state[cst_forward_data.forwarding_item->index] == CST_SS_STATE_PROGRESS) { /* Ignore as another SS request is already active */
		return;
	}

	if (elm_genlist_item_expanded_get(item)) {
		__cst_gl_collapse_req(data, obj, event_info);
		__cst_gl_collapse(selected_waittime, obj, event_info);
	} else {
		selected_waittime = cst_forward_data.cf_wait_time_index;
		__cst_gl_expand_req(data, obj, event_info);
		__cst_gl_expand(data, obj, event_info);
	}
}

static void __cst_gl_collapse_req(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_collapse_req);
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_expanded_set(item, 0);
}

static void __cst_gl_collapse(int selected_time, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_collapse);
	Elm_Object_Item *item = event_info;
	elm_genlist_item_subitems_clear(item);
	elm_genlist_item_update(item);
	elm_object_item_signal_emit(item, "elm,state,bottom", "");

	if (selected_time != cst_forward_data.cf_wait_time_index) {
		__cst_update_wait_time_and_index(cst_forward_data.cf_wait_time_index, CST_CF_WAIT_TIME_NONE);
		__cst_gl_update_waiting_time_ss_request();
	}
}

static void __cst_gl_expand_req(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_expand_req);
	Elm_Object_Item *item = event_info;
	elm_genlist_item_expanded_set(item, 1);
}

static void __cst_gl_expand(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_expand);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	ret_if(NULL == it);
	Evas_Object *genlist = elm_object_item_widget_get(it);
	ret_if(NULL == genlist);
	CstGlItemData_t *sub_item_data;
	int i = 0;

	cst_forward_data.waitingtime_radio_btn = NULL;
	cst_forward_data.waitingtime_radio_btn = elm_radio_add(genlist);
	elm_radio_value_set(cst_forward_data.waitingtime_radio_btn, -1);
	for (i = 0; list_call_forwarding_dep2_waiting_time[i].style != CST_GL_ITEM_NONE; ++i) {
		sub_item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		ret_if(sub_item_data == NULL);
		sub_item_data->index = i;
		sub_item_data->ad = ad;
		sub_item_data->parent_gl_item = it;
		sub_item_data->gl_item = elm_genlist_item_append(genlist, cst_forward_data.itc_waiting_time_expand,
								(const void *)sub_item_data, it,
								list_call_forwarding_dep2_waiting_time[i].flags,
								__cst_gl_sel_waiting_time, sub_item_data);
	}
}

static void __cst_gl_update_waiting_time_ss_request(void)
{
	ENTER("__cst_gl_update_waiting_time_ss_request");
	CstGlItemData_t *item_data = (CstGlItemData_t *)cst_forward_data.forwarding_item;
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	CallSettingSSReq_t *req = NULL;

	req = (CallSettingSSReq_t *)calloc(1, sizeof(CallSettingSSReq_t));
	ret_if(req == NULL);
	ad->popup = NULL;
	req->data = (void *)item_data->gl_item;
	req->call_type = ad->call_type;
	req->flavour = CST_SSTYPE_CF_NO_REPLY;
	req->action = CST_ACTION_REGISTER;
	req->original_state = CST_SS_STATE_ON;
	ad->cf_state[item_data->index] = CST_SS_STATE_PROGRESS;
	elm_genlist_item_update(item_data->gl_item);
	snprintf(req->number, CST_MAX_PHONE_NUMBER_LEN, "%s", cst_forward_data.forwardto_item->number);
	_cst_add_ss_request(&ad->req_queue, req->action,
						req->call_type, req->flavour, EINA_FALSE, req->number,
						__cst_update_cf_state, cst_forward_data.cf_sel_item, cst_forward_data.cf_wait_time, ad);
	free(req);
}

static void __cst_gl_sel_waiting_time(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	cst_forward_data.cf_wait_time_index = (long)list_call_forwarding_dep2_waiting_time[item_data->index].func;
	if (cst_forward_data.waitingtime_radio_btn) {
		elm_radio_value_set(cst_forward_data.waitingtime_radio_btn,
							(long)list_call_forwarding_dep2_waiting_time[item_data->index].func);
		elm_genlist_item_update(item_data->parent_gl_item);
		__cst_gl_on_click_waitingtime_expand(data, NULL, obj,
				item_data->parent_gl_item);
	}
	return;
}

static void __cst_update_wait_time_and_index(int wait_time_index, int wait_time)
{
	ENTER(__cst_update_wait_time_and_index);

	if (wait_time_index != CST_CF_WAIT_TIME_IDX_NONE) { /* Update the waiting time from the index */
		if (wait_time_index == CST_CF_WAIT_TIME_IDX_5_SEC) {
			cst_forward_data.cf_wait_time = CST_CF_WAIT_TIME_5_SEC;
		} else if (wait_time_index == CST_CF_WAIT_TIME_IDX_10_SEC) {
			cst_forward_data.cf_wait_time = CST_CF_WAIT_TIME_10_SEC;
		} else if (wait_time_index == CST_CF_WAIT_TIME_IDX_15_SEC) {
			cst_forward_data.cf_wait_time = CST_CF_WAIT_TIME_15_SEC;
		} else if (wait_time_index == CST_CF_WAIT_TIME_IDX_20_SEC) {
			cst_forward_data.cf_wait_time = CST_CF_WAIT_TIME_20_SEC;
		} else if (wait_time_index == CST_CF_WAIT_TIME_IDX_25_SEC) {
			cst_forward_data.cf_wait_time = CST_CF_WAIT_TIME_25_SEC;
		} else if (wait_time_index == CST_CF_WAIT_TIME_IDX_30_SEC) {
			cst_forward_data.cf_wait_time = CST_CF_WAIT_TIME_30_SEC;
		}
	} else if (wait_time != CST_CF_WAIT_TIME_NONE) { /* Update the waiting time index from the time */
		if (wait_time == CST_CF_WAIT_TIME_5_SEC) {
			cst_forward_data.cf_wait_time_index = CST_CF_WAIT_TIME_IDX_5_SEC;
		} else if (wait_time == CST_CF_WAIT_TIME_10_SEC) {
			cst_forward_data.cf_wait_time_index = CST_CF_WAIT_TIME_IDX_10_SEC;
		} else if (wait_time == CST_CF_WAIT_TIME_15_SEC) {
			cst_forward_data.cf_wait_time_index = CST_CF_WAIT_TIME_IDX_15_SEC;
		} else if (wait_time == CST_CF_WAIT_TIME_20_SEC) {
			cst_forward_data.cf_wait_time_index = CST_CF_WAIT_TIME_IDX_20_SEC;
		}  else if (wait_time == CST_CF_WAIT_TIME_25_SEC) {
			cst_forward_data.cf_wait_time_index = CST_CF_WAIT_TIME_IDX_25_SEC;
		}  else if (wait_time == CST_CF_WAIT_TIME_30_SEC) {
			cst_forward_data.cf_wait_time_index = CST_CF_WAIT_TIME_IDX_30_SEC;
		}
	}

	if (cst_forward_data.waitingtime_item) {
		elm_genlist_item_update(cst_forward_data.waitingtime_item->gl_item);
	}
	return;
}

void _cst_on_click_call_forwarding(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_call_forwarding);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	ret_if(ad == NULL);
	Elm_Object_Item *navi_it;
	Evas_Object *back_btn = NULL;

	if (_cst_check_flight_mode()) {
		_cst_create_error_popup(CST_ERROR_CHANGE_FLIGHT_MODE);
		return;
	}

	if (!_cst_check_dual_sim_status(ad)) {
		_cst_create_error_popup(CST_ERROR_INSERT_SIM_CARD);
		return;
	}

	ad->call_type = CST_CALLTYPE_VOICE;
	ad->popup = NULL;

	__cst_cf_create_genlist_item_class(ad);

	ad->backup_genlist = __cst_create_genlist_cf(ad);

	back_btn = _cst_util_navi_back_btn_create(ad->nf);

	navi_it = elm_naviframe_item_push(ad->nf, I_(CST_STR_CALL_FORWARDING),
			back_btn, NULL, ad->backup_genlist, NULL);
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_CALL_FORWARDING));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_click_cf_back_button, (void *)ad);
}

void _cst_call_forwarding_input_panel_event_callback(void *data, Ecore_IMF_Context *imf_context, int value)
{
	ENTER(_cst_call_forwarding_input_panel_event_callback);

	ret_if(NULL == data);
	CstAppData_t *ad = (CstAppData_t *)data;

	if (value == ECORE_IMF_INPUT_PANEL_STATE_WILL_SHOW) {
		return;
	}

	if (1 > keypad_enabled) {
		DBG("keypad_enabled = %d", keypad_enabled);
		keypad_enabled++;

		if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
			return;
		}

	}

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_SHOW");
		if (ad->b_expanded && ad->gl_expanded_item) {
			elm_genlist_item_expanded_set(ad->gl_expanded_item, EINA_FALSE);
			elm_genlist_item_subitems_clear(ad->gl_expanded_item);
			elm_genlist_item_update(ad->gl_expanded_item);
			elm_object_item_signal_emit(ad->gl_expanded_item, "elm,state,bottom", "");
			ad->b_expanded = EINA_FALSE;
			ad->gl_expanded_item = NULL;
		}
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_HIDE");
	}
}

void _cst_call_forward_entry_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	DBG("_cst_call_forward_entry_changed_cb calling");
	ret_if(!data);
	CstAppData_t *ad = (CstAppData_t *)data;
	Evas_Object *btn = NULL;
	const char *entry_input = NULL;

	Elm_Object_Item *top_it = elm_naviframe_top_item_get(ad->nf);
	ret_if(!top_it);

	entry_input = elm_entry_entry_get(ad->dg_entry);
	snprintf(ad->entry_string, sizeof(ad->entry_string), "%s", entry_input);
	if ((btn = elm_object_item_part_content_get(top_it, "title_right_btn")) != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(btn, EINA_FALSE);
		} else {
			elm_object_disabled_set(btn, EINA_TRUE);
		}
	} else {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(ad->cf_btn, EINA_FALSE);
		} else {
			elm_object_disabled_set(ad->cf_btn, EINA_TRUE);
		}
	}
}

void _cst_destroy_call_forwarding(void)
{
	ENTER(_cst_destroy_call_forwarding);

	__cst_cf_destroy_genlist_item_class();
}
