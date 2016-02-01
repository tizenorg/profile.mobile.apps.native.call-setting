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

#ifdef _CALL_SET_BARRING_SUPPORT

#include <Elementary.h>

#include "cst-common-string.h"
#include "cst-call-setting.h"

#include "cst-barring.h"
#include "cst-widget.h"
#include "cst-common.h"
#include "cst-tapi-request.h"
#include "cst-util.h"

static Evas_Object *more_btn			= NULL;
static Evas_Object *ctxpopup			= NULL;

static void __cst_destroy_genlist_item_styles(void);
static void __cst_on_click_cb_onoff(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_on_click_voice_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static Eina_Bool __cst_on_click_cb_voice_video_back_button(void *data, Elm_Object_Item *it);
static void __cst_on_click_change_password_cb(void *data, Evas_Object *obj, void *event_info);
static void __cst_call_barr_popup_resize_more_ctxpopup_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info);

static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_1text = NULL;
static Evas_Object *ec_item[3];

static Evas_Object *barring_gl = NULL;

static gboolean keypad_enabled = EINA_FALSE;

static gboolean b_notify_cb_enabled = EINA_TRUE;

static CstGlItemData_t *cb_item_data[5];
static CstGlItemData_t *g_item_data = NULL;
static Evas_Object *entry = NULL;
static CstGlItemDisplayInfo_t list_call_type[] = {
	{1, CST_STR_VOICECALL, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, __cst_on_click_voice_cb},
	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static CstGlItemDisplayInfo_t list_call_barring[] = {
	{1, CST_STR_CALL_ALL_OUTGOING_CALLS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF_PROGRESS, __cst_on_click_cb_onoff},
	{1, CST_STR_INTL_CALLS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF_PROGRESS, __cst_on_click_cb_onoff},
	{1, CST_STR_CALL_INTL_EXCEPT_HOME, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF_PROGRESS, __cst_on_click_cb_onoff},
	{1, CST_STR_ALL_INCOMING_CALLS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF_PROGRESS, __cst_on_click_cb_onoff},
	{1, CST_STR_CALL_INCOMING_WHEN_ROAMING, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF_PROGRESS, __cst_on_click_cb_onoff},

	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_1text_1icon) {
		_cst_destroy_genlist_item_class(itc_1text_1icon);
		itc_1text_1icon = NULL;
	}
	if (itc_1text) {
		_cst_destroy_genlist_item_class(itc_1text);
		itc_1text = NULL;
	}
}

void _cst_call_barring_entry_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_call_barring_entry_changed_cb);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *btn = NULL;
	const char *entry_input = NULL;

	Elm_Object_Item *top_it = elm_naviframe_top_item_get(ugd->nf);
	ret_if(!top_it);

	entry_input = elm_entry_entry_get(ugd->dg_entry);
	if ((btn = elm_object_item_part_content_get(top_it, "title_right_btn")) != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(btn, EINA_FALSE);
		} else {
			elm_object_disabled_set(btn, EINA_TRUE);
		}
	} else if (ec_item[1] != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	}
}

static Eina_Bool __cst_on_click_cb_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_cb_back_button);

	__cst_on_click_cb_voice_video_back_button(data, NULL);

	_cst_destroy_call_barring();

	return EINA_TRUE;
}

static void __cst_disable_cb_glitem(CstGlItemData_t *item_data)
{
	ENTER(__cst_disable_cb_glitem);
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ugd->cb_state[item_data->index] = CST_SS_STATE_OFF;
	elm_genlist_item_update(item_data->gl_item);
#ifdef	_CALL_SET_TBD
	elm_object_item_disabled_set(item_data->gl_item, EINA_TRUE);
#endif  /* _CALL_SET_TBD */
}

static int __cst_update_cb_state(int call_type, int cb_flavour,
								 Eina_Bool cb_state, char *number,
								 int error, int req_action, void *data)
{
	retv_if(NULL == data, -1);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = item_data->ugd;
	DBG("call_type=%d (0:voice 1:video)", call_type);
	DBG("cb_flavour=%d (6:BAOC 7:BAOIC 8:BAOIC exc home 9:BAIC 10:BAIC roaming)", cb_flavour);
	DBG("cb_state=%d (1:on 0:off), error=%d", cb_state, error);
	DBG("req_action=%d(0: Activate, 1: Deactivate, 2: Register, 3: Query)", req_action);
	retv_if((call_type != ugd->call_type && call_type != CST_CALLTYPE_ALL), -1);

	if (error == CST_ERROR_NONE) {
		if ((item_data->cf_cb_request_by == CST_CF_CB_INTERROGATE_BY_ACTIVATE) && (cb_state == EINA_FALSE)) {
			_cst_create_error_popup(CST_ERROR_OEM_NOT_SUPPORTED);
		} else if ((item_data->cf_cb_request_by == CST_CF_CB_INTERROGATE_BY_DEACTIVATE) && (cb_state == EINA_TRUE)) {
			_cst_create_error_popup(CST_ERROR_OEM_NOT_SUPPORTED);
		}

		if (cb_state == EINA_TRUE) {
			ugd->cb_state[item_data->index] = CST_SS_STATE_ON;
		} else {
			ugd->cb_state[item_data->index] = CST_SS_STATE_OFF;
		}
		elm_genlist_item_update(item_data->gl_item);
	} else {
		if (error == CST_ERROR_FDN_MODE_ACTIVE) {
			if (ugd->popup) {
				evas_object_del(ugd->popup);
				ugd->popup = NULL;
			}
			ugd->popup = (Evas_Object *)_cst_create_error_popup_with_ok_btn(ugd,
					error);
			evas_object_show(ugd->popup);
		}

		if (req_action != CST_ACTION_QUERY) {
			if (error != CST_ERROR_SERVICE_UNAVAILABLE &&
					error != CST_ERROR_REJECTED_BY_NETWORK &&
					error != CST_ERROR_OEM_NOT_SUPPORTED &&
					error != CST_ERROR_UNKNOWN) {
				if (req_action == CST_ACTION_DEACTIVATE) {
					ugd->cb_state[item_data->index] = CST_SS_STATE_ON;
				} else {
					ugd->cb_state[item_data->index] = CST_SS_STATE_OFF;
				}
			} else {
				ugd->cb_state[item_data->index] = CST_SS_STATE_OFF;
			}

			elm_genlist_item_update(item_data->gl_item);
			if (ugd->popup) {
				evas_object_del(ugd->popup);
				ugd->popup = NULL;
			}
			_cst_create_error_popup(error);
		} else {
			__cst_disable_cb_glitem(item_data);
		}
	}
	return 0;
}

static void __cst_query_cb_status(CstGlItemData_t *item_data)
{
	ENTER(__cst_query_cb_status);
	ret_if(NULL == item_data);
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	int cb_flavour = -1;

	ugd->original_state = EINA_FALSE;
	switch (item_data->index) {
	case 0:
		cb_flavour = CST_SSTYPE_CB_OC;
		DBG("CST_SSTYPE_CB_OC");
		break;
	case 1:
		cb_flavour = CST_SSTYPE_CB_OIC;
		DBG("CST_SSTYPE_CB_OIC");
		break;
	case 2:
		cb_flavour = CST_SSTYPE_CB_OICEH;
		DBG("CST_SSTYPE_CB_OICEH");
		break;
	case 3:
		cb_flavour = CST_SSTYPE_CB_IC;
		DBG("CST_SSTYPE_CB_IC");
		break;
	case 4:
		cb_flavour = CST_SSTYPE_CB_ICR;
		DBG("CST_SSTYPE_CB_ICR");
		break;
	default:
		DBG("Error type");
		break;
	}

	_cst_add_ss_request(&ugd->req_queue,
						CST_ACTION_QUERY, ugd->call_type, cb_flavour, EINA_FALSE,
						NULL, __cst_update_cb_state, item_data, -1, ugd);
}

static int __cst_get_cb_input_password(char *buffer, void *data)
{
	ENTER(__cst_get_cb_input_password);
	CstUgData_t *ugd = (CstUgData_t *)data;
	retv_if(!ugd, -1);
	const char *entry_input;
	Ecore_IMF_Context *imf_ctx;

	entry_input = elm_entry_entry_get(ugd->dg_entry);
	retv_if(!entry_input, -1);
	if (strlen(entry_input) == 0) {
		imf_ctx = elm_entry_imf_context_get(ugd->dg_entry);
		if (imf_ctx) {
			ecore_imf_context_input_panel_hide(imf_ctx);
		}
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		_cst_create_error_popup(CST_ERROR_ENTER_NUMBER);
		return -1;
	}

	DBG("IME input string = %s length=%d", entry_input, strlen(entry_input));

	snprintf(buffer, CST_MAX_PHONE_NUMBER_LEN, "%s", entry_input);
	return strlen(entry_input);
}

static void __cst_on_click_cb_ime_done_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cb_ime_done_btn);
	ret_if(!data);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;

	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	ret_if(!item_data);
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);
	ret_if(!ugd->popup);
	ret_if(!ugd->dg_entry);

	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);
	ret_if(!imf_context);

	char buffer[CST_MAX_PHONE_NUMBER_LEN] = {0, };

	if (__cst_get_cb_input_password(buffer, ugd) <= 0) {
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		if (req) {
			free(req);
		}
		DBG("__cst_get_cb_input_password <= 0");
		return;
	}
	b_notify_cb_enabled = EINA_FALSE;
	g_item_data = item_data;

	ugd->cb_state[item_data->index] = CST_SS_STATE_PROGRESS;
	elm_genlist_item_update(item_data->gl_item);

	DBG("activate CB pwd=%s", buffer);
	snprintf(req->number, CST_MAX_PHONE_NUMBER_LEN, "%s", buffer);
	if (req->action == CST_ACTION_ACTIVATE) {
		item_data->cf_cb_request_by = CST_CF_CB_INTERROGATE_BY_ACTIVATE;
	} else if (req->action == CST_ACTION_DEACTIVATE) {
		item_data->cf_cb_request_by = CST_CF_CB_INTERROGATE_BY_DEACTIVATE;
	}

	_cst_add_ss_request(&ugd->req_queue,
						req->action, req->call_type, req->flavour, EINA_FALSE,
						req->number, __cst_update_cb_state, item_data, -1, ugd);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_call_barring_input_panel_event_callback);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}
	evas_object_smart_callback_del(ugd->dg_entry, "changed", _cst_call_barring_entry_changed_cb);
	evas_object_smart_callback_del(ugd->dg_entry, "preedit,changed", _cst_call_barring_entry_changed_cb);

	evas_object_del(ugd->dg_entry);
	ugd->dg_entry = NULL;

	keypad_enabled = EINA_FALSE;
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

static void __cst_on_click_cb_ime_cancel_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cb_ime_cancel_btn);
	ret_if(!data);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;
	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	ret_if(!item_data);
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);

	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);
	ret_if(!imf_context);

	DBG("original state : %d", req->original_state);
	if (req->original_state == CST_SS_STATE_ON) {
		ugd->cb_state[item_data->index] = CST_SS_STATE_ON;
	} else {
		ugd->cb_state[item_data->index] = CST_SS_STATE_OFF;
	}
	elm_genlist_item_update(item_data->gl_item);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context,
				ECORE_IMF_INPUT_PANEL_STATE_EVENT,
				_cst_call_barring_input_panel_event_callback);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}
	evas_object_smart_callback_del(ugd->dg_entry, "changed",
			_cst_call_barring_entry_changed_cb);
	evas_object_smart_callback_del(ugd->dg_entry, "preedit,changed",
			_cst_call_barring_entry_changed_cb);
	keypad_enabled = EINA_FALSE;

	if (req) {
		free(req);
	}

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	return;
}

static char *__cst_gl_label_get_cb(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (strcmp(part, "elm.text.main.left") == 0) {
		if (list_call_barring[item_data->index].str_id != -1) {
			return strdup(T_(list_call_barring[item_data->index].str_id));
		}
	}
	return NULL;
}

static void __cst_gl_del_cb(void *data, Evas_Object *obj)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

static void __cst_barring_create_password_confirm_popup_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_barring_create_password_confirm_popup_cancel_cb);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
	ugd->check_state = EINA_FALSE;
}

static void __cst_barring_create_password_confirm_popup_check_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_barring_create_password_confirm_popup_check_cb);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Eina_Bool check_state = ugd->check_state;
	if (ugd->dg_entry) {
		elm_entry_password_set(ugd->dg_entry, check_state);
		ugd->check_state = !check_state;
	}
}

static void __cst_barring_create_password_confirm_popup(CstUgData_t *ugd,
		int popup_title, Evas_Smart_Cb ok_cb, Evas_Smart_Cb cancel_cb,
		void *cb_data, Evas_Object **c_item)
{
	ENTER(__cst_barring_create_password_confirm_popup);
	ret_if(NULL == cb_data);

	Evas_Object *layout = NULL;
	Evas_Object *btn_ok = NULL;
	Evas_Object *btn_cancel = NULL;
	Evas_Object *btn_check = NULL;

	/* Destroy popup if exists */
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	ugd->popup = elm_popup_add(ugd->nf);
	elm_object_style_set(ugd->popup, "dimrect");
	evas_object_size_hint_weight_set(ugd->popup, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	layout = elm_layout_add(ugd->popup);
	elm_layout_theme_set(layout, "layout", "popup",
			"entrypasswordview_with_show_password");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	cst_util_domain_translatable_part_text_set(ugd->popup, "title,text",
			I_(popup_title));

	entry = _cst_create_ime_editfield(ugd, layout, CST_IME_CALL_BAR, NULL);
	elm_object_style_set(entry, "editfield/password/popup");
	elm_object_signal_emit(entry, "elm,action,hide,search_icon", "");
	elm_object_part_content_set(layout, "elm.swallow.content", entry);

	elm_object_content_set(ugd->popup, layout);

	/* Cancel Button */
	btn_cancel = elm_button_add(ugd->popup);
	cst_util_domain_translatable_text_set(btn_cancel, I_(CST_STR_POP_CANCEL));

	elm_object_part_content_set(ugd->popup, "button1", btn_cancel);
	if (cancel_cb) {
		evas_object_smart_callback_add(btn_cancel, "clicked", cancel_cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn_cancel, "clicked",
				__cst_barring_create_password_confirm_popup_cancel_cb, ugd);
	}
	if (c_item)
		c_item[0] = btn_cancel;

	/* Ok Button */
	btn_ok = elm_button_add(ugd->popup);
	cst_util_domain_translatable_text_set(btn_ok, I_(CST_STR_OK));

	elm_object_part_content_set(ugd->popup, "button2", btn_ok);
	evas_object_smart_callback_add(btn_ok, "clicked", ok_cb, cb_data);
	elm_object_disabled_set(btn_ok, EINA_TRUE);
	if (c_item)
		c_item[1] = btn_ok;

	/* Check Button */
	btn_check = elm_check_add(ugd->popup);
	elm_object_style_set(btn_check, "popup");
	elm_check_state_set(btn_check, ugd->check_state);
	cst_util_domain_translatable_text_set(btn_check, I_(CST_STR_SHOW_PASSWORD));
	elm_object_focus_allow_set(btn_check, EINA_FALSE);
	elm_entry_password_set(ugd->dg_entry, !ugd->check_state);
	evas_object_smart_callback_add(btn_check, "changed",
			__cst_barring_create_password_confirm_popup_check_cb, ugd);
	elm_object_part_content_set(layout, "elm.swallow.end", btn_check);

	if (cancel_cb) {
		eext_object_event_callback_add(ugd->popup, EEXT_CALLBACK_BACK, cancel_cb,
				cb_data);
	} else {
		eext_object_event_callback_add(ugd->popup, EEXT_CALLBACK_BACK,
				__cst_barring_create_password_confirm_popup_cancel_cb, ugd);
	}
	evas_object_show(ugd->popup);

	return;
}

static Eina_Bool __cst_barring_check_nw_req_progress(void *data)
{
	retv_if(NULL == data, EINA_FALSE);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int index = 0;

	for (index = 0; list_call_barring[index].style != CST_GL_ITEM_NONE; ++index) {
		if (ugd->cb_state[index] == CST_SS_STATE_PROGRESS) {
			return EINA_TRUE;
		}
	}

	return EINA_FALSE;
}

static void __cst_update_change_cb_req(CstGlItemData_t *item_data, Eina_Bool req_state)
{
	ret_if(item_data == NULL);

	CallSettingSSReq_t *req = NULL;
	CstUgData_t *ugd = item_data->ugd;
	const char *entry_input = NULL;
	int title = CST_STR_PASSWORD;

	if (__cst_barring_check_nw_req_progress(ugd) == EINA_TRUE) {
		DBG("One request pending so returning");
		return;
	}

	req = (CallSettingSSReq_t *)calloc(1, sizeof(CallSettingSSReq_t));
	ret_if(req == NULL);

	memset(ec_item, 0, sizeof(ec_item));

	ugd->popup = NULL;
	req->data = item_data;
	req->call_type = ugd->call_type;
	req->flavour = -1;

	DBG("req_state = %d", req_state);
	switch (item_data->index) {
	case 0:
		req->flavour = CST_SSTYPE_CB_OC;
		DBG("CST_SSTYPE_CB_OC");
		title = CST_STR_CALL_ALL_OUTGOING_CALLS;
		break;
	case 1:
		req->flavour = CST_SSTYPE_CB_OIC;
		DBG("CST_SSTYPE_CB_OIC");
		title = CST_STR_INTL_CALLS;
		break;
	case 2:
		req->flavour = CST_SSTYPE_CB_OICEH;
		DBG("CST_SSTYPE_CB_OICEH");
		title = CST_STR_CALL_INTL_EXCEPT_HOME;
		break;
	case 3:
		req->flavour = CST_SSTYPE_CB_IC;
		DBG("CST_SSTYPE_CB_IC");
		title = CST_STR_ALL_INCOMING_CALLS;
		break;
	case 4:
		req->flavour = CST_SSTYPE_CB_ICR;
		DBG("CST_SSTYPE_CB_ICR");
		title = CST_STR_CALL_INCOMING_WHEN_ROAMING;
		break;
	default:
		DBG("Error type");
		break;
	}

	ugd->original_state = !req_state;
	if (req_state == EINA_TRUE) {
		req->original_state = CST_SS_STATE_OFF;
		req->action = CST_ACTION_ACTIVATE;
	} else {
		req->original_state = CST_SS_STATE_ON;
		req->action = CST_ACTION_DEACTIVATE;
	}

	__cst_barring_create_password_confirm_popup(ugd, title,
			__cst_on_click_cb_ime_done_btn, __cst_on_click_cb_ime_cancel_btn,
			(void *)req, ec_item);

	entry_input = elm_entry_entry_get(ugd->dg_entry);

	if (ec_item[1] != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	}
}

static void __cst_on_click_cb_onoff(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);
	if (ugd->cb_state[item_data->index] == CST_SS_STATE_PROGRESS) {
		return;
	}

	Eina_Bool check_state = elm_check_state_get(item_data->eo_check);

	__cst_update_change_cb_req(item_data, !check_state);
	elm_genlist_item_update(item_data->gl_item);
}

static void __cst_gl_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
	if (list_call_barring[item_data->index].func) {
		list_call_barring[item_data->index].func((void *)item_data, NULL, obj, event_info);
	}
	return;
}

static void __cst_on_changed_cb_check(void *data, Evas_Object *obj, void *event_info)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = item_data->ugd;
	Eina_Bool check_state = elm_check_state_get(obj);

	if (__cst_barring_check_nw_req_progress(ugd) == EINA_TRUE) {
		elm_check_state_set(obj, !elm_check_state_get(item_data->eo_check);
		return;
	}

	if (elm_object_item_disabled_get(item_data->gl_item) == EINA_TRUE) {
		elm_check_state_set(obj, EINA_FALSE);
		return;
	}

	__cst_update_change_cb_req(item_data, check_state);
}

static Evas_Object *__cst_gl_icon_get_cb(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	Evas_Object *icon = NULL;
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Eina_Bool check_state = EINA_FALSE;

	if (!strcmp(part, "elm.icon.right")) {
		switch (ugd->cb_state[item_data->index]) {
		case CST_SS_STATE_ON:
		case CST_SS_STATE_OFF:
			DBG("ugd->cb_state : %d(%d)",
				ugd->cb_state[item_data->index], item_data->index);
			if (ugd->cb_state[item_data->index] == CST_SS_STATE_ON) {
				check_state = EINA_TRUE;
			}
			item_data->eo_check = icon = _cst_create_onoff_button(obj, check_state,
										 __cst_on_changed_cb_check, item_data);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DEFAULT);
			break;
		case CST_SS_STATE_PROGRESS:
			icon = _cst_create_progressbar(obj);
			elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_NONE);
			item_data->eo_check = NULL;
			break;
		default:
			return NULL;
		}
	}
#ifdef _CALL_SET_TTS_SUPPORT
	/* Unregister the access object */
	if (elm_config_access_get() && icon) {
		elm_access_object_unregister(icon);
	}
#endif
	return icon;
}

static Eina_Bool __cst_on_click_cb_voice_video_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_cb_voice_video_back_button);

	retv_if(data == NULL, EINA_TRUE);

	CstUgData_t *ugd = (CstUgData_t *)data;

	ugd->back_button = _cst_get_navifr_prev_btn(ugd->nf);
	tel_deregister_noti_event(ugd->tapi_handle, TAPI_NOTI_SS_BARRING_STATUS);
	_cst_cancel_all_ss_request(ugd);

	return EINA_TRUE;
}

static Eina_Bool __cst_barring_tapi_notify_timer_cb(void *data)
{
	ENTER(__cst_barring_tapi_notify_timer_cb);

	int i = 0;
	for (; i < 5; i++) {
		__cst_query_cb_status(cb_item_data[i]);
	}

	return ECORE_CALLBACK_CANCEL;
}

static void __cst_barring_tapi_notify_cb(TapiHandle *handle, const char *noti_id, void *data, void *user_data)
{
	ENTER(__cst_barring_tapi_notify_cb);
	if (b_notify_cb_enabled) {
		ecore_timer_add(5.0, __cst_barring_tapi_notify_timer_cb, NULL);
	} else {
		b_notify_cb_enabled = EINA_TRUE;
		__cst_query_cb_status(g_item_data);
	}
}

static Evas_Object *__cst_create_genlist_cb(void *data)
{
	ENTER(__cst_create_genlist_cb);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int index = 0;
	Evas_Object *genlist;
	CstGlItemData_t *item_data;
	b_notify_cb_enabled = EINA_TRUE;
	g_item_data = NULL;
	TapiResult_t err = TAPI_API_SUCCESS;

	genlist = elm_genlist_add(ugd->nf);

	_cst_create_genlist_separator(genlist, EINA_FALSE);
	for (index = 0; list_call_barring[index].style != CST_GL_ITEM_NONE; ++index) {
		if (list_call_barring[index].style == CST_GL_ITEM_1TEXT_ONOFF_PROGRESS) {
			item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
			retv_if(item_data == NULL, NULL);
			item_data->index = index;
			item_data->ugd = ugd;

			ugd->cb_state[index] = CST_SS_STATE_PROGRESS;
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon, (void *)item_data, NULL, list_call_barring[index].flags, __cst_gl_sel_cb, item_data);
			__cst_query_cb_status(item_data);
			cb_item_data[index] = item_data;
			err = tel_register_noti_event(ugd->tapi_handle, TAPI_NOTI_SS_BARRING_STATUS, (__cst_barring_tapi_notify_cb), (void *)item_data);
			if (err != TAPI_API_SUCCESS) {
				DBG("tel_register_noti_event failed: err(%d)", err);
			}
		} else {
			DBG("No style");
			return NULL;
		}
	}
	_cst_create_genlist_separator(genlist, EINA_FALSE);

	evas_object_smart_callback_add(genlist, "realized",
						_cst_gl_realized_cb, (const void *)CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST);
	return genlist;
}


static void __cst_on_click_cb_ime_password_cancel_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cb_ime_password_cancel_btn);
	ret_if(data == NULL);
	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;
	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	ret_if(!item_data);
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
	if (req) {
		free(req);
	}
}

static void __cst_1btn_delete_popup_cancel_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}
static void __cst_change_password_cb_state(TapiHandle *handle, int result, void *data, void *user_data)
{
	ENTER(__cst_change_password_cb_state);
	CstGlItemData_t *item_data = (CstGlItemData_t *)user_data;
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);
	DBG("result from tapi: %d", result);

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	if (result != TAPI_API_SUCCESS) {
		DBG("fail!!");
		ugd->popup = _cst_create_2btn_ok_cancel_popup(ugd->nf, item_data, CST_STR_POP_CANCEL, CST_STR_OK,
				I_(CST_STR_INCORRECT_PASSWORD), __cst_barring_create_password_confirm_popup_cancel_cb, __cst_on_click_change_password_cb);
		elm_object_style_set(ugd->popup, "dimrect");
		evas_object_show(ugd->popup);
	} else {
		DBG("pass!!");
		ugd->popup = _cst_create_1btn_popup(ugd->nf, ugd,
				T_(CST_PASSWORD_CHANGED), T_(CST_STR_CALL_BARRING),
				__cst_1btn_delete_popup_cancel_cb, ugd);
		elm_object_style_set(ugd->popup, "dimrect");
		evas_object_show(ugd->popup);
		ugd->check_state = EINA_FALSE;
	}
}

static void __cst_on_click_cb_ime_confirm_password_done_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cb_ime_confirm_password_done_btn);
	ret_if(!data);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;

	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	ret_if(!item_data);
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);
	ret_if(!ugd->popup);
	ret_if(!ugd->dg_entry);
	Evas_Object *btn_ok = NULL;

	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);
	ret_if(!imf_context);

	char buffer[CST_MAX_PHONE_NUMBER_LEN] = {0, };

	if (__cst_get_cb_input_password(buffer, ugd) <= 0) {
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		if (req) {
			free(req);
		}
		DBG("__cst_get_cb_input_password <= 0");
		return;
	}
	DBG("activate CB pwd=%s", buffer);
	snprintf(req->confirm_password, CST_MAX_PHONE_NUMBER_LEN, "%s", buffer);

	elm_entry_entry_set(entry, "");
	btn_ok =  elm_object_part_content_get(ugd->popup, "button2");
	evas_object_smart_callback_del(btn_ok, "clicked", __cst_on_click_cb_ime_confirm_password_done_btn);

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	if (!g_strcmp0(req->confirm_password, req->new_password)) {
		DBG("new and confirm passwords are same!!!!!!!");
		ugd->popup = _cst_create_processing_popup(ugd->nf);
		elm_object_style_set(ugd->popup, "dimrect");
		evas_object_show(ugd->popup);
		_cst_change_password_ss_request(req, __cst_change_password_cb_state);
	} else {
		DBG("new and confirm passwords are not same!!!!!!");
		ugd->popup = _cst_create_2btn_ok_cancel_popup(ugd->nf, item_data, CST_STR_POP_CANCEL, CST_STR_OK,
				I_(CST_STR_INCORRECT_PASSWORD), __cst_barring_create_password_confirm_popup_cancel_cb, __cst_on_click_change_password_cb);
		elm_object_style_set(ugd->popup, "dimrect");
		evas_object_show(ugd->popup);
	}

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_call_barring_input_panel_event_callback);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}
	evas_object_smart_callback_del(ugd->dg_entry, "changed", _cst_call_barring_entry_changed_cb);
	evas_object_smart_callback_del(ugd->dg_entry, "preedit,changed", _cst_call_barring_entry_changed_cb);

	evas_object_del(ugd->dg_entry);
	ugd->dg_entry = NULL;

	keypad_enabled = EINA_FALSE;

	if (req) {
		free(req);
	}
}

static void __cst_on_click_cb_ime_new_password_done_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cb_ime_new_password_done_btn);
	ret_if(!data);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;
	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	ret_if(!item_data);
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);
	ret_if(!ugd->popup);
	ret_if(!ugd->dg_entry);
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);
	ret_if(!imf_context);
	Evas_Object *btn_ok = NULL;

	char buffer[CST_MAX_PHONE_NUMBER_LEN] = {0, };

	if (__cst_get_cb_input_password(buffer, ugd) <= 0) {
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		if (req) {
			free(req);
		}
		DBG("__cst_get_cb_input_password <= 0");
		return;
	}
	DBG("activate CB pwd=%s", buffer);
	snprintf(req->new_password, CST_MAX_PHONE_NUMBER_LEN, "%s", buffer);

	elm_entry_entry_set(entry, "");
	btn_ok =  elm_object_part_content_get(ugd->popup, "button2");
	evas_object_smart_callback_del(btn_ok, "clicked", __cst_on_click_cb_ime_new_password_done_btn);
	cst_util_domain_translatable_part_text_set(ugd->popup, "title,text", I_(CST_STR_CONFIRM_PASSWORD));
	evas_object_smart_callback_add(btn_ok, "clicked", __cst_on_click_cb_ime_confirm_password_done_btn, (void *)req);
}


static void __cst_on_click_cb_ime_current_password_done_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cb_ime_current_password_done_btn);
	ret_if(!data);

	CallSettingSSReq_t *req = (CallSettingSSReq_t *)data;

	CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
	ret_if(!item_data);
	CstUgData_t *ugd = item_data->ugd;
	ret_if(!ugd);
	ret_if(!ugd->popup);
	ret_if(!ugd->dg_entry);
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);
	ret_if(!imf_context);
	Evas_Object *btn_ok = NULL;

	char buffer[CST_MAX_PHONE_NUMBER_LEN] = {0, };

	if (__cst_get_cb_input_password(buffer, ugd) <= 0) {
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		if (req) {
			free(req);
		}
		DBG("__cst_get_cb_input_password <= 0");
		return;
	}
	DBG("activate CB pwd=%s", buffer);
	snprintf(req->old_password, CST_MAX_PHONE_NUMBER_LEN, "%s", buffer);
	elm_entry_entry_set(entry, "");
	btn_ok =  elm_object_part_content_get(ugd->popup, "button2");
	evas_object_smart_callback_del(btn_ok, "clicked", __cst_on_click_cb_ime_current_password_done_btn);
	cst_util_domain_translatable_part_text_set(ugd->popup, "title,text", I_(CST_STR_NEW_PASSWORD));

	evas_object_smart_callback_add(btn_ok, "clicked", __cst_on_click_cb_ime_new_password_done_btn, (void *)req);
}

static void __cst_call_barr_voice_call_ctxpopup_rotation_changed(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_call_barr_voice_call_ctxpopup_rotation_changed);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;

	if (ctxpopup) {
		elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
		rotation_angle = elm_win_rotation_get((Evas_Object *)ugd->win_main);
		switch (rotation_angle) {
			case 0:
			case 180:
				evas_object_move(ctxpopup, 0, h);
				break;
			case 90:
				evas_object_move(ctxpopup, 0, w);
				break;
			case 270:
				evas_object_move(ctxpopup, h, w);
				break;
			default:
				evas_object_move(ctxpopup, 0, h);
				break;
		}
		evas_object_show(ctxpopup);
	}
}

static void __cst_call_barr_voice_call_dismiss_ctxpopup_more_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_call_barr_voice_call_dismiss_ctxpopup_more_cb);
	ret_if(NULL == data);

	if (ctxpopup) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static void __cst_on_click_change_password_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_change_password_cb);

	if (ctxpopup) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}

	ret_if(NULL == data);
	CallSettingSSReq_t *req = NULL;
	CstGlItemData_t *item_data;
	item_data = (CstGlItemData_t *)malloc(sizeof(CstGlItemData_t));
	CstUgData_t *ugd = (CstUgData_t *)data;
	item_data->ugd = ugd;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	req = (CallSettingSSReq_t *)calloc(1, sizeof(CallSettingSSReq_t));
	if (req == NULL) {
		ERR("Memory allocation failed");
		free(item_data);
		return;
	}
	req->data = item_data;

	memset(ec_item, 0, sizeof(ec_item));
	__cst_barring_create_password_confirm_popup(ugd, CST_STR_CURRENT_PASSWORD,
			__cst_on_click_cb_ime_current_password_done_btn, __cst_on_click_cb_ime_password_cancel_btn,
			(void *)req, ec_item);
}

static void __cst_call_barr_popup_resize_more_ctxpopup_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_call_barr_popup_resize_more_ctxpopup_cb);
	ret_if(NULL == data);
	__cst_call_barr_voice_call_ctxpopup_rotation_changed(data, NULL, NULL);
}

static void __cst_call_barr_popup_hide_more_option(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_call_barr_popup_hide_more_option);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ctxpopup) {
		if (ugd->nf) {
			evas_object_event_callback_del_full(ugd->nf, EVAS_CALLBACK_RESIZE,
					__cst_call_barr_popup_resize_more_ctxpopup_cb, data);
		}

		evas_object_smart_callback_del(ugd->win_main, "rotation,changed",
				__cst_call_barr_voice_call_ctxpopup_rotation_changed);

		evas_object_smart_callback_del(ctxpopup, "dismissed",
				__cst_call_barr_voice_call_dismiss_ctxpopup_more_cb);

		evas_object_event_callback_del_full(ctxpopup, EVAS_CALLBACK_DEL,
				__cst_call_barr_popup_hide_more_option, data);

		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static void __cst_call_barr_create_ctxpopup_more_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	DBG("__cst_call_barr_create_ctxpopup_more_cb");
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;

	if (__cst_barring_check_nw_req_progress(ugd) == EINA_TRUE) {
		DBG("One request pending so returning");
		return;
	}

	if (ctxpopup == NULL) {
		ctxpopup = elm_ctxpopup_add(ugd->win_main);
		elm_object_style_set(ctxpopup, "more/default");
		evas_object_smart_callback_add(ctxpopup, "dismissed",
				__cst_call_barr_voice_call_dismiss_ctxpopup_more_cb, data);
		eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_BACK, ea_ctxpopup_back_cb, NULL);
		eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_MORE, ea_ctxpopup_back_cb, NULL);
		elm_ctxpopup_auto_hide_disabled_set(ctxpopup, EINA_TRUE);
		evas_object_event_callback_add(ctxpopup, EVAS_CALLBACK_DEL,
				__cst_call_barr_popup_hide_more_option, data);
		evas_object_event_callback_add(ugd->nf, EVAS_CALLBACK_RESIZE,
				__cst_call_barr_popup_resize_more_ctxpopup_cb, data);

		elm_ctxpopup_item_append(ctxpopup, T_(CST_STR_CHANGE_PASSWORD), NULL, __cst_on_click_change_password_cb, (void *)ugd);

		elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_UP,
							ELM_CTXPOPUP_DIRECTION_UNKNOWN,
							ELM_CTXPOPUP_DIRECTION_UNKNOWN,
							ELM_CTXPOPUP_DIRECTION_UNKNOWN);
		elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
		evas_object_smart_callback_add(ugd->win_main, "rotation,changed",
				__cst_call_barr_voice_call_ctxpopup_rotation_changed, data);
		rotation_angle = elm_win_rotation_get((Evas_Object *)ugd->win_main);
		switch (rotation_angle) {
			case 0:
			case 180:
				evas_object_move(ctxpopup, (w / 2), h);
				break;
			case 90:
				evas_object_move(ctxpopup,	(h / 2), w);
				break;
			case 270:
				evas_object_move(ctxpopup, (h / 2), w);
				break;
			default:
				evas_object_move(ctxpopup, (w / 2), h);
				break;
		}
		evas_object_show(ctxpopup);
	} else {
		elm_ctxpopup_dismiss(ctxpopup);
	}
}

static void __cst_on_click_voice_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_voice_cb);
	ret_if(NULL == data);

	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *layout;
	Elm_Object_Item *navi_it;

	ugd->call_type = CST_CALLTYPE_VOICE;

	ugd->popup = NULL;

	Evas_Object *genlist = __cst_create_genlist_cb(ugd);

	layout = elm_layout_add(ugd->nf);
	elm_layout_file_set(layout, EDJ_NAME, "toolbar");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "elm.swallow.contents", genlist);
	navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_VOICE_CALL_BARRING),
									  NULL, NULL, layout, NULL);
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_VOICE_CALL_BARRING));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_click_cb_voice_video_back_button, ugd);

	/*create more button*/
	more_btn = elm_button_add(ugd->nf);
	elm_object_style_set(more_btn, "naviframe/more/default");
	evas_object_smart_callback_add(more_btn, "clicked", __cst_call_barr_create_ctxpopup_more_cb, ugd);
	elm_object_item_part_content_set(navi_it, "toolbar_more_btn", more_btn);
}

static void __cst_gl_sel_call_type(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (list_call_type[item_data->index].func) {
		list_call_type[item_data->index].func((void *)item_data->ugd, NULL, obj, item_data->gl_item);
	}
	return;
}

static char *__cst_gl_label_get_call_type(void *data, Evas_Object *obj, const char *part)
{
	retv_if(data == NULL, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (strcmp(part, "elm.text.main.left") == 0) {
		if (list_call_type[item_data->index].str_id != -1) {
			return strdup(T_(list_call_type[item_data->index].str_id));
		}
	}
	return NULL;
}

static Evas_Object *__cst_create_genlist_call_type(void *data)
{
	ENTER(__cst_create_genlist_call_type);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *genlist;
	CstGlItemData_t *item_data;
	int index = 0;

	if (!itc_1text_1icon) {
		itc_1text_1icon = _cst_create_genlist_item_class("1line",
				__cst_gl_label_get_cb,
				__cst_gl_icon_get_cb,
				NULL, __cst_gl_del_cb);
	}

	if (!itc_1text) {
		itc_1text = _cst_create_genlist_item_class("1line",
				__cst_gl_label_get_call_type,
				NULL, NULL, __cst_gl_del_cb);
	}

	genlist = elm_genlist_add(ugd->nf);

	_cst_create_genlist_separator(genlist, EINA_FALSE);
	for (index = 0; list_call_type[index].style != CST_GL_ITEM_NONE; index++) {
		item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(item_data == NULL, NULL);
		item_data->index = index;
		item_data->ugd = ugd;

		item_data->gl_item = elm_genlist_item_append(genlist, itc_1text,
								  (void *)item_data, NULL, ELM_GENLIST_ITEM_NONE,
								  __cst_gl_sel_call_type, (void *)item_data);
	}
	_cst_create_genlist_separator(genlist, EINA_FALSE);

	return genlist;
}

void _cst_call_barring_input_panel_event_callback(void *data, Ecore_IMF_Context *imf_context, int value)
{
	ENTER(_cst_call_barring_input_panel_event_callback);

	ret_if(NULL == data);

	if (EINA_FALSE == keypad_enabled) {
		DBG("keypad_enabled = %d", keypad_enabled);
		keypad_enabled = EINA_TRUE;
		return;
	}

	if (value == ECORE_IMF_INPUT_PANEL_STATE_WILL_SHOW) {
		return;
	}

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_SHOW");
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_HIDE");
	}
}

void _cst_on_click_call_barring(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_call_barring);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(ugd == NULL);
	Elm_Object_Item *navi_it;

	if (_cst_check_flight_mode()) {
		_cst_create_error_popup(CST_ERROR_CHANGE_FLIGHT_MODE);
		return;
	}

	if (!_cst_check_dual_sim_status(ugd)) {
		_cst_create_error_popup(CST_ERROR_INSERT_SIM_CARD);
		return;
	}

	barring_gl = __cst_create_genlist_call_type(ugd);
	evas_object_smart_callback_add(barring_gl, "realized",
						_cst_gl_realized_cb, (const void *)CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST);
	navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_CALL_BARRING),
									  NULL, NULL, barring_gl, NULL);
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_CALL_BARRING));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_click_cb_back_button, ugd);
}

void _cst_destroy_call_barring(void)
{
	ENTER(_cst_destroy_call_barring);

	__cst_destroy_genlist_item_styles();

	if (barring_gl != NULL) {
		barring_gl = NULL;
	}
}
#endif  /* _CALL_SET_BARRING_SUPPORT */

