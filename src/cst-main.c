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
#include <ui-gadget-module.h>
#include <ui-gadget.h>
#include <Eina.h>
#include <vconf.h>
#include <app.h>
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
#include <efl_extension.h>

static Evas_Object *__cst_create_content(Evas_Object *parent, CstUgData_t *ugd)
{
	ENTER(__cst_create_content);
	Evas_Object *nf;
	retv_if(parent == NULL, NULL);

	nf = elm_naviframe_add(parent);
	ugd->nf = nf;
	elm_naviframe_prev_btn_auto_pushed_set(nf, EINA_FALSE);
	eext_object_event_callback_add(nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	evas_object_show(nf);
	elm_object_focus_set(nf, EINA_TRUE);
	ugd->popup = NULL;

	switch (ugd->ug_req_type) {
	case CST_UG_REQ_MAIN_SCREEN:
		DBG("Its a CST_UG_REQ_MAIN_SCREEN UG request.");
		_cst_create_call_setting(ugd);
		break;
	case CST_UG_REQ_REJECT_MESSAGES:
		DBG("Its a CST_UG_REQ_REJECT_MESSAGES UG request.");
		_cst_on_click_reject_message(ugd, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_ANSWER_END_CALLS:
		DBG("Its a CST_UG_REQ_ANSWER_END_CALLS UG request.");
		_cst_on_click_answering_call(ugd, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_ADDITIONAL_SETTINGS:
		DBG("Its a CST_UG_REQ_ADDITIONAL_SETTINGS UG request.");
		_cst_on_click_more_call_setting(ugd, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_CALL_WAITING:
		DBG("Its a CST_UG_REQ_CALL_WAITING UG request.");
		_cst_on_click_more_call_setting(ugd, NULL, NULL, NULL);
		break;
	case CST_UG_REQ_VOICE_MAIL:
		break;
	default:
		_cst_create_call_setting(ugd);
		break;
	}

	LEAVE();

	return nf;
}

static Evas_Object *__cst_create_fullview(Evas_Object *parent, CstUgData_t *ugd)
{
	ENTER(__cst_create_fullview);
	Evas_Object *base;

	/* Create Full view */
	base = elm_layout_add(parent);
	if (!base) {
		return NULL;
	}

	elm_layout_theme_set(base, "layout", "application", "default");
	evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(base);
	LEAVE();

	return base;
}

static Evas_Object *__cst_create_frameview(Evas_Object *parent, CstUgData_t *ugd)
{
	ENTER(__cst_create_frameview);
	Evas_Object *base = NULL;

	return base;
}

static Evas_Object *__cst_create_bg(Evas_Object *parent)
{
	ENTER(__cst_create_bg);
	Evas_Object *bg = elm_bg_add(parent);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(bg);

	return bg;
}

static void *__cst_on_create(ui_gadget_h ug, enum ug_mode mode, app_control_h app_control, void *priv)
{
	WARN(">>");
	Evas_Object *parent, *content;
	CstUgData_t *ugd;
	char *simslot_id = NULL;

	if (!ug || !priv) {
		return NULL;
	}

	ugd = priv;
	ugd->ug = (void *)ug;

	bindtextdomain(UGNAME, CST_LOCALE);
	cst_util_feedback_init();
	elm_theme_extension_add(NULL, THEME_NAME);

	parent = ug_get_parent_layout(ug);
	ugd->win_main = parent;
	retv_if(parent == NULL, NULL);
	ugd->back_button = NULL;
	ugd->req_queue = NULL;
	ugd->sim_req_list = NULL;

	int rotate_angles[3] = {0};

	/* Set the required angles wherein the rotation has to be supported */
	elm_win_wm_rotation_available_rotations_set(ugd->win_main,
			rotate_angles, 1);

	if (app_control) {
		ugd->ug_req_type = _cst_parse_bundle(app_control);
		DBG("ug_req_type=%d", ugd->ug_req_type);
	} else {
		ugd->ug_req_type = CST_UG_REQ_MAIN_SCREEN;
	}

	/* Register the telephony ss events */
	_cst_register_tel_event(ugd);

	if (ugd->ug_req_type == CST_UG_REQ_VOICE_MAIL) {
		app_control_get_extra_data(app_control, CST_UG_BUNDLE_VOICEMAIL_SIMSLOT, &simslot_id);
		if (NULL == simslot_id) {
			return NULL;
		}
		int sim_id = atoi(simslot_id);
		free(simslot_id);
		if (0 == sim_id) {
			ugd->sel_sim = CST_SELECTED_SIM1;
			_cst_update_tapi_handle_by_simslot(ugd, CST_SELECTED_SIM1);
		} else if (1 == sim_id) {
			ugd->sel_sim = CST_SELECTED_SIM2;
			_cst_update_tapi_handle_by_simslot(ugd, CST_SELECTED_SIM2);
		} else {
			DBG("Invalid SIM Index = %d", sim_id);
			return NULL;
		}
	}

	/* Get message handle */
	_cst_open_msg_handle(ugd);

	ugd->bg = __cst_create_bg(parent);
	elm_object_style_set(ugd->bg, "transparent");

	if (mode == UG_MODE_FULLVIEW) {
		ugd->base = __cst_create_fullview(parent, ugd);
	} else {
		ugd->base = __cst_create_frameview(parent, ugd);
	}

	if (ugd->base) {
		content = __cst_create_content(parent, ugd);
		elm_object_part_content_set(ugd->base, "elm.swallow.content", content);
	}
	elm_object_part_content_set(ugd->base, "elm.swallow.bg", ugd->bg);

	ugd->scale = elm_config_scale_get();
	DBG("scale factor=%f", ugd->scale);
	return ugd->base;
}

static void __cst_on_start(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	ENTER(__cst_on_start);
	CstUgData_t *ugd = priv;
	ret_if(!ugd);
	ret_if(!ug);

	ugd->conform = ug_get_conformant();

	LEAVE();
}

static void __cst_on_pause(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	ENTER(__cst_on_pause);
}

static void __cst_on_resume(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	ENTER(__cst_on_resume);
}

static void __cst_on_destroy(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	WARN(">>");
	CstUgData_t *ugd = priv;
	ret_if(!ugd);
	ret_if(!ug);

	_cst_deregister_tel_event(ugd);
	_cst_close_msg_handle(ugd);
	_cst_unlisten_vconf_change();
	_cst_destroy_all_items(ugd);

	if (ugd->rejct_popup) {
		evas_object_del(ugd->rejct_popup);
		ugd->rejct_popup = NULL;
	}
	if (ugd->reject_list) {
		ugd->reject_list = eina_list_free(ugd->reject_list);
	}

	if (ugd->req_queue) {
		Eina_List *l;
		CallSettingSSReq_t *req;

		EINA_LIST_FOREACH(ugd->req_queue, l, req) {
			free(req);
		}
		ugd->req_queue = eina_list_free(ugd->req_queue);
	}

	if (ugd->sim_req_list) {
		g_slist_free_full(ugd->sim_req_list, g_free);
		ugd->sim_req_list = NULL;
	}

	if (ugd->popup != NULL) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	if (ugd->nf) {
		evas_object_del(ugd->nf);
		ugd->nf = NULL;
	}

	if (ugd->bg) {
		evas_object_del(ugd->bg);
		ugd->bg = NULL;
	}

	if (ugd->base) {
		evas_object_del(ugd->base);
		ugd->base = NULL;
	}

	elm_theme_extension_del(NULL, THEME_NAME);
	cst_util_feedback_deinit();
	LEAVE();
}

static void __cst_on_message(ui_gadget_h ug, app_control_h msg, app_control_h app_control, void *priv)
{
}

static void __cst_on_key_event(ui_gadget_h ug, enum ug_key_event event, app_control_h app_control, void *priv)
{
	ENTER(__cst_on_key_event);
	CstUgData_t *ugd = priv;
	ret_if(!ugd);
	ret_if(!ug);

	switch (event) {
	case UG_KEY_EVENT_END:
		if (ugd->popup != NULL) {
			DBG("popup=0x%p", ugd->popup);
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		} else if (ugd->back_button != NULL) {
			DBG("back_button = 0x%p", ugd->back_button);
			evas_object_smart_callback_call(ugd->back_button, "clicked", NULL);
		} else {
			DBG("Error: No action is defined for END KEY EVENT, popup=0x%p, back_button=0x%p", ugd->popup, ugd->back_button);
		}
		break;

	default:
		DBG("Unknown Event Detected, event=%d", event);
		break;
	}
}

static void __cst_on_event(ui_gadget_h ug, enum ug_event event, app_control_h app_control, void *priv)
{
	ENTER(__cst_on_event);
	CstUgData_t *ugd;

	if (!ug || !priv) {
		return;
	}

	ugd = priv;

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		DBG("UG_EVENT_LOW_MEMORY");
		break;
	case UG_EVENT_LOW_BATTERY:
		DBG("UG_EVENT_LOW_BATTERY");
		break;
	case UG_EVENT_LANG_CHANGE:
		DBG("UG_EVENT_LANG_CHANGE");
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		DBG("UG_EVENT_ROTATE_PORTRAIT");
		/* fall-through */
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		DBG("UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN");
		ugd->b_landscape_mode = EINA_FALSE;
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		DBG("UG_EVENT_ROTATE_LANDSCAPE");
		/* fall-through */
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		DBG("UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN");
		ugd->b_landscape_mode = EINA_TRUE;
		break;
	default:
		break;
	}
	LEAVE();
}

CST_MODULE_EXPORT int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	ENTER(UG_MODULE_INIT);

	CstUgData_t *ugd;

	if (!ops) {
		return -1;
	}

	ugd = calloc(1, sizeof(CstUgData_t));
	if (!ugd) {
		return -1;
	}

	ops->create = __cst_on_create;
	ops->start = __cst_on_start;
	ops->pause = __cst_on_pause;
	ops->resume = __cst_on_resume;
	ops->destroy = __cst_on_destroy;
	ops->message = __cst_on_message;
	ops->event = __cst_on_event;
	ops->key_event = __cst_on_key_event;
	ops->priv = ugd;
	ops->opt = UG_OPT_INDICATOR_ENABLE;
	LEAVE();
	return 0;
}

CST_MODULE_EXPORT void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	ENTER(UG_MODULE_EXIT);
	struct ug_data *ugd;

	if (!ops) {
		return;
	}

	ugd = ops->priv;
	if (ugd) {
		free(ugd);
	}
	LEAVE();
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
