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

#include "cst-debug.h"
#include "cst-util.h"
#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-reject-msg.h"
#include "cst-widget.h"
#include "cst-call-setting.h"

#include <Evas.h>
#include <vconf.h>
#include <msg_types.h>
#include <msg.h>
#include <contacts.h>
#include <metadata_extractor.h>
#include <telephony.h>

#define	POPUP_LIST_HD_W 610
#define	POPUP_LIST_ITEM_HD_H 114

#define CST_CT_APPCONTROL_MIME_CONTACT      "application/vnd.tizen.contact"
#define CST_CT_APPCONTROL_DATA_TYPE         "http://tizen.org/appcontrol/data/type"
#define CST_CT_APPCONTROL_DATA_PHONE        "phone"

CST_MODULE_EXPORT char *cst_reject_msg_get(int index, gboolean b_parsing)
{
    ENTER(cst_reject_msg_get);
    bindtextdomain(UGNAME, CST_LOCALE);

    char *message = _cst_get_reject_message(index, b_parsing, EINA_TRUE);
    return message;
}

gboolean _cst_core_util_strcpy(char *pbuffer, int buf_count, const char *pstring)
{
	DBG("entered _cst_core_util_strcpy");
	retv_if(pbuffer == NULL, -1);
	retv_if(pstring == NULL, -1);
	if (buf_count == 0) {
		DBG("buf_count is zero!!");
		return FALSE;
	}
#ifdef _NO_USE_STRCPY_
	if ((buf_count - 1) >= (int)strlen(pstring)) {
		strcpy(pbuffer, pstring);
	} else {
		DBG("pbuffer size is smaller than pstring!!");
		strncpy(pbuffer, pstring, (buf_count - 1));
		pbuffer[buf_count - 1] = '\0';
	}
#else
	strncpy(pbuffer, pstring, (buf_count - 1));
	pbuffer[buf_count - 1] = '\0';
#endif
	return TRUE;
}

void cst_util_domain_translatable_text_set(Evas_Object *obj, const char* text)
{
	char *domain = UGNAME;
	if (text && strstr(text, "IDS_COM") && _cst_text_id_is_common(text))
		domain = "sys_string";

	elm_object_domain_translatable_text_set(obj, domain, text);
}

void cst_util_domain_translatable_part_text_set(Evas_Object *obj, const char* part, const char* text)
{
	char *domain = UGNAME;
	if (text && strstr(text, "IDS_COM") && _cst_text_id_is_common(text))
		domain = "sys_string";

	elm_object_domain_translatable_part_text_set(obj, part, domain, text);
}

void cst_util_item_domain_translatable_text_set(Elm_Object_Item *it, const char* text)
{
	char *domain = UGNAME;
	if (text && strstr(text, "IDS_COM") && _cst_text_id_is_common(text))
		domain = "sys_string";

	elm_object_item_domain_translatable_text_set(it, domain, text);
}

void cst_util_item_domain_translatable_part_text_set(Elm_Object_Item *it, const char* part, const char* text)
{
	char *domain = UGNAME;
	if (text && strstr(text, "IDS_COM") && _cst_text_id_is_common(text))
		domain = "sys_string";

	elm_object_item_domain_translatable_part_text_set(it, part, domain, text);
}

void cst_util_item_domain_text_translatable_set(Elm_Object_Item *it, const char* text)
{
	char *domain = UGNAME;
	if (text && strstr(text, "IDS_COM") && _cst_text_id_is_common(text))
		domain = "sys_string";

	elm_object_item_domain_text_translatable_set(it, domain, EINA_TRUE);
}

Eina_Bool _cst_vconf_get_bool(const char *in_key, int *boolval)
{
	if ((vconf_get_bool(in_key, boolval)) < 0) {
		ERR("vconf get error : %s", in_key);
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

Eina_Bool _cst_vconf_get_int(const char *in_key, int *intval)
{
	if ((vconf_get_int(in_key, intval)) < 0) {
		ERR("vconf get error : %s", in_key);
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

char *_cst_vconf_get_str(const char *in_key)
{
	char *result = NULL;
	result = vconf_get_str(in_key);
	if (result == NULL) {
		ERR("vconf get error : %s", in_key);
	}

	return result;
}

void _cst_vconf_set_bool(const char *in_key, const int boolval)
{
	if (vconf_set_bool(in_key, boolval) < 0) {
		ERR("vconf set error : %s", in_key);
	} else {
		DBG("vconf set : %d(%s)", boolval, in_key);
	}
}

void _cst_vconf_set_int(const char *in_key, const int intval)
{
	if (vconf_set_int(in_key, intval) < 0) {
		ERR("vconf set error : %s", in_key);
	} else {
		DBG("vconf set : %d(%s)", intval, in_key);
	}
}

void _cst_vconf_set_str(const char *in_key, const char *strval)
{
	if (vconf_set_str(in_key, strval) < 0) {
		ERR("vconf set error : %s", in_key);
	} else {
		DBG("vconf set : %s(%s)", strval, in_key);
	}
}

Eina_Bool _cst_check_flight_mode(void)
{
	ENTER(_cst_check_flight_mode);
	bool flight_mode = false;

	int ret = system_settings_get_value_bool(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, &flight_mode);
	if (SYSTEM_SETTINGS_ERROR_NONE != ret) {
		ERR("system_settings_get_value_bool error : %d", ret);
	}

	return (Eina_Bool)flight_mode;
}

/*
 * This API is used to find the SIM status in Dual SIM Devices
 */
Eina_Bool _cst_check_dual_sim_status(CstUgData_t *ugd)
{
	ENTER(_cst_check_dual_sim_status);

	Eina_Bool sim_avail = EINA_FALSE;
	telephony_handle_list_s handle_list;

	int res = telephony_init(&handle_list);
	if (TELEPHONY_ERROR_NONE == res) {
		int sel_sim = ugd->sel_sim;

		if (CST_SELECTED_SIM1 <= sel_sim && sel_sim < handle_list.count) {
			telephony_sim_state_e sim_state = TELEPHONY_SIM_STATE_UNAVAILABLE;

			res = telephony_sim_get_state(handle_list.handle[sel_sim], &sim_state);
			if (TELEPHONY_ERROR_NONE == res && TELEPHONY_SIM_STATE_AVAILABLE == sim_state) {
				sim_avail = EINA_TRUE;
			} else {
				ERR("Cannot get sim state. Error: %d", res);
			}
		} else {
			ERR("Wrong sim selected");
		}

		res = telephony_deinit(&handle_list);
		if (TELEPHONY_ERROR_NONE != res) {
			ERR("telephony_deinit fail. Error: %d", res);
		}
	} else {
		ERR("telephony_init fail. Error: %d", res);
	}

	return sim_avail;
}

void _cst_flight_mode_genlist_item_disable(void *data)
{
	ENTER(_cst_flight_mode_genlist_item_disable);
	ret_if(NULL == data);
	Elm_Object_Item *gl_item = (Elm_Object_Item *)data;

	if (_cst_check_flight_mode()) {
		elm_object_item_disabled_set(gl_item, EINA_TRUE);
	} else {
		elm_object_item_disabled_set(gl_item, EINA_FALSE);
	}
}

void _cst_flight_mode_changed_cb_for_call_forwarding(system_settings_key_e key, void *data)
{
	ENTER(_cst_flight_mode_changed_cb_for_call_forwarding);
	ret_if(NULL == data);
	Elm_Object_Item *gl_item = (Elm_Object_Item *)data;

	_cst_flight_mode_genlist_item_disable(gl_item);

	elm_genlist_item_update(gl_item);
}

void _cst_vconfkey_change_notify_cb(keynode_t *node, void *data)
{
	ENTER(_cst_vconfkey_change_notify_cb);
	ret_if(NULL == data);
	Elm_Object_Item *gl_item = (Elm_Object_Item *)data;

	elm_genlist_item_update(gl_item);
}

void _cst_flight_mode_changed_cb_for_voice_mail_number(system_settings_key_e key, void *data)
{
	ENTER(_cst_flight_mode_changed_cb_for_voice_mail_number);
	ret_if(NULL == data);
	Elm_Object_Item *gl_item = (Elm_Object_Item *)data;

	_cst_flight_mode_genlist_item_disable(gl_item);

	elm_genlist_item_update(gl_item);
}

void _cst_flight_mode_changed_cb_for_call_barring(system_settings_key_e key, void *data)
{
	ENTER(_cst_flight_mode_changed_cb_for_call_barring);
	ret_if(NULL == data);
	Elm_Object_Item *gl_item = (Elm_Object_Item *)data;

	_cst_flight_mode_genlist_item_disable(gl_item);

	elm_genlist_item_update(gl_item);
}

void _cst_flight_mode_changed_cb_for_call_waiting(system_settings_key_e key, void *data)
{
	ENTER(_cst_flight_mode_changed_cb_for_call_waiting);
	ret_if(NULL == data);
	Elm_Object_Item *gl_item = (Elm_Object_Item *)data;

	_cst_flight_mode_genlist_item_disable(gl_item);

	elm_genlist_item_update(gl_item);
}

void _cst_flight_mode_unlisten_setting_change_for_main_menu(void)
{
	ENTER(_cst_flight_mode_unlisten_setting_change_for_main_menu);
	_cst_util_system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_voice_mail_number);

}

void _cst_flight_mode_unlisten_setting_change_for_more_call_setting(void)
{
	ENTER(_cst_flight_mode_unlisten_setting_change_for_more_call_setting);

	_cst_util_system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_call_forwarding);
	_cst_util_system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_call_barring);
	_cst_util_system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_NETWORK_FLIGHT_MODE, _cst_flight_mode_changed_cb_for_call_waiting);
}

char *_cst_get_voice_mail_number(CstUgData_t *ugd)
{
	ENTER(_cst_get_voice_mail_number);
	retv_if(ugd == NULL, NULL);
	msg_error_t err = MSG_SUCCESS;
	char vmail[CST_MAX_PHONE_NUMBER_LEN+1] = {0,};

	msg_struct_t msg_struct = NULL;
	msg_struct = msg_create_struct(MSG_STRUCT_SETTING_VOICE_MSG_OPT);

	if (ugd->sel_sim == CST_SELECTED_SIM1) {
		err = msg_set_int_value(msg_struct, MSG_VOICEMSG_SIM_INDEX_INT, 1);
	} else {
		err = msg_set_int_value(msg_struct, MSG_VOICEMSG_SIM_INDEX_INT, 2);
	}
	if (MSG_SUCCESS != err) {
		ERR("msg_set_int_value failed !!");
		msg_release_struct(&msg_struct);
		return NULL;
	}

	err = msg_get_voice_msg_opt(ugd->msg_handle, msg_struct);
	if (MSG_SUCCESS != err) {
		DBG("msg_get_voice_msg_opt() is failed: %d", err);
		msg_release_struct(&msg_struct);
		return NULL;
	}

	err = msg_get_str_value(msg_struct, MSG_VOICEMSG_ADDRESS_STR, vmail, CST_MAX_PHONE_NUMBER_LEN);
	if (MSG_SUCCESS != err) {
		DBG("msg_get_str_value() is failed: %d", err);
		msg_release_struct(&msg_struct);
		return NULL;
	}

	msg_release_struct(&msg_struct);

	if (strlen(vmail) < 1) {
		return NULL;
	}
	return strdup(vmail);
}

Eina_Bool _cst_set_voice_mail_number(CstUgData_t *ugd, char *vm_num)
{
	ENTER(_cst_set_voice_mail_number);

	msg_error_t err = MSG_SUCCESS;
	Eina_Bool ret = EINA_TRUE;
	retv_if((ugd == NULL), EINA_FALSE);

	msg_struct_t msg_struct = NULL;
	msg_struct = msg_create_struct(MSG_STRUCT_SETTING_VOICE_MSG_OPT);

	if (ugd->sel_sim == CST_SELECTED_SIM1) {
		err = msg_set_int_value(msg_struct, MSG_VOICEMSG_SIM_INDEX_INT, 1);
	} else {
		err = msg_set_int_value(msg_struct, MSG_VOICEMSG_SIM_INDEX_INT, 2);
	}
	if (MSG_SUCCESS != err) {
		ERR("msg_get_voice_msg_opt failed !!");
		msg_release_struct(&msg_struct);
		ret = EINA_FALSE;
		return ret;
	}

	if (MSG_SUCCESS != msg_set_str_value(msg_struct, MSG_VOICEMSG_ADDRESS_STR, vm_num, strlen(vm_num))) {
		DBG("msg_set_str_value() is failed");
		ret = EINA_FALSE;
	} else if (MSG_SUCCESS != msg_set_voice_msg_opt(ugd->msg_handle, msg_struct)) {
		DBG("msg_set_voice_msg_opt() is failed");
		ret = EINA_FALSE;
	}

	msg_release_struct(&msg_struct);
	return ret;
}

int _cst_parse_bundle(service_h service)
{
	int type = CST_UG_REQ_MAIN_SCREEN;
	char *val = NULL;
	char *viewtype = NULL;
	char *keyword = NULL;

	if (service) {
		service_get_extra_data(service, CST_UG_BUNDLE_TYPE, &val);
		if (val) {
			type = atoi(val);
		}

		service_get_extra_data(service, CST_UG_BUNDLE_VIEWTYPE, &viewtype);	/* viewtype : frontpage*/
		if (viewtype) {
			DBG("viewtype = %s ", viewtype);
		}

		service_get_extra_data(service, CST_UG_BUNDLE_KEYWORD, &keyword); /* keyword : string STMS ID*/
		if (keyword && viewtype) {
			DBG("\n\n *** keyword = %s *** \n\n", keyword);
			if (!strcmp(keyword, "IDS_CST_HEADER_CALL_BLOCK_LIST_ABB")) {
				type = CST_UG_REQ_AUTO_REJECT_LIST;
			} else if (!strcmp(keyword, "IDS_CST_HEADER_CALL_REJECTION") ||
					!strcmp(keyword, "IDS_CST_HEADER_AUTO_REJECT_MODE")) {
				type = CST_UG_REQ_AUTO_REJECT_MODE;
			} else if (!strcmp(keyword, "IDS_CST_MBODY_REJECTION_MESSAGES")) {
				type = CST_UG_REQ_REJECT_MESSAGES;
			} else if (!strcmp(keyword, "IDS_CST_BODY_VOICE_MAIL_NUMBER")) {
				type = CST_UG_REQ_VOICE_MAIL;
			} else if (!strcmp(keyword, "IDS_CST_HEADER_ANSWERING_ENDING_CALLS_ABB") ||
					!strcmp(keyword, "IDS_CST_BODY_THE_HOME_KEY_ANSWERS_CALLS") ||
					!strcmp(keyword, "IDS_CAM_BODY_VOICE_CONTROL_ABB") ||
					!strcmp(keyword, "IDS_CST_BODY_THE_POWER_KEY_ENDS_CALLS_ABB2")) {
				type = CST_UG_REQ_ANSWER_END_CALLS;
			} else if (!strcmp(keyword, "IDS_CST_BODY_SCREEN_OFF_IN_CALLS_ABB") ||
					!strcmp(keyword, "IDS_CST_BODY_USE_CALL_FAIL_OPTIONS")) {
				type = CST_UG_REQ_MAIN_SCREEN;
			} else if (!strcmp(keyword, "IDS_CST_MBODY_CALL_ALERTS") ||
					!strcmp(keyword, "IDS_CST_MBODY_ANSWER_VIBRATION") ||
					!strcmp(keyword, "IDS_CST_MBODY_CALL_END_VIBRATION") ||
					!strcmp(keyword, "IDS_CST_BODY_CALL_CONNECT_TONE") ||
					!strcmp(keyword, "IDS_CST_BODY_MINUTE_REMINDERS_ABB") ||
					!strcmp(keyword, "IDS_CST_BODY_CALL_END_TONE") ||
					!strcmp(keyword, "IDS_CST_BODY_ALERTS_ON_CALL")) {
				type = CST_UG_REQ_CALL_ALERTS;
			} else if (!strcmp(keyword, "IDS_CST_BODY_CALL_ACCESSORIES") ||
					!strcmp(keyword, "IDS_CST_MBODY_AUTOMATIC_ANSWERING") ||
					!strcmp(keyword, "IDS_CST_MBODY_AUTOMATIC_ANSWERING_TIMER") ||
					!strcmp(keyword, "IDS_CST_MBODY_OUTGOING_CALL_CONDITIONS") ||
					!strcmp(keyword, "IDS_CST_MBODY_OUTGOING_CALL_TYPE") ||
					!strcmp(keyword, "IDS_CST_BODY_INCOMING_CALL_SOUND_DEVICE_ABB")) {
				type = CST_UG_REQ_CALL_ACCESSORIES;
			} else if (!strcmp(keyword, "IDS_CST_HEADER_ADDITIONAL_SETTINGS") ||
					!strcmp(keyword, "IDS_CST_MBODY_MY_CALLER_ID") ||
					!strcmp(keyword, "IDS_CST_BODY_AUTO_AREA_CODE_ABB") ||
					!strcmp(keyword, "IDS_CST_BODY_AUTO_REDIAL")) {
				type = CST_UG_REQ_ADDITIONAL_SETTINGS;
			} else if (!strcmp(keyword, "IDS_CST_BODY_CALL_WAITING")) {
				type = CST_UG_REQ_CALL_WAITING;
			} else if (!strcmp(keyword, "IDS_ST_BODY_SOUNDS") ||
					!strcmp(keyword, "IDS_COM_BODY_RINGTONES") ||
					!strcmp(keyword, "IDS_ST_BODY_VIBRATIONS") ||
					!strcmp(keyword, "IDS_ST_HEADER_KEYPAD_TONES")) {
				type = CST_UG_REQ_RING_KEYPAD_TONES;
			} else if (!strcmp(keyword, "IDS_CST_BODY_PERSONALISE_CALL_SOUND_ABB")) {
				type = CST_UG_REQ_PERSONALIZE_CALL_SOUND;
			}
		}

		WARN("Bundle type = %d", type);
	}
	if (keyword) {
		free(keyword);
		keyword = NULL;
	}
	if (val) {
		free(val);
		val = NULL;
	}
	if (viewtype) {
		free(viewtype);
		viewtype = NULL;
	}
	return type;
}

static Eina_Bool __cst_imf_context_entry_focus_set_cb(void *data)
{
	retv_if(NULL == data, ECORE_CALLBACK_CANCEL);
	elm_object_focus_set((Evas_Object *)data, EINA_TRUE);
	elm_entry_cursor_end_set((Evas_Object *)data);
	return ECORE_CALLBACK_CANCEL;
}

static void __cst_contact_list_view_reply_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *user_data)
{
	ENTER(__cst_contact_list_view_reply_cb);
	CstUgData_t *ugd = (CstUgData_t *)user_data;
	ret_if(NULL == ugd);
	ret_if(APP_CONTROL_RESULT_SUCCEEDED != result);

	int number_id = -1;
	char **numbers = NULL;
	int count = 0;
	char *number = NULL;
	contacts_record_h numbers_record = NULL;
	char buffer[CST_MAX_PHONE_NUMBER_LEN] = { 0, };

	app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &numbers, &count);
	if (numbers && numbers[0]) {
		number_id = atoi(numbers[0]);
	}

	int i = 0;
	for (; i < count; ++i) {
		free(numbers[i]);
	}

	free(numbers);
	numbers = NULL;

	DBG("number_id = %d", number_id);

	if (contacts_connect() != CONTACTS_ERROR_NONE) {
		ERR("contacts_connect failed");
		_cst_create_error_popup(CST_ERROR_UNKNOWN);
		return;
	}

	/* get and set number */
	if (contacts_db_get_record(_contacts_number._uri, number_id, &numbers_record) != CONTACTS_ERROR_NONE) {
		DBG("contacts_db_get_record for number failed");
	} else {
		if (contacts_record_get_str_p(numbers_record, _contacts_number.number, (char **)&number) == CONTACTS_ERROR_NONE) {
			DBG("number = %s", number);
			/*Remove Invalid characters from the phone number received from Contacts*/
			_cst_util_remove_invalid_chars_from_phone_num(number, buffer);
		}
	}

	if (ugd->dg_entry_contact_number) {
		elm_entry_entry_set(ugd->dg_entry_contact_number, buffer);
	} else {
		elm_entry_entry_set(ugd->dg_entry, buffer);
	}
	contacts_record_destroy(numbers_record, true);

	if (contacts_disconnect() != CONTACTS_ERROR_NONE) {
		DBG("contacts_disconnect failed");
	}

	ugd->is_app_control_invoked = false;

	if (ugd->dg_entry_contact_number) {
		ecore_idler_add(__cst_imf_context_entry_focus_set_cb,
				(const void *)ugd->dg_entry_contact_number);
	} else if (ugd->popup == NULL) {
		ecore_idler_add(__cst_imf_context_entry_focus_set_cb,
				(const void *)ugd->dg_entry);
	}
}

static void __cst_invoke_contact_single_picker(void *data)
{
	ENTER(__cst_invoke_contact_single_picker);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	app_control_h service = NULL;

	app_control_create(&service);
	ret_if(NULL == service);

	app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
	app_control_set_mime(service, CST_CT_APPCONTROL_MIME_CONTACT);
	app_control_add_extra_data(service, CST_CT_APPCONTROL_DATA_TYPE,
			CST_CT_APPCONTROL_DATA_PHONE);

	if (APP_CONTROL_ERROR_NONE == app_control_send_launch_request(service,
			__cst_contact_list_view_reply_cb, (void *)ugd)) {
		ugd->is_app_control_invoked = true;
	}
	app_control_destroy(service);
}

static Eina_Bool __cst_util_contact_ug_create_cb(void *data)
{
	retv_if(NULL == data, ECORE_CALLBACK_CANCEL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *contact_icon = (Evas_Object *) elm_object_part_content_get(ugd->contact_btn, "icon");
	evas_object_color_set(contact_icon, 61, 185, 204, 255);
	__cst_invoke_contact_single_picker(data);
	return ECORE_CALLBACK_CANCEL;
}

void _cst_on_click_ime_contact_btn(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_ime_contact_btn);
	ret_if(NULL == data);
	ecore_timer_add(0.05, __cst_util_contact_ug_create_cb,
			(const void *)data);
}

void _cst_open_msg_handle(CstUgData_t *ugd)
{
	ENTER(_cst_open_msg_handle);
	ret_if(NULL == ugd);

	msg_error_t msg_error = msg_open_msg_handle(&(ugd->msg_handle));
	if (msg_error < CST_ERR_NONE) {
		DBG("msg_open_msg_handle Failed : %d", msg_error);
	}
}

void _cst_close_msg_handle(CstUgData_t *ugd)
{
	ENTER(_cst_close_msg_handle);
	ret_if(NULL == ugd);

	msg_error_t msg_error = msg_close_msg_handle(&(ugd->msg_handle));
	if (msg_error < CST_ERR_NONE) {
		DBG("msg_close_msg_handle Failed : %d", msg_error);
	}
}

void _cst_unlisten_vconf_change(void)
{
	ENTER(_cst_unlisten_vconf_change);

	_cst_flight_mode_unlisten_setting_change_for_main_menu();
	_cst_flight_mode_unlisten_setting_change_for_more_call_setting();
}

/* this api is shared also by NTT DoCoMo code*/
Eina_Bool _cst_naviframe_item_pop_cb(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;
	evas_object_smart_callback_del(ugd->nf, "transition,finished", _cst_transition_cb);

	ugd->dg_entry = NULL;

	elm_naviframe_item_pop_cb_set(elm_naviframe_top_item_get(ugd->nf), NULL, NULL);

	elm_naviframe_item_pop(ugd->nf);

	ugd->back_button = _cst_get_navifr_prev_btn(ugd->nf);

	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool _cst_util_remove_invalid_chars_from_phone_num(const char *src_num,
		char *dst_num)
{
	ENTER(_cst_util_remove_invalid_chars_from_phone_num);
	if ((src_num == NULL) || (dst_num == NULL)) {
		return EINA_FALSE;
	}
	int i = 0;
	int j = 0;
	int nSrc = 0;

	nSrc = strlen(src_num);
	SEC_DBG("* passed number [%s] *", src_num);

	for (i = 0; i < nSrc; ++i) {
		switch (src_num[i]) {
			case '(':
				if (src_num[i + 1] == '0')
					++i;
				break;
			case ')':
			case '-':
			case ' ':
			case '/':
				break;
			default:
				dst_num[j++] = src_num[i];
				break;
		}

		if (j > CST_MAX_PHONE_NUMBER_LEN) {
			break;
		}
	}

	dst_num[j] = '\0';
	SEC_DBG("* returned number [%s] *", dst_num);

	return EINA_TRUE;
}

char *_cst_util_fetch_basename_from_path(char *path)
{
	ENTER(_cst_util_fetch_basename_from_path);

	if (NULL == path || '\0' == path[0]) {
		return NULL;	/* invalid arguement */
	}
	char *p = strrchr(path, '/');
	if (!p) {
		return (char *)g_strdup(path);	/*  cannot find '/' */
	}
	if ('\0' == p[1]) {
		return NULL;	/* end with '/' */
	}

	return (char *)g_strdup(p + 1);
}

char *_cst_util_get_metadata_from_media_file(char *file_path)
{
	metadata_extractor_h metadata = NULL;

	char *title = NULL;
	int ret = metadata_extractor_create(&metadata);
	if (ret == METADATA_EXTRACTOR_ERROR_NONE && metadata) {
		ret = metadata_extractor_set_path(metadata, file_path);
		if (ret == METADATA_EXTRACTOR_ERROR_NONE) {
			ret = metadata_extractor_get_metadata(metadata, METADATA_TITLE, &title);
			metadata_extractor_destroy(metadata);
			return (char *)title;
		}
		metadata_extractor_destroy(metadata);
   }

	return NULL;
}

static Eina_List *keys_array[SYSTEM_SETTINGS_KEY_MAX];

/* Settings data */
typedef struct {
	system_settings_changed_cb cb;
	void *user_data;
} settings_cb_data;

static void __system_settings_changed_in_cb(system_settings_key_e key, void *user_data)
{
	ENTER("__system_settings_changed_in_cb");

	if (key < 0 || key >= SYSTEM_SETTINGS_KEY_MAX) {
		ERR("System Error. Wrong key.");
		return;
	}

	if (!keys_array[key]) {
		ERR("No callbacks for key.");
		return;
	}

	Eina_List *list = keys_array[key];
	Eina_List *l = NULL;
	void *list_data = NULL;
	EINA_LIST_FOREACH(list, l, list_data) {
		settings_cb_data *data = (settings_cb_data *) list_data;
		if (data) {
			data->cb(key, data->user_data);
		}
	}
}

static Eina_Bool __append_settings_cb_data(system_settings_key_e key, system_settings_changed_cb callback, void *user_data)
{
	settings_cb_data *data = calloc(1, sizeof(settings_cb_data));
	if (data) {
		data->cb = callback;
		data->user_data = user_data;

		keys_array[key] = eina_list_append(keys_array[key], data);

		return EINA_TRUE;
	}

	return EINA_FALSE;
}

int _cst_util_system_settings_set_changed_cb(system_settings_key_e key, system_settings_changed_cb callback, void *user_data)
{
	ENTER("_cst_util_system_settings_set_changed_cb");

	if (key < 0 || key >= SYSTEM_SETTINGS_KEY_MAX || !callback) {
		return SYSTEM_SETTINGS_ERROR_INVALID_PARAMETER;
	}

	if (NULL == keys_array[key]) {
		int ret = system_settings_set_changed_cb(key, __system_settings_changed_in_cb, NULL);
		if (SYSTEM_SETTINGS_ERROR_NONE != ret) {
			ERR("system_settings_set_changed_cb failed. err = %d", ret);
			return ret;
		}
	} else {
		Eina_List *list = keys_array[key];
		Eina_List *l = NULL;
		void *list_data = NULL;
		EINA_LIST_FOREACH(list, l, list_data) {
			settings_cb_data *data = (settings_cb_data *) list_data;
			if (data) {
				if (data->cb == callback) {

					/* Already added */
					WARN("The callback already added");
					return SYSTEM_SETTINGS_ERROR_NONE;
				}
			}
		}
	}

	if (!__append_settings_cb_data(key, callback, user_data)) {
		return SYSTEM_SETTINGS_ERROR_OUT_OF_MEMORY;
	}

	return SYSTEM_SETTINGS_ERROR_NONE;
}


int _cst_util_system_settings_unset_changed_cb(system_settings_key_e key, system_settings_changed_cb callback)
{
	ENTER("_cst_util_system_settings_unset_changed_cb");

	if (key < 0 || key >= SYSTEM_SETTINGS_KEY_MAX || !callback || !keys_array[key]) {
		return SYSTEM_SETTINGS_ERROR_INVALID_PARAMETER;
	}

	Eina_List *list = keys_array[key];
	Eina_List *l = NULL;
	void *list_data = NULL;
	EINA_LIST_FOREACH(list, l, list_data) {
		settings_cb_data *data = (settings_cb_data *) list_data;
		if (data) {
			if (data->cb == callback) {
				keys_array[key] = eina_list_remove(keys_array[key], data);
				free(data);

				if (NULL == keys_array[key]) {
					int ret = system_settings_unset_changed_cb(key);

					if (SYSTEM_SETTINGS_ERROR_NONE != ret) {
						ERR("system_settings_unset_changed_cb failed. Err = %d", ret);
					}
				}
				return SYSTEM_SETTINGS_ERROR_NONE;
			}
		}
	}

	return SYSTEM_SETTINGS_ERROR_INVALID_PARAMETER;
}

int _cst_util_system_settings_unset_changed_cb_all(system_settings_key_e key)
{
	ENTER("_cst_util_system_settings_unset_changed_cb_all");

	if (key < 0 || key >= SYSTEM_SETTINGS_KEY_MAX || !keys_array[key]) {
		return SYSTEM_SETTINGS_ERROR_INVALID_PARAMETER;
	}

	Eina_List *list = keys_array[key];
	void *list_data = NULL;
	EINA_LIST_FREE(list, list_data) {
		free(list_data);
	}

	keys_array[key] = NULL;

	return system_settings_unset_changed_cb(key);
}

#ifdef _CALL_SET_TTS_SUPPORT
char *_cst_util_entry_number_access_info_cb(void *data, Evas_Object *obj)
{
	char *ret = NULL;
	Evas_Object *entry = (Evas_Object *)data;
	char access_info_text[2 * CST_MAX_PHONE_NUMBER_LEN] = {0, };
	Eina_Strbuf *buf = eina_strbuf_new();
	const char *text_number = elm_entry_entry_get(entry);

	_cst_util_convert_tts_number(text_number, access_info_text);

	eina_strbuf_append_printf(buf, "%s", access_info_text);

	ret = eina_strbuf_string_steal(buf);
	eina_strbuf_free(buf);

	DBG("ret = (%s)", ret);

	return ret;
}

char *_cst_util_2item_number_access_info_cb(void *data, Evas_Object *obj)
{
	char *ret = NULL;
	Elm_Object_Item *item = (Elm_Object_Item *)data;
	char access_info_text[2 * CST_MAX_PHONE_NUMBER_LEN] = {0, };
	Eina_Strbuf *buf = eina_strbuf_new();
	const char *main_text = elm_object_item_part_text_get(item, "elm.text.1");
	if (NULL == main_text)
		main_text = elm_object_item_part_text_get(item, "elm.text");
	const char *text_number = elm_object_item_part_text_get(item, "elm.text.2");

	_cst_util_convert_tts_number(text_number, access_info_text);

	eina_strbuf_append_printf(buf, "%s %s", main_text, access_info_text);

	ret = eina_strbuf_string_steal(buf);
	eina_strbuf_free(buf);

	DBG("ret = (%s)", ret);

	return ret;
}

char *_cst_util_on_off_btn_access_info_cb(void *data, Evas_Object *obj)
{
	Elm_Object_Item *item = (Elm_Object_Item *)data;

	char *ret = NULL;
	Eina_Strbuf *buf = eina_strbuf_new();

	const char *text_1 = elm_object_item_part_text_get(item, "elm.text.1");
	if (NULL == text_1)
		text_1 = elm_object_item_part_text_get(item, "elm.text");
	const char *text_2 = elm_object_item_part_text_get(item, "elm.text.2");
	const char *text_3 = elm_object_item_part_text_get(item, "elm.text.3");

	const char *on_off_text;
	Evas_Object *check_btn = elm_object_item_part_content_get(item, "elm.icon");
	Eina_Bool value = elm_check_state_get(check_btn);

	if (0 == value)
		on_off_text = T_(CST_STR_OFF);
	else
		on_off_text = T_(CST_STR_ON);

	if (text_2 != NULL) {
		if (text_3 != NULL) {
			eina_strbuf_append_printf(buf, "%s %s %s %s %s", text_1, text_2, text_3, T_(CST_STR_ON_OFF_BTN), on_off_text);
		} else {
			eina_strbuf_append_printf(buf, "%s %s %s %s", text_1, text_2, T_(CST_STR_ON_OFF_BTN), on_off_text);
		}
	} else {
		eina_strbuf_append_printf(buf, "%s %s %s", text_1, T_(CST_STR_ON_OFF_BTN), on_off_text);
	}

	ret = eina_strbuf_string_steal(buf);
	eina_strbuf_free(buf);

	DBG("ret = (%s)", ret);
	return ret;
}

void _cst_util_convert_tts_number(const char *text_pointer, char *access_entry_text)
{
	int i = 0;
	if ((text_pointer == NULL) || (access_entry_text == NULL)) {
		ERR("Entry info set fails!!!");
		return;
	}

	if ((strlen(text_pointer) > 0)
		&& (strchr(text_pointer, '-') == NULL)) {
		while (*text_pointer != '\0') {
			access_entry_text[i] = *text_pointer;
			i++;
			if (isdigit(*text_pointer)) {
				access_entry_text[i] = ' ';
				i++;
			}
			text_pointer++;
		}
		access_entry_text[i] = '\0';
	}

	DBG("converted tts call number :[%s]", access_entry_text);
}
#endif /*_CALL_SET_TTS_SUPPORT*/

static void __cst_on_clicked_navi_back_btn(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_clicked_navi_back_btn);
	ret_if(NULL == data);
	Evas_Object *nf = (Evas_Object *)data;

	elm_naviframe_item_pop(nf);
}

Evas_Object *_cst_util_navi_back_btn_create(Evas_Object *naviframe)
{
	ENTER(_cst_util_navi_back_btn_create);

	Evas_Object *back_btn = elm_button_add(naviframe);
	elm_object_style_set(back_btn, "naviframe/back_btn/default");
	evas_object_smart_callback_add(back_btn, "clicked", __cst_on_clicked_navi_back_btn, naviframe);

	return back_btn;
}
