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
#include <Eina.h>
#include <vconf.h>
#include <app.h>
#include <efl_extension.h>

#include "cst-common.h"
#include "cst-debug.h"
#include "cst-common-string.h"
#include "cst-tapi-request.h"
#include "cst-call-setting.h"
#include "cst-util.h"
#include "cst-widget.h"
#include "cst-reject-msg.h"
#include "cst-answering.h"
#include "cst-more-call-setting.h"
#include "cst-waiting.h"

static bool __cst_app_create(void *data);
static void __cst_app_control(app_control_h app_control, void *data);
static void __cst_app_pause(void *data);
static void __cst_app_resume(void *data);
static void __cst_app_terminate(void *data);

static int __cst_create_naviframe(CstAppData_t *ad)
{
	ENTER(__cst_create_naviframe);

	ad->nf = elm_naviframe_add(ad->conform);
	retv_if(!ad->nf, false);
	elm_naviframe_prev_btn_auto_pushed_set(ad->nf, EINA_FALSE);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	evas_object_show(ad->nf);
	elm_object_focus_set(ad->nf, EINA_TRUE);

	elm_object_part_content_set(ad->main_layout, "elm.swallow.content", ad->nf);

	LEAVE();

	return true;
}

static int __cst_create_main_layout(CstAppData_t *ad)
{
	ENTER(__cst_create_main_layout);

	ad->main_layout = elm_layout_add(ad->win_main);
	retv_if(!ad->main_layout, false);

	elm_layout_theme_set(ad->main_layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(ad->main_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(ad->main_layout);

	elm_object_content_set(ad->conform, ad->main_layout);

	Evas_Object *bg = elm_bg_add(ad->main_layout);
	retv_if(!bg, false);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_style_set(bg, "transparent");
	elm_object_part_content_set(ad->main_layout, "elm.swallow.bg", bg);

	evas_object_show(bg);

	return true;
}

/**
* Reset function to 'reset' the settings of the UG, it will be invoked by 'Reset' UG
*
* @param[in] data
* @param[in] priv
*/
CST_MODULE_EXPORT int setting_plugin_reset(app_control_h app_control, void *priv)
{
	ENTER(setting_plugin_reset);
	int ret = 0;

	/*Answering mode*/
	ret += vconf_set_bool(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, EINA_FALSE);
	ret += vconf_set_bool(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, EINA_FALSE);

	/*Reject call message*/
	ret += vconf_set_int(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT, 5);
	ret += vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR, "IDS_CST_MBODY_SORRY_IM_BUSY_CALL_YOU_LATER_ABB");
	ret += vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR, "IDS_CST_BODY_SORRY_IM_BUSY_PLEASE_TEXT_ME_ABB");
	ret += vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR, "IDS_CST_TMBODY_I_CANT_TALK_NOW_WHATS_UP_Q");
	ret += vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR, "IDS_CST_BODY_IM_IN_A_MEETING");
	ret += vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR, "IDS_CST_TMBODY_SORRY_IM_DRIVING_NOW");
	ret += vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR, "");

	return ret;
}

int main(int argc, char *argv[])
{
	CstAppData_t ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};

	event_callback.create = __cst_app_create;
	event_callback.terminate = __cst_app_terminate;
	event_callback.pause = __cst_app_pause;
	event_callback.resume = __cst_app_resume;
	event_callback.app_control = __cst_app_control;

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}

static void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static int __cst_create_base_eo(CstAppData_t *ad)
{
	ad->win_main = elm_win_util_standard_add(PACKAGE, PACKAGE);
	retv_if(!ad->win_main, false);
	elm_win_autodel_set(ad->win_main, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win_main)) {
		int rots[4] = { 0 };
		elm_win_wm_rotation_available_rotations_set(ad->win_main, (const int *)(&rots), 1);
	}

	evas_object_smart_callback_add(ad->win_main, "delete,request", win_delete_request_cb, NULL);

	ad->conform = elm_conformant_add(ad->win_main);
	retv_if(!ad->conform, false);
	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win_main, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_conformant_set(ad->win_main, EINA_TRUE);
	elm_win_resize_object_add(ad->win_main, ad->conform);

	evas_object_show(ad->conform);
	evas_object_show(ad->win_main);

	return true;
}

static bool __cst_app_create(void *data)
{
	ENTER(__FUNCTION__);

	retv_if(!data, false);
	CstAppData_t *ad = data;

	bool res = __cst_create_base_eo(ad);
	if (!res) {
		ERR("__cst_create_base_eo failed");
		return false;
	}
	res = __cst_create_main_layout(ad);
	if (!res) {
		ERR("__cst_create_main_layout failed");
		evas_object_del(ad->win_main);
		return false;
	}
	res = __cst_create_naviframe(ad);
	if (!res) {
		ERR("__cst_create_naviframe failed");
		evas_object_del(ad->win_main);
		return false;
	}

	cst_util_feedback_init();

	_cst_register_tel_event(ad);

	bindtextdomain(APPNAME, CST_LOCALE);

	elm_theme_extension_add(NULL, THEME_NAME);

	return true;
}

static void __cst_app_control(app_control_h app_control, void *data)
{
	ENTER(__FUNCTION__);

	ret_if(!data);
	CstAppData_t *ad = data;

	if (app_control) {
		ad->ug_req_type = _cst_parse_bundle(app_control);
		DBG("ug_req_type=%d", ad->ug_req_type);
	} else {
		ad->ug_req_type = CST_UG_REQ_MAIN_SCREEN;
	}

	switch (ad->ug_req_type) {
	case CST_UG_REQ_MAIN_SCREEN:
		DBG("Its a CST_UG_REQ_MAIN_SCREEN UG request.");
		_cst_create_call_setting(ad);
		break;
	case CST_UG_REQ_REJECT_MESSAGES:
		DBG("Its a CST_UG_REQ_REJECT_MESSAGES UG request.");
		_cst_on_click_reject_message(ad, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_ANSWER_END_CALLS:
		DBG("Its a CST_UG_REQ_ANSWER_END_CALLS UG request.");
		_cst_on_click_answering_call(ad, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_ADDITIONAL_SETTINGS:
		DBG("Its a CST_UG_REQ_ADDITIONAL_SETTINGS UG request.");
		_cst_on_click_more_call_setting(ad, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_CALL_WAITING:
		DBG("Its a CST_UG_REQ_CALL_WAITING UG request.");
		_cst_on_click_more_call_setting(ad, NULL, NULL, NULL);
		break;
	default:
		_cst_create_call_setting(ad);
		break;
	}
}

static void __cst_app_pause(void *data)
{
	ENTER(__FUNCTION__);
}

static void __cst_app_resume(void *data)
{
	ENTER(__FUNCTION__);
}

static void __cst_app_terminate(void *data)
{
	ret_if(!data);
	CstAppData_t *ad = data;

	_cst_deregister_tel_event(ad);
	_cst_unlisten_vconf_change();
	_cst_destroy_all_items(ad);

	if (ad->rejct_popup) {
		evas_object_del(ad->rejct_popup);
		ad->rejct_popup = NULL;
	}
	if (ad->reject_list) {
		ad->reject_list = eina_list_free(ad->reject_list);
	}

	if (ad->req_queue) {
		Eina_List *l;
		CallSettingSSReq_t *req;

		EINA_LIST_FOREACH(ad->req_queue, l, req) {
			free(req);
		}
		ad->req_queue = eina_list_free(ad->req_queue);
	}

	if (ad->sim_req_list) {
		g_slist_free_full(ad->sim_req_list, g_free);
		ad->sim_req_list = NULL;
	}

	if (ad->popup != NULL) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (ad->win_main) {
		evas_object_del(ad->win_main);
	}

	elm_theme_extension_del(NULL, THEME_NAME);

	cst_util_feedback_deinit();

	LEAVE();
}
