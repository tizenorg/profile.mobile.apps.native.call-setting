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
#include <vconf.h>
#include <TelSs.h>
#include <string.h>
#include "cst-common.h"
#include "cst-tapi-request.h"
#include "cst-common-string.h"
#include "cst-more-call-setting.h"
#include "cst-widget.h"
#include "cst-util.h"
#include "cst-forwarding.h"
#include "cst-waiting.h"

static Elm_Genlist_Item_Class *itc_1text = NULL;
static Elm_Genlist_Item_Class *itc_2text_expand = NULL;
static Elm_Genlist_Item_Class *itc_sub_expand = NULL;
static Elm_Genlist_Item_Class *itc_help = NULL;
static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_multiline_1icon = NULL;

static Evas_Object *morecallsetting_gl = NULL;
static Evas_Object *caller_id_popup = NULL;

static void __cst_set_genlist_item_styles_more_cst(void);
static void __cst_destroy_genlist_item_styles(void);
static void __cst_on_click_expanded(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_on_click_call_waiting(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static Eina_Bool __cst_on_click_more_call_setting_back_button(void *data, Elm_Object_Item *it);

static CstGlItemDisplayInfo_t list_more_call_setting[] = {
	{1, CST_STR_SHOW_MY_CALLER_ID, ELM_GENLIST_ITEM_TREE, CST_GL_ITEM_EXPANDABLE, __cst_on_click_expanded},
	{1, CST_STR_CALL_FORWARDING, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, _cst_on_click_call_forwarding},
	{1, CST_STR_CALL_WAITING, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF, __cst_on_click_call_waiting},
	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static CstGlItemDisplayInfo_t list_dep2_show_my_number[] = {
	{2, CST_STR_BY_NETWORK, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CLI_BY_NETWORK},
	{2, CST_STR_SHOW, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CLI_SHOW},
	{2, CST_STR_HIDE, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CLI_HIDE},
	{2, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_1text) {
		_cst_destroy_genlist_item_class(itc_1text);
		itc_1text = NULL;
	}
	if (itc_2text_expand) {
		_cst_destroy_genlist_item_class(itc_2text_expand);
		itc_2text_expand = NULL;
	}
	if (itc_sub_expand) {
		_cst_destroy_genlist_item_class(itc_sub_expand);
		itc_sub_expand = NULL;
	}
	if (itc_help) {
		_cst_destroy_genlist_item_class(itc_help);
		itc_help = NULL;
	}
	if (itc_1text_1icon) {
		_cst_destroy_genlist_item_class(itc_1text_1icon);
		itc_1text_1icon = NULL;
	}
	if (itc_multiline_1icon) {
		_cst_destroy_genlist_item_class(itc_multiline_1icon);
		itc_multiline_1icon = NULL;
	}
}

static char *__cst_gl_label_get_more_cst(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if ((strcmp(part, "elm.text.main.left.top") == 0) ||
			(strcmp(part, "elm.text.main.left") == 0) || (strcmp(part, "elm.text.main") == 0) ||
			(strcmp(part, "elm.text") == 0)) {
		if (list_more_call_setting[item_data->index].str_id != -1) {
			return strdup(T_(list_more_call_setting[item_data->index].str_id));
		}
	}

	else if (!strcmp(part, "elm.text.sub.left.bottom") || (strcmp(part, "elm.text.multiline") == 0) ||
			(strcmp(part, "elm.text.sub") == 0)) {
		if (list_more_call_setting[item_data->index].str_id == CST_STR_CALL_WAITING) {
			return strdup(T_(CST_STR_ALERTS_DURING_CALLS));
		}
	}

	return NULL;
}

void __cst_on_update_cw_state(int call_type, int flavour,
				 Eina_Bool cw_state, char *number, int error, int req_action, void *data)
{
	ENTER(__cst_on_update_cw_state);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	ret_if(NULL == item_data->ugd);
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(NULL == item_data->gl_item);
	DBG("req_action=%d(0: Activate, 1:Deactivate, 2:Query)", req_action);
	DBG("cw_state=%d(0: off, 1:on), error=%d", cw_state, error);

	if (error == CST_ERROR_NONE) {
		if (cw_state == EINA_TRUE) {
			ugd->cw_state = CST_SS_STATE_ON;
		} else {
			ugd->cw_state = CST_SS_STATE_OFF;
		}
		elm_object_item_disabled_set(item_data->gl_item, EINA_FALSE);
	} else {
		if (error == CST_ERROR_FDN_MODE_ACTIVE) {
			if (ugd->popup) {
				evas_object_del(ugd->popup);
				ugd->popup = NULL;
			}
			ugd->popup = (Evas_Object *)_cst_create_error_popup_with_ok_btn(ugd,
					error);
			evas_object_show(ugd->popup);
			elm_object_item_disabled_set(item_data->gl_item, EINA_TRUE);
		}

		ugd->cw_state = CST_SS_STATE_OFF;
		if (req_action != CST_ACTION_QUERY) {
			_cst_create_error_popup(error);
		}
	}
	elm_genlist_item_update(item_data->gl_item);
}

static void __cst_on_click_call_waiting(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data ;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(NULL == ugd);
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (ugd->cw_state == CST_SS_STATE_PROGRESS || ugd->cw_state == CST_SS_STATE_PROGRESS_INIT) {
		return;
	}

	Eina_Bool check_state = elm_check_state_get(item_data->eo_check);

	int action = check_state ? CST_ACTION_DEACTIVATE : CST_ACTION_ACTIVATE;
	ugd->cw_state = CST_SS_STATE_PROGRESS;
	elm_genlist_item_update(item_data->gl_item);
	_cst_add_ss_request(&ugd->req_queue, action,
						CST_CALLTYPE_VOICE, CST_SSTYPE_CW, EINA_FALSE, NULL,
						__cst_on_update_cw_state, item_data, -1, ugd);
}

static void __cst_on_changed_cw_check(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(NULL == ugd);
	int check_state = elm_check_state_get(obj);

	if (elm_object_item_disabled_get(item_data->gl_item) == EINA_TRUE) {
		elm_check_state_set(obj, EINA_FALSE);
		return;
	}

	int action = check_state ? CST_ACTION_ACTIVATE : CST_ACTION_DEACTIVATE;
	ugd->cw_state = CST_SS_STATE_PROGRESS;
	elm_genlist_item_update(item_data->gl_item);
	_cst_add_ss_request(&ugd->req_queue, action,
						CST_CALLTYPE_VOICE, CST_SSTYPE_CW, EINA_FALSE, NULL,
						__cst_on_update_cw_state, item_data, -1, ugd);

}

static void __cst_waiting_tapi_notify_cb(TapiHandle *handle, const char *noti_id, void *data, void *user_data)
{
	ENTER(__cst_waiting_tapi_notify_cb);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)user_data;
	ret_if(NULL == ugd);
	CstGlItemData_t *item_data = ugd->cw_gl_item_data;
	ret_if(NULL == item_data);

	if (ugd->cw_state == CST_SS_STATE_PROGRESS_INIT) {
		DBG("The call waiting state initial state query is still in progress");
		return;
	}

	TelSsWaitingNoti_t *cw_status_data = (TelSsWaitingNoti_t *)data;
	TelSsStatus_t cw_status = cw_status_data->record[0].Status;
	DBG("status = %d", cw_status_data->record[0].Status);

	if (cw_status == TAPI_SS_STATUS_ACTIVE) {
		ugd->cw_state = CST_SS_STATE_ON;
	} else {
		ugd->cw_state = CST_SS_STATE_OFF;
	}
	elm_genlist_item_update(item_data->gl_item);
}

static Evas_Object *__cst_gl_icon_get_more_cst(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
#ifdef _TIZEN_LITE_CODE
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	retv_if(NULL == ugd, NULL);
#endif/*Tizen Lite Code*/
	if (!strcmp(part, "elm.icon.right") || !strcmp(part, "elm.icon")
		|| !strcmp(part, "elm.swallow.end")) {
		if (list_more_call_setting[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF) {
			if (CST_STR_CALL_WAITING == list_more_call_setting[item_data->index].str_id) {
				Evas_Object *icon = NULL;
				Eina_Bool check_state = EINA_FALSE;
				switch (ugd->cw_state) {
				case CST_SS_STATE_ON:
				case CST_SS_STATE_OFF:
					if (ugd->cw_state == CST_SS_STATE_ON) {
						check_state = EINA_TRUE;
					}
					item_data->eo_check = icon = _cst_create_onoff_button(obj, check_state,
												 __cst_on_changed_cw_check, item_data);
					elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DEFAULT);
					break;
				case CST_SS_STATE_PROGRESS:
				case CST_SS_STATE_PROGRESS_INIT:
					icon = _cst_create_progressbar(obj);
					item_data->eo_check = NULL;
					elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
					elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_NONE);
					break;
				default:
					return NULL;
				}
				return icon;
			}
		}

		return item_data->eo_check;
	}
	return NULL;
}

static void __cst_gl_del_more_cst(void *data, Evas_Object *obj)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

static void __cst_gl_sel_more_cst(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (list_more_call_setting[item_data->index].func) {
		if (list_more_call_setting[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF) {
			list_more_call_setting[item_data->index].func((void *)item_data, NULL, obj, item_data->gl_item);
		} else if (list_more_call_setting[item_data->index].str_id == CST_STR_VOICE_MAIL_NUMBER) {
			list_more_call_setting[item_data->index].func((void *)ugd, NULL, obj, item_data->gl_item);
		} else {
			list_more_call_setting[item_data->index].func((void *)item_data, NULL, obj, item_data->gl_item);
		}
	}
	return;
}

static char *__cst_gl_label_get_expandable(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
        char *buff = NULL;
	if (!strcmp(part, "elm.text")) {
		if (list_more_call_setting[item_data->index].str_id != -1) {
			return strdup(T_(list_more_call_setting[item_data->index].str_id));
		}
	} else if (!strcmp(part, "elm.text.sub")) {
		int value = 0;
		if (CST_STR_SHOW_MY_CALLER_ID == list_more_call_setting[item_data->index].str_id) {
#ifdef _TIZEN_LITE_CODE
			CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
			Eina_Bool result;
			if (ugd->sel_sim == CST_SELECTED_SIM1) {
				result = _cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE, &value);
			} else {
				result = _cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE2, &value);
			}
#else
			Eina_Bool result = _cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE, &value);
#endif/*Tizen Lite Code*/

			if (result == EINA_FALSE) {
				ERR("_cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE) failed!!!");
				return NULL;
			}

			if (list_dep2_show_my_number[value].str_id != -1) {
				buff = g_strdup_printf("<color=#%02x%02x%02x%02x>%s</color>",  61, 185, 204, 255, T_(list_dep2_show_my_number[value].str_id));
				return buff;
			}
		}
	}
	return NULL;
}

static void __cst_more_call_setting_gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	CstGlItemData_t *item_data = elm_object_item_data_get(it);
#ifdef _CALL_SET_TTS_SUPPORT
	if (NULL != item_data && list_more_call_setting[item_data->index].str_id == CST_STR_VOICE_MAIL_NUMBER) {
		elm_access_info_cb_set(elm_object_item_access_object_get(it),
				ELM_ACCESS_INFO, _cst_util_2item_number_access_info_cb, it);
	}
	if (NULL != item_data && list_more_call_setting[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF) {
		elm_access_info_cb_set(elm_object_item_access_object_get(it),
				ELM_ACCESS_INFO, _cst_util_on_off_btn_access_info_cb, it);
	}

#endif /*_CALL_SET_TTS_SUPPORT*/
	Elm_Object_Item *nxt_item = NULL;
	Elm_Object_Item *last_item = NULL;
	int i;

	nxt_item = elm_genlist_first_item_get(obj);

	while (nxt_item != elm_genlist_last_item_get(obj)) {
		item_data = (CstGlItemData_t *)elm_object_item_data_get(nxt_item);
		while (item_data == NULL || item_data->index == -1 || item_data->style == CST_GL_ITEM_HELP_TEXT) {
			nxt_item = elm_genlist_item_next_get(nxt_item);
			if (nxt_item == NULL) {
				DBG("Genlist end");
				return;
			}
			item_data = (CstGlItemData_t *)elm_object_item_data_get(nxt_item);
		}

		last_item = nxt_item;

		item_data = (CstGlItemData_t *)elm_object_item_data_get(last_item);
		while (item_data != NULL && item_data->index != -1 && item_data->style != CST_GL_ITEM_HELP_TEXT) {
			last_item = elm_genlist_item_next_get(last_item);
			item_data = (CstGlItemData_t *)elm_object_item_data_get(last_item);
		}
		last_item = elm_genlist_item_prev_get(last_item);

		if (nxt_item == last_item) {
			elm_object_item_signal_emit(nxt_item, "elm,state,default", "");
		} else {
			elm_object_item_signal_emit(nxt_item, "elm,state,top", "");

			elm_object_item_signal_emit(last_item, "elm,state,bottom", "");

			nxt_item = elm_genlist_item_next_get(nxt_item);

			for (i = 1; nxt_item != last_item; i++) {
				elm_object_item_signal_emit(nxt_item, "elm,state,center", "");
				nxt_item = elm_genlist_item_next_get(nxt_item);
			}
		}
		nxt_item = elm_genlist_item_next_get(last_item);
	}
}

static void __cst_gl_sel_show_my_number(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
#ifdef _TIZEN_LITE_CODE

	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
	if (ugd->sel_sim == CST_SELECTED_SIM1) {
		_cst_vconf_set_int(VCONFKEY_TELEPHONY_SS_CLI_STATE,
						   (long)list_dep2_show_my_number[item_data->index].func);
	} else {
		_cst_vconf_set_int(VCONFKEY_TELEPHONY_SS_CLI_STATE2,
						   (long)list_dep2_show_my_number[item_data->index].func);
	}
		evas_object_del(caller_id_popup);
		caller_id_popup = NULL;
		ugd->rdg_show_my_number = NULL;
#else
	_cst_vconf_set_int(VCONFKEY_TELEPHONY_SS_CLI_STATE,
					   (int)list_dep2_show_my_number[item_data->index].func);
#endif/*Tizen Lite Code*/
	return;
}

#ifdef _TIZEN_LITE_CODE
static void __cst_show_my_number_radio_btn_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	__cst_gl_sel_show_my_number(data, obj, event_info);
}
#endif

static char *__cst_gl_label_get_sub_exp(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstGlItemData_t *parent_item_data = (CstGlItemData_t *)elm_object_item_data_get(item_data->parent_gl_item);
	if (strcmp(part, "elm.text") == 0) {
		if (CST_STR_SHOW_MY_CALLER_ID == list_more_call_setting[parent_item_data->index].str_id) {
			return strdup(T_(list_dep2_show_my_number[item_data->index].str_id));
		} else {
			DBG("Unknown parent");
			return NULL;
		}
	}
	return NULL;
}

static Evas_Object *__cst_gl_icon_get_sub_exp(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	retv_if(NULL == ugd, NULL);
	CstGlItemData_t *parent_item_data = (CstGlItemData_t *)elm_object_item_data_get(item_data->parent_gl_item);

	if (!strcmp(part, "elm.swallow.end")) {
		Evas_Object *radio;
		if (CST_STR_SHOW_MY_CALLER_ID == list_more_call_setting[parent_item_data->index].str_id) {
			if (NULL == ugd->rdg_show_my_number) {
				ugd->rdg_show_my_number = elm_radio_add(obj);
				elm_radio_state_value_set(ugd->rdg_show_my_number, -1);
				elm_radio_value_set(ugd->rdg_show_my_number, -1);
			}
#ifdef _TIZEN_LITE_CODE
			if (ugd->sel_sim == CST_SELECTED_SIM1) {
				radio = _cst_create_radio_icon(obj, ugd->rdg_show_my_number,
						(long)list_dep2_show_my_number[item_data->index].func, 0,
						VCONFKEY_TELEPHONY_SS_CLI_STATE);
			} else {
				radio = _cst_create_radio_icon(obj, ugd->rdg_show_my_number,
						(long)list_dep2_show_my_number[item_data->index].func, 0,
						VCONFKEY_TELEPHONY_SS_CLI_STATE2);
			}
			evas_object_event_callback_add(radio, EVAS_CALLBACK_MOUSE_UP,
				__cst_show_my_number_radio_btn_mouse_up_cb, item_data);
#else
			radio = _cst_create_radio_icon(obj, ugd->rdg_show_my_number,
										   (int)list_dep2_show_my_number[item_data->index].func, 0,
										   VCONFKEY_TELEPHONY_SS_CLI_STATE);
			evas_object_smart_callback_add(radio, "changed",
					__cst_gl_sel_show_my_number, item_data);
#endif/*Tizen Lite Code*/
		} else {
			DBG("Unknown parent");
			return NULL;
		}
#ifdef _CALL_SET_TTS_SUPPORT
		/* Unregister the access object */
		if (elm_config_access_get() && radio) {
			elm_access_object_unregister(radio);
		}
#endif
		return radio;
	}
	return NULL;
}

static void __cst_more_caller_id_changed_cb(keynode_t *node, void *data)
{
	ENTER(__cst_more_caller_id_changed_cb);
	ret_if(NULL == data);
	int value;
	CstUgData_t *ugd = (CstUgData_t *)data;
#ifdef _TIZEN_LITE_CODE
	Eina_Bool result;
	if (ugd->sel_sim == CST_SELECTED_SIM1) {
		result = _cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE, &value);
	} else {
		result = _cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE2, &value);
	}
#else
	Eina_Bool result = _cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE, &value);
#endif/*Tizen Lite*/
	DBG("value = %d", value);
	if (result == EINA_FALSE) {
		ERR("_cst_vconf_get_int(VCONFKEY_TELEPHONY_SS_CLI_STATE) failed!!!");
		return;
	}

	if (ugd->rdg_show_my_number) {
		elm_radio_value_set(ugd->rdg_show_my_number, value);
	}
}

static Eina_Bool __cst_on_click_more_call_setting_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_more_call_setting_back_button);
	CstUgData_t *ugd = (CstUgData_t *)data;
	retv_if(ugd == NULL, EINA_TRUE);
	_cst_cancel_all_ss_request(ugd);
	tel_deregister_noti_event(ugd->sim1_tapi_handle, TAPI_NOTI_SS_WAITING_STATUS);
	tel_deregister_noti_event(ugd->sim2_tapi_handle, TAPI_NOTI_SS_WAITING_STATUS);

	_cst_destroy_more_call_setting();

	if (ugd->ug_req_type == CST_UG_REQ_ADDITIONAL_SETTINGS) {
		ug_destroy_me(ugd->ug);
	}

	return EINA_TRUE;
}

static void
__cst_popup_block_clicked_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
	evas_object_del(caller_id_popup);
	caller_id_popup = NULL;
}

static void __cst_gl_exp(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_exp);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(ugd == NULL);
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	ret_if(NULL == it);
	CstGlItemData_t *parent_item_data1 = (CstGlItemData_t *)elm_object_item_data_get(it);
	ret_if(NULL == parent_item_data1);

	if (!_cst_check_dual_sim_status(ugd)) {
		_cst_create_error_popup(CST_ERROR_INSERT_SIM_CARD);
		return;
	}

	CstGlItemData_t *sub_item_data;
	Evas_Object *genlist = NULL;
	Evas_Object *box = NULL;
	int i = 0;
	caller_id_popup = elm_popup_add(ugd->nf);
	eext_object_event_callback_add(caller_id_popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(caller_id_popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(caller_id_popup, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(caller_id_popup, "block,clicked", __cst_popup_block_clicked_cb, ugd->nf);
	cst_util_domain_translatable_part_text_set(caller_id_popup, "title,text", I_(CST_STR_SHOW_MY_CALLER_ID));
	/* box */

	box = elm_box_add(caller_id_popup);
	elm_object_style_set(box, "default");
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	/* genlist */
	genlist = elm_genlist_add(box);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);
	elm_box_pack_end(box, genlist);
	evas_object_show(genlist);
	/* radio */

	if (CST_STR_SHOW_MY_CALLER_ID == list_more_call_setting[parent_item_data1->index].str_id) {
		ugd->rdg_show_my_number = NULL;
		ugd->rdg_show_my_number = elm_radio_add(genlist);
		elm_radio_state_value_set(ugd->rdg_show_my_number, -1);
		elm_radio_value_set(ugd->rdg_show_my_number, -1);
		for (i = 0; list_dep2_show_my_number[i].style != CST_GL_ITEM_NONE; ++i) {
			sub_item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
			ret_if(sub_item_data == NULL);
			sub_item_data->index = i;
			sub_item_data->ugd = ugd;
			sub_item_data->parent_gl_item = it;
			sub_item_data->style = list_dep2_show_my_number[i].style;
			sub_item_data->gl_item = elm_genlist_item_append(genlist, itc_sub_expand,
									(void *)sub_item_data, NULL,
									list_dep2_show_my_number[i].flags,
									__cst_gl_sel_show_my_number, sub_item_data);
		}
	elm_object_content_set(caller_id_popup, box);
	evas_object_show(box);
	evas_object_show(caller_id_popup);

		vconf_notify_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE,
				__cst_more_caller_id_changed_cb, ugd);
#ifdef _TIZEN_LITE_CODE
		vconf_notify_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE2,
				__cst_more_caller_id_changed_cb, ugd);
#endif/*Tizen Lite Code*/
	}
}

static void __cst_on_click_expanded(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_expanded);
	ret_if(NULL == data);
	__cst_gl_exp(data, obj, event_info);
}

static void __cst_set_genlist_item_styles_more_cst(void)
{
	ENTER(__cst_set_genlist_item_styles_more_cst);
	if (!itc_1text) {
		itc_1text = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_more_cst, NULL, NULL,
				__cst_gl_del_more_cst);
	}
	if (!itc_2text_expand) {
		itc_2text_expand = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_expandable, NULL, NULL,
				__cst_gl_del_more_cst);
	}
	if (!itc_sub_expand) {
		itc_sub_expand = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_sub_exp,
				__cst_gl_icon_get_sub_exp,
				NULL, __cst_gl_del_more_cst);
	}
	if (!itc_help) {
		itc_help = _cst_create_genlist_item_class("multiline",
				__cst_gl_label_get_more_cst, NULL, NULL,
				__cst_gl_del_more_cst);
	}

	if (!itc_1text_1icon) {
		itc_1text_1icon = _cst_create_genlist_item_class("1line",
				__cst_gl_label_get_more_cst,
				__cst_gl_icon_get_more_cst,
				NULL, __cst_gl_del_more_cst);
	}

	if (!itc_multiline_1icon) {
		itc_multiline_1icon = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_more_cst,
				__cst_gl_icon_get_more_cst,
				NULL, __cst_gl_del_more_cst);
	}
}

static Evas_Object *__cst_create_genlist_more_cst(void *data)
{
	ENTER(__cst_create_genlist_more_cst);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	CstGlItemData_t *item_data;
	Evas_Object *genlist;
	int i;

	genlist = elm_genlist_add(ugd->nf);
	retv_if(NULL == genlist, NULL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	for (i = 0; list_more_call_setting[i].style != CST_GL_ITEM_NONE; ++i) {
		item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(NULL == item_data, NULL);

		item_data->index = i;
		item_data->ugd = ugd;
		item_data->style = list_more_call_setting[i].style;
		if (list_more_call_setting[i].style == CST_GL_ITEM_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text,
								 (void *)item_data, NULL, list_more_call_setting[i].flags,
								 __cst_gl_sel_more_cst, item_data);
			if (list_more_call_setting[i].str_id == CST_STR_CALL_FORWARDING) {
				_cst_flight_mode_genlist_item_disable(item_data->gl_item);
				_cst_util_system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_call_forwarding,
						item_data->gl_item);
			}
			if (list_more_call_setting[i].str_id == CST_STR_CALL_WAITING) {
				_cst_flight_mode_genlist_item_disable(item_data->gl_item);
				_cst_util_system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_call_waiting,
						item_data->gl_item);
			}
			if (list_more_call_setting[i].str_id == CST_STR_VOICE_MAIL_NUMBER) {
				_cst_flight_mode_genlist_item_disable(item_data->gl_item);
				_cst_util_system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_voice_mail_number,
						item_data->gl_item);
				ugd->vm_gl_item = item_data->gl_item;
			}
		} else if (list_more_call_setting[i].style == CST_GL_ITEM_EXPANDABLE) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_2text_expand,
								 (void *)item_data, NULL, list_more_call_setting[i].flags,
								 __cst_gl_sel_more_cst, item_data);
			if (CST_STR_SHOW_MY_CALLER_ID == list_more_call_setting[i].str_id) {
				vconf_notify_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE,
						_cst_vconfkey_change_notify_cb, item_data->gl_item);
#ifdef _TIZEN_LITE_CODE
				vconf_notify_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE2,
						_cst_vconfkey_change_notify_cb, item_data->gl_item);
#endif/*Tizen Lite Code*/
			} else {
				if (!_cst_check_dual_sim_status(ugd)) {
					DBG("No SIMs inserted!!!");
					elm_object_item_disabled_set(item_data->gl_item, EINA_TRUE);
				}
			}
		} else if (list_more_call_setting[i].style == CST_GL_ITEM_HELP_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_help,
								 (void *)item_data, NULL, list_more_call_setting[i].flags,
								 NULL, NULL);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			if (list_more_call_setting[i+1].style != CST_GL_ITEM_NONE) {
				_cst_create_genlist_separator(genlist, EINA_FALSE);
			}
		} else if (list_more_call_setting[i].style == CST_GL_ITEM_1TEXT_ONOFF) {
			if (CST_STR_CALL_WAITING == list_more_call_setting[item_data->index].str_id) {
				item_data->gl_item = elm_genlist_item_append(genlist, itc_multiline_1icon,
								 (void *)item_data, NULL, list_more_call_setting[i].flags,
								 __cst_gl_sel_more_cst, item_data);
				ugd->cw_gl_item_data = item_data;
			} else {
				item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon,
								 (void *)item_data, NULL, list_more_call_setting[i].flags,
								 __cst_gl_sel_more_cst, item_data);
			}
		} else {
			DBG("No style");
			free(item_data);
			item_data = NULL;
			return NULL;
		}
	}
	_cst_create_genlist_separator(genlist, EINA_FALSE);

	return genlist;
}

#ifdef _TIZEN_LITE_CODE
static void __cst_more_settings_sim1_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->sel_sim == CST_SELECTED_SIM1) {
		DBG("SIM1 already selected");
		return;
	}
	DBG("SIM1 not selected");

	ugd->sel_sim = CST_SELECTED_SIM1;
	_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM1);
	elm_genlist_realized_items_update(morecallsetting_gl);
	elm_object_item_signal_emit(ugd->sim2_btn, "unselected", "");
	if (ugd->cw_gl_item_data) {
		_cst_cancel_all_ss_request(ugd);
		ugd->cw_state = CST_SS_STATE_PROGRESS_INIT;
		_cst_add_ss_request(&ugd->req_queue, CST_ACTION_QUERY,
							CST_CALLTYPE_VOICE, CST_SSTYPE_CW, EINA_FALSE, NULL,
							__cst_on_update_cw_state, ugd->cw_gl_item_data, -1, ugd);
	}
}

static void __cst_more_settings_sim2_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->sel_sim == CST_SELECTED_SIM2) {
		DBG("SIM2 already selected");
		return;
	}
	DBG("SIM2 not selected");

	ugd->sel_sim = CST_SELECTED_SIM2;
	_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM2);
	elm_genlist_realized_items_update(morecallsetting_gl);
	elm_object_item_signal_emit(ugd->sim1_btn, "unselected", "");
	if (ugd->cw_gl_item_data) {
		_cst_cancel_all_ss_request(ugd);
		ugd->cw_state = CST_SS_STATE_PROGRESS_INIT;
		_cst_add_ss_request(&ugd->req_queue, CST_ACTION_QUERY,
							CST_CALLTYPE_VOICE, CST_SSTYPE_CW, EINA_FALSE, NULL,
							__cst_on_update_cw_state, ugd->cw_gl_item_data, -1, ugd);
	}
}
#endif /* _TIZEN_LITE_CODE */

void _cst_on_click_more_call_setting(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_more_call_setting);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Eina_Bool is_sim1_present = EINA_FALSE;
	Eina_Bool is_sim2_present = EINA_FALSE;
	TapiResult_t err = TAPI_API_SUCCESS;
	Evas_Smart_Cb sim1_cb = NULL;
	Evas_Smart_Cb sim2_cb = NULL;
	Evas_Object *back_btn = NULL;

	is_sim1_present = _cst_is_sim_present_by_simslot(data, CST_SELECTED_SIM1);
	is_sim2_present = _cst_is_sim_present_by_simslot(data, CST_SELECTED_SIM2);

	if (!is_sim1_present && !is_sim2_present) { /* None of the SIM's inserted */
		_cst_create_error_popup(CST_ERROR_INSERT_SIM_CARD);
		return;
	}
	if (is_sim1_present) {
		err = tel_register_noti_event(ugd->sim1_tapi_handle, TAPI_NOTI_SS_WAITING_STATUS, __cst_waiting_tapi_notify_cb, (void *)ugd);
		if (err != TAPI_API_SUCCESS) {
			DBG("tel_register_noti_event failed: err(%d)", err);
		}
		sim1_cb = __cst_more_settings_sim1_sel_cb;
		ugd->sel_sim = CST_SELECTED_SIM1;
	}
	if (is_sim2_present) {
		err = tel_register_noti_event(ugd->sim2_tapi_handle, TAPI_NOTI_SS_WAITING_STATUS, __cst_waiting_tapi_notify_cb, (void *)ugd);
		if (err != TAPI_API_SUCCESS) {
			DBG("tel_register_noti_event failed: err(%d)", err);
		}
		sim2_cb = __cst_more_settings_sim2_sel_cb;
		if (!is_sim1_present) {
			ugd->sel_sim = CST_SELECTED_SIM2;
		}
	}

	__cst_set_genlist_item_styles_more_cst();
	morecallsetting_gl = __cst_create_genlist_more_cst(ugd);

	evas_object_smart_callback_add(morecallsetting_gl, "realized",
				__cst_more_call_setting_gl_realized_cb, NULL);

	back_btn = _cst_util_navi_back_btn_create(ugd->nf);

#ifdef _TIZEN_LITE_CODE
	ugd->more_cst_navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_MORE_CALL_SETTINGS),
			back_btn, NULL, morecallsetting_gl, "tabbar");

	_cst_create_dual_sim_tabbar(ugd->more_cst_navi_it, sim1_cb, sim2_cb, ugd);
#else
	ugd->more_cst_navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_MORE_CALL_SETTINGS),
			back_btn, NULL, morecallsetting_gl, NULL);

#endif /* _TIZEN_LITE_CODE */

	if (ugd->cw_gl_item_data) {
		ugd->cw_state = CST_SS_STATE_PROGRESS;
		_cst_add_ss_request(&ugd->req_queue, CST_ACTION_QUERY,
							CST_CALLTYPE_VOICE, CST_SSTYPE_CW, EINA_FALSE, NULL,
							__cst_on_update_cw_state, ugd->cw_gl_item_data, -1, ugd);
	}

	cst_util_item_domain_text_translatable_set(ugd->more_cst_navi_it, I_(CST_STR_MORE_CALL_SETTINGS));

	elm_naviframe_item_pop_cb_set(ugd->more_cst_navi_it, __cst_on_click_more_call_setting_back_button, (void *)ugd);

	LEAVE();
}

void _cst_destroy_more_call_setting(void)
{
	ENTER(_cst_destroy_more_call_setting);

	_cst_flight_mode_unlisten_setting_change_for_more_call_setting();

	__cst_destroy_genlist_item_styles();

	if (morecallsetting_gl != NULL) {
		morecallsetting_gl = NULL;
	}

	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE, _cst_vconfkey_change_notify_cb);
	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE, __cst_more_caller_id_changed_cb);
#ifdef _TIZEN_LITE_CODE
	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE2, __cst_more_caller_id_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SS_CLI_STATE2, _cst_vconfkey_change_notify_cb);
#endif/*Tizen Lite Code*/
}
