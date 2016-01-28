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
#include <app.h>
#include <stdio.h>
#include <Elementary.h>
#include <Evas.h>
#include <vconf.h>
#include <notification.h>
#include <notification_internal.h>

#include "cst-common.h"
#include "cst-widget.h"
#include "cst-common-string.h"
#include "cst-debug.h"
#include "cst-reject-msg.h"
#include "cst-fdn-setting.h"
#include "cst-fdn-list.h"
#include "cst-forwarding.h"
#include "cst-barring.h"
#include "cst-util.h"
#include "cst-widget.h"
#include "cst-delete-list.h"
#include "cst-answering.h"
#include "cst-more-call-setting.h"
#include "cst-waiting.h"
#include "cst-tapi-request.h"
#include "cst-editfield.h"
#include <efl_extension.h>

#define CONTACT_BTN_COLOR_NORMAL	61, 185, 204, 255
#define CONTACT_BTN_COLOR_PRESSED	59, 148, 162, 255

static Evas_Object *g_entry = NULL;

static void __cst_2btn_popup_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void __cst_2btn_delete_popup_cancel_cb(void *data, Evas_Object *obj,
		void *event_info);

static void _cst_on_press_ime_contact_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *contact_icon = (Evas_Object *)
			elm_object_part_content_get(obj, "icon");

	if (contact_icon) {
		evas_object_color_set(contact_icon, CONTACT_BTN_COLOR_PRESSED);
	}

	if (ugd->dg_entry) {
		elm_object_focus_set(ugd->dg_entry, EINA_FALSE);
	}
}

static void _cst_on_unpress_ime_contact_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	Evas_Object *contact_icon = (Evas_Object *)
			elm_object_part_content_get(obj, "icon");
	if (contact_icon) {
		evas_object_color_set(contact_icon, CONTACT_BTN_COLOR_NORMAL);
	}
}

Evas_Object *_cst_create_processing_popup(Evas_Object *parent)
{
	Evas_Object *progressbar;
	Evas_Object *layout, *popup;
	retv_if(NULL == parent, NULL);
	popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);

	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, THEME_NAME, "popup_processingview_1button");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	progressbar = _cst_create_progressbar(popup);

	elm_object_part_content_set(layout, "elm.swallow.content", progressbar);
	elm_object_part_text_set(layout, "elm.text", T_(CST_STR_LOADING));

	elm_object_content_set(popup, layout);
	return popup;
}
Evas_Object *_cst_create_progressbar(Evas_Object *parent)
{
	ENTER(_cst_create_progressbar);
	retv_if(NULL == parent, NULL);
	Evas_Object *progressbar;
	progressbar = elm_progressbar_add(parent);
	elm_object_style_set(progressbar, "process_medium");
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(progressbar);
	elm_progressbar_pulse(progressbar, EINA_TRUE);

	return progressbar;
}

Evas_Object *_cst_create_scroller(Evas_Object* parent)
{
    Evas_Object *scroller = elm_scroller_add(parent);
	retv_if(NULL == scroller, NULL);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	elm_object_style_set(scroller, "dialogue");
    evas_object_show(scroller);

    return scroller;
}

Evas_Object *_cst_create_box(Evas_Object* parent)
{
	Evas_Object *box = elm_box_add(parent);
	retv_if(NULL == box, NULL);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, 0.0);
	elm_object_content_set(parent, box);
	evas_object_show(box);

    return box;
}

void _cst_create_entry_margin(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	ret_if(NULL == layout);
	elm_layout_file_set(layout, EDJ_NAME, "entry_margin");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	elm_box_pack_end(parent, layout);
	evas_object_show(layout);
}

void _cst_create_entry(Evas_Object *parent, Evas_Object *(*p_func)(), void *data, char *dialog_group_style)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *layout;

	layout = p_func(parent, ugd);
	elm_box_pack_end(parent, layout);
	evas_object_show(layout);

	elm_object_signal_emit(layout, dialog_group_style, "");
}

void _cst_create_editfield_dialogue_entry(Evas_Object *parent,
		Evas_Object* (*p_func)(), void *data, char *dialog_group_style)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *layout;

	layout = p_func(parent, ugd);
	elm_box_pack_end(parent, layout);

	/*Create the EditFiled Layout with Contact Icon*/
	elm_object_signal_emit(layout, "elm,state,edit,enabled", "");

	evas_object_show(layout);
	elm_object_signal_emit(layout, dialog_group_style, "");
}

/* Note: This function will return an allocated string. The caller should free it using g_free */
char *_cst_get_error_string(int error)
{
	const char *popup_message = NULL;
	switch (error) {
	case CST_ERROR_INCORRECT_OPERATION:
		popup_message = I_(CST_STR_INCORRECT_OPERATION);
		break;
	case CST_ERROR_INVALID_PASSWORD:
		popup_message = I_(CST_STR_INVALID_PASSWORD);
		break;
	case CST_ERROR_PASSWORD_BLOCKED:
		popup_message = I_(CST_STR_PASSWORD_BLOCKED);
		break;
	case CST_ERROR_SERVICE_UNAVAILABLE:
		popup_message = I_(CST_STR_SERVICE_UNAVAILABLE);
		break;
	case CST_ERROR_UNKNOWN:
		popup_message = I_(CST_STR_REJECTED_BY_NETWORK);
		break;
	case CST_ERROR_ENTER_NUMBER:
		popup_message = I_(CST_STR_ENTER_NUMBER);
		break;
	case CST_ERROR_WRONG_NUMBER:
		popup_message = I_(CST_STR_WRONG_NUMBER);
		break;
	case CST_ERROR_ENTER_MESSAGE:
		popup_message = I_(CST_STR_ENTER_MESSAGE);
		break;
	case CST_ERROR_SELECT_VALID_ENTRY:
		popup_message = I_(CST_STR_SELECT_VALID_ENTRY);
		break;
	case CST_ERROR_REJECTED_BY_NETWORK:
		popup_message = I_(CST_STR_REJECTED_BY_NETWORK);
		break;
	case CST_ERROR_OEM_NOT_SUPPORTED:
	case CST_ERROR_OEM_NOT_SUPPORTED_USER_BUSY:
	case CST_ERROR_OEM_NOT_SUPPORTED_NO_REPLY:
	case CST_ERROR_OEM_NOT_SUPPORTED_OUT_OF_REACH:	/* according to UX decision, all string should be regarded as one, "Rejected by network"..*/
		popup_message = I_(CST_STR_REJECTED_BY_NETWORK);
		break;
	case CST_ERROR_DUPLICATE_MESSAGE:
		popup_message = I_(CST_STR_ENTER_DUPLICATE_MESSAGE);
		break;
	case CST_ERROR_INSERT_SIM_CARD:
		popup_message = I_(CST_STR_INSERT_SIM_CARD);
		break;
	case CST_ERROR_CHANGE_FLIGHT_MODE:
		popup_message = I_(CST_STR_CHNAGE_FLIGHT_MODE_MESSAGE);
		break;
	case CST_ERROR_AUL_LAUNCH_ERROR:
		popup_message = I_(CST_STR_APP_NOT_INSTALLED);
		break;
	case CST_ERROR_CHANGE_VOICEMAIL_NUMBER:
		popup_message = I_(CST_STR_UNABLE_TO_CHANGE_VOICEMAIL_NUMBER);
		break;
	case CST_ERROR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED_PD:
		popup_message = I_(CST_STR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED_PD);
		break;
	case CST_ERROR_MAXIMUM_DIGITS_OF_PHONE_NUMBER_REACHED:
		popup_message = (const char *)g_strdup_printf(I_(CST_STR_POPUP_MAX_NUMBER_OF_DIGITS_REACHED), CST_MAX_PHONE_NUMBER_LEN);
		return (char *)popup_message;
	case CST_ERROR_FDN_MODE_ACTIVE:
		popup_message = I_(CST_STR_FDN_MODE_ACTIVATED);
		break;
	case CST_ERROR_NO_VOICEMAIL_NUM_CHANGED:
		popup_message = I_(CST_STR_NO_CHANGES_WERE_MADE);
		break;
	default:
		popup_message = I_(CST_STR_UNKNOWN_OPERATION);
		break;
	}

	return g_strdup(popup_message);
}

void _cst_gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *nxt_item = NULL;
	Elm_Object_Item *last_item = NULL;
	CstGlItemData_t *item_data = NULL;
	nxt_item = elm_genlist_first_item_get(obj);

	if ((long)data == CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST) {
		nxt_item = elm_genlist_item_next_get(nxt_item);
	} else {
		item_data = (CstGlItemData_t *)elm_object_item_data_get(nxt_item);
		while (item_data == NULL || item_data->index == -1) {
			nxt_item = elm_genlist_item_next_get(nxt_item);
			item_data = (CstGlItemData_t *)elm_object_item_data_get(nxt_item);
		}
	}

	last_item = elm_genlist_last_item_get(obj);

	if (((long)data == CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST) ||
			((long)data == CST_DIALOG_GROUP_CALL_FORWARDING_WAITING_TIME)) {
		if ((long)data == CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST)
			last_item = elm_genlist_item_prev_get(last_item);
	} else {
		item_data = (CstGlItemData_t *)elm_object_item_data_get(last_item);
		while (item_data == NULL || item_data->index == -1) {
			last_item = elm_genlist_item_prev_get(last_item);
			item_data = (CstGlItemData_t *)elm_object_item_data_get(last_item);
		}
		if ((long)data == CST_DIALOG_GROUP_END_WITH_HELP_TEXT) {
			last_item = elm_genlist_item_prev_get(last_item);
		}
	}

	if (nxt_item == last_item) {
		elm_object_item_signal_emit(nxt_item, "elm,state,default", "");
		return;
	}

	elm_object_item_signal_emit(nxt_item, "elm,state,top", "");

	elm_object_item_signal_emit(last_item, "elm,state,bottom", "");

	nxt_item = elm_genlist_item_next_get(nxt_item);

	while (nxt_item != NULL && nxt_item != last_item) {
		elm_object_item_signal_emit(nxt_item, "elm,state,center", "");
		nxt_item = elm_genlist_item_next_get(nxt_item);
	}
}

static void __cst_del_error_popup_with_ok_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;

	evas_object_del(ugd->popup);
	ugd->popup = NULL;

	if (ugd->dg_entry != NULL) {
		elm_object_focus_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_cursor_end_set(ugd->dg_entry);
	}
}

static void __cst_del_error_popup_destroy_ug_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	evas_object_smart_callback_del(obj, "block,clicked",
			__cst_del_error_popup_destroy_ug_cb);
	evas_object_smart_callback_del(obj, "timeout",
			__cst_del_error_popup_destroy_ug_cb);

	if (obj == ugd->popup) {
		ugd->popup = NULL;
	}
	evas_object_del(obj);

	if (ugd->ug) {
		ug_destroy_me(ugd->ug);
	}
}

Evas_Object *_cst_show_error_popup_destroy_ug(CstUgData_t *ugd, int error)
{
	ENTER(_cst_show_error_popup_destroy_ug);
	retv_if(error < 0, NULL);
	retv_if(ugd == NULL, NULL);

	char *error_msg;
	error_msg = _cst_get_error_string(error);
	DBG("Error message=%s", error_msg);

	Evas_Object *popup = NULL;
	popup = elm_popup_add(ugd->nf);
	elm_popup_timeout_set(popup, CST_POPUP_TIMER);
	cst_util_domain_translatable_text_set(popup, error_msg);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked",
			__cst_del_error_popup_destroy_ug_cb, ugd);
	evas_object_smart_callback_add(popup, "timeout",
			__cst_del_error_popup_destroy_ug_cb, ugd);
	g_free(error_msg);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			__cst_del_error_popup_destroy_ug_cb, ugd);
	evas_object_show(popup);

	return popup;
}

void _cst_create_error_popup(int error)
{
	ENTER(_cst_create_error_popup);
	ret_if(error < 0);

	char *error_msg;
	error_msg = _cst_get_error_string(error);
	DBG("Error message=%s", A_(error_msg));
	_cst_create_toast_popup(A_(error_msg));

	return;
}

Evas_Object *_cst_create_error_popup_with_ok_btn(CstUgData_t *ugd, int error)
{
	retv_if(error < 0, NULL);
	retv_if(ugd == NULL, NULL);

	char *error_msg;
	error_msg = _cst_get_error_string(error);
	DBG("Error message=%s", error_msg);

	Evas_Object *popup, *btn;
	ugd->popup = popup = elm_popup_add(ugd->nf);
	cst_util_domain_translatable_text_set(popup, error_msg);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	btn = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn, I_(CST_STR_SK_OK));
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", __cst_del_error_popup_with_ok_btn_cb, (void *)ugd);

	evas_object_show(popup);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
								__cst_del_error_popup_with_ok_btn_cb, (void *)ugd);

	g_free(error_msg);
	return popup;
}

Evas_Object *_cst_create_duplicate_msg_popup(Evas_Object *parent, const char *string)
{
	retv_if(string == NULL, NULL);

	char *error_msg = NULL;
	error_msg = g_strdup_printf(T_(CST_STR_ALREADY_IN_USE), string);
	DBG("Error message=%s", error_msg);

	Evas_Object *popup;
	popup = elm_popup_add(parent);
	elm_popup_timeout_set(popup, 1.5);
	cst_util_domain_translatable_text_set(popup, error_msg);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	g_free(error_msg);

	return popup;
}

static void __cst_2btn_popup_ok_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;

	elm_genlist_item_update(item_data->gl_item);

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

static void __cst_2btn_popup_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
	ugd->check_state = EINA_FALSE;
}

Evas_Object *_cst_create_2btn_ok_cancel_popup(Evas_Object *parent,
		CstGlItemData_t *item_data, int btn1_str_id, int btn2_str_id,
		const char *info_text, Evas_Smart_Cb btn1_func, Evas_Smart_Cb btn2_func)
{
	retv_if((parent == NULL) || (item_data == NULL), NULL);

	Evas_Object *popup, *btn1, *btn2;
	popup = elm_popup_add(parent);
	cst_util_domain_translatable_text_set(popup, info_text);

	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	btn1 = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn1, I_(btn1_str_id));

	elm_object_part_content_set(popup, "button1", btn1);
	if (btn1_func) {
		evas_object_smart_callback_add(btn1, "clicked", btn1_func,
									   (void *)item_data->ugd);
	} else {
		evas_object_smart_callback_add(btn1, "clicked",
									   __cst_2btn_popup_cancel_cb, (void *)item_data);
	}

	btn2 = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn2, I_(btn2_str_id));

	elm_object_part_content_set(popup, "button2", btn2);
	if (btn2_func) {
		evas_object_smart_callback_add(btn2, "clicked", btn2_func,
									   (void *)item_data->ugd);
	}

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
									__cst_2btn_popup_cancel_cb, (void *)item_data);

	return popup;
}

Eina_Bool _cst_popup_is_check_set(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;
	retv_if((data == NULL), EINA_FALSE);

	Evas_Object *lyt = elm_object_content_get(ugd->popup);
	if (NULL == lyt) {
		DBG("lyt == NULL");
		return EINA_FALSE;
	}

	Evas_Object *check = elm_object_part_content_get(lyt, "elm.swallow.end");
	if (NULL == check) {
		DBG("check == NULL");
		return EINA_FALSE;
	}

	return ((elm_check_state_get(check) == EINA_TRUE) ? EINA_TRUE : EINA_FALSE);
}

Evas_Object *_cst_create_2btn_check_popup(Evas_Object *parent, void *data,
		const char *info_text, const char *popup_title, Evas_Smart_Cb cb, Evas_Smart_Cb cncl_cb, void *cb_data, Evas_Smart_Cb check_callback_fn)
{
	retv_if((parent == NULL) || (data == NULL) || (cb_data == NULL), NULL);
	Evas_Object *check, *layout, *label;
	Evas_Object *btn1, *btn2;
	CstUgData_t *ugd = (CstUgData_t *)data;


	/* Destroy popup if exists */
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
	ugd->popup = elm_popup_add(parent);

	label = elm_label_add(ugd->popup);
	elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
	cst_util_domain_translatable_text_set(label, (info_text));
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	cst_util_domain_translatable_part_text_set(ugd->popup, "title,text", (popup_title));
	evas_object_show(label);

	layout = elm_layout_add(ugd->popup);
	elm_layout_file_set(layout, THEME_NAME, "popup_checkview");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	check = elm_check_add(ugd->popup);
	/*elm_object_style_set(check, "multiline");*/
	elm_object_text_set(check, "Don't ask again");
	evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (check_callback_fn)
		evas_object_smart_callback_add(check, "changed", check_callback_fn, cb_data);
	evas_object_show(check);

	elm_object_part_content_set(layout, "elm.swallow.content", label);
	elm_object_part_content_set(layout, "elm.swallow.end", check);

	evas_object_show(layout);
	elm_object_content_set(ugd->popup, layout);
	btn1 = elm_button_add(ugd->popup);
	elm_object_style_set(btn1, "popup");
	elm_object_text_set(btn1, "Enable");
	elm_object_part_content_set(ugd->popup, "button1", btn1);
	if (cb)
		evas_object_smart_callback_add(btn1, "clicked", cb, cb_data);
	btn2 = elm_button_add(ugd->popup);
	elm_object_style_set(btn2, "popup");
	elm_object_text_set(btn2, "Cancel");
	elm_object_part_content_set(ugd->popup, "button2", btn2);
	if (cncl_cb)
		evas_object_smart_callback_add(btn2, "clicked", cncl_cb, cb_data);
	else
		evas_object_smart_callback_add(btn2, "clicked", __cst_2btn_popup_cancel_cb, cb_data);
	return (ugd->popup);
}

Evas_Object *_cst_create_2btn_popup(Evas_Object *parent, void *data,
		const char *info_text, Evas_Smart_Cb cb, void *cb_data)
{
	retv_if((parent == NULL) || (data == NULL), NULL);
	Evas_Object *popup, *btn1, *btn2;
	popup = elm_popup_add(parent);
	cst_util_domain_translatable_text_set(popup, info_text);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	btn1 = elm_button_add(popup);
	elm_object_style_set(btn1, "popup");
	cst_util_domain_translatable_text_set(btn1, I_(CST_STR_POP_CANCEL));
	elm_object_part_content_set(popup, "button1", btn1);
	if (cb) {
		evas_object_smart_callback_add(btn1, "clicked", cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn1, "clicked", __cst_2btn_popup_cancel_cb,
				cb_data);
	}

	btn2 = elm_button_add(popup);
	elm_object_style_set(btn2, "popup");
	cst_util_domain_translatable_text_set(btn2, I_(CST_STR_SK_OK));
	elm_object_part_content_set(popup, "button2", btn2);
	if (cb) {
		evas_object_smart_callback_add(btn2, "clicked", cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn2, "clicked", __cst_2btn_popup_ok_cb,
				cb_data);
	}

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
									__cst_2btn_popup_cancel_cb, cb_data);
	evas_object_show(popup);
	return popup;
}

Evas_Object *_cst_create_1btn_popup(Evas_Object *parent, void *data,
		const char *info_text, const char *title_text, Evas_Smart_Cb cb,
		void *cb_data)
{
	retv_if((parent == NULL) || (data == NULL), NULL);
	Evas_Object *popup = NULL, *btn1 = NULL;
	popup = elm_popup_add(parent);
	cst_util_domain_translatable_text_set(popup, info_text);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (title_text) {
		cst_util_domain_translatable_part_text_set(popup, "title,text",
				title_text);
	}

	btn1 = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn1, I_(CST_STR_SK_OK));
	elm_object_part_content_set(popup, "button1", btn1);
	if (cb) {
		evas_object_smart_callback_add(btn1, "clicked", cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn1, "clicked",
				__cst_2btn_delete_popup_cancel_cb, cb_data);
	}

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			__cst_2btn_delete_popup_cancel_cb, cb_data);
	evas_object_show(popup);
	return popup;
}

static void __cst_2btn_delete_popup_cancel_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

Evas_Object *_cst_create_2btn_delete_popup(Evas_Object *parent, void *data,
		const char *info_text, Evas_Smart_Cb cb, void *cb_data)
{
	retv_if((parent == NULL) || (data == NULL), NULL);
	Evas_Object *popup, *btn1, *btn2;
	popup = elm_popup_add(parent);
	cst_util_domain_translatable_text_set(popup, info_text);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	cst_util_domain_translatable_part_text_set(popup, "title,text", I_(CST_STR_HEADER_DELETE));

	btn1 = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn1, I_(CST_STR_POP_CANCEL));
	elm_object_part_content_set(popup, "button1", btn1);
	evas_object_smart_callback_add(btn1, "clicked",
			__cst_2btn_delete_popup_cancel_cb, data);

	btn2 = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn2, I_(CST_STR_HEADER_DELETE));
	elm_object_part_content_set(popup, "button2", btn2);
	if (cb) {
		evas_object_smart_callback_add(btn2, "clicked", cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn2, "clicked", __cst_2btn_popup_ok_cb,
				cb_data);
	}

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			__cst_2btn_delete_popup_cancel_cb, data);
	evas_object_show(popup);
	return popup;
}

Evas_Object *_cst_get_navifr_prev_btn(Evas_Object *nf)
{
	ENTER(_cst_get_navifr_prev_btn);
	Elm_Object_Item *top_it = elm_naviframe_top_item_get(nf);
	retv_if(top_it == NULL, NULL);
	Evas_Object *btn = elm_object_item_part_content_get(top_it, "prev_btn");
	retv_if(btn == NULL, NULL);

	return btn;
}

static Evas_Object *__cst_create_title_left_btn_text_style(Evas_Object *parent,
		const char *text, Evas_Smart_Cb func, void *data)
{
	retv_if(NULL == parent, NULL);
	Evas_Object *btn = elm_button_add(parent);
	elm_object_style_set(btn, "naviframe/title_left");
	cst_util_domain_translatable_text_set(btn, text);
	evas_object_smart_callback_add(btn, "clicked", func, data);
	evas_object_show(btn);
	return btn;
}

static Evas_Object *__cst_create_title_right_btn_text_style(Evas_Object *parent,
		const char *text, Evas_Smart_Cb func, void *data)
{
	retv_if(NULL == parent, NULL);
	Evas_Object *btn = elm_button_add(parent);
	elm_object_style_set(btn, "naviframe/title_right");
	cst_util_domain_translatable_text_set(btn, text);
	evas_object_smart_callback_add(btn, "clicked", func, data);
	evas_object_show(btn);
	return btn;
}

static Evas_Object *__cst_create_title_btn_icon_style(Evas_Object *parent,
		const char *icon_path, const char *label, Evas_Smart_Cb func, void *data)
{
	retv_if(NULL == parent, NULL);
	Evas_Object *btn = elm_button_add(parent);
	elm_object_style_set(btn, "naviframe/title_done");
	cst_util_domain_translatable_text_set(btn, label);
#ifdef _CALL_SET_TTS_SUPPORT
	if (label) {
		if (strstr(label, "IDS_COM") && _cst_text_id_is_common(label)) {
			elm_access_info_set(btn, ELM_ACCESS_INFO, dgettext("sys_string", label));
		} else {
			elm_access_info_set(btn, ELM_ACCESS_INFO, label);
		}
	}
#endif
	evas_object_smart_callback_add(btn, "clicked", func, data);
	evas_object_show(btn);
	return btn;
}

static Evas_Object *__cst_create_title_btn_icon_style_cancel(Evas_Object *parent,
		const char *icon_path, const char *label, Evas_Smart_Cb func, void *data)
{
	retv_if(NULL == parent, NULL);
	Evas_Object *btn = elm_button_add(parent);
	elm_object_style_set(btn, "naviframe/title_cancel");
	cst_util_domain_translatable_text_set(btn, label);
#ifdef _CALL_SET_TTS_SUPPORT
	if (label) {
		if (strstr(label, "IDS_COM") && _cst_text_id_is_common(label)) {
			elm_access_info_set(btn, ELM_ACCESS_INFO, dgettext("sys_string", label));
		} else {
			elm_access_info_set(btn, ELM_ACCESS_INFO, label);
		}
	}
#endif
	evas_object_smart_callback_add(btn, "clicked", func, data);
	evas_object_show(btn);
	return btn;
}

void _cst_create_navi_control_bar(Evas_Object *parent,
		char *label1, const char *icon1, Evas_Smart_Cb cb1,
		char *label2, const char *icon2, Evas_Smart_Cb cb2,
		void *data, Elm_Object_Item *navi_it,
		Evas_Object **c_item)
{
	ENTER(_cst_create_navi_control_bar);
	ret_if(NULL == parent);
	ret_if(NULL == navi_it);
	Evas_Object *btn = NULL;

	elm_object_item_part_content_set(navi_it, "title_left_btn", NULL);
	elm_object_item_part_content_set(navi_it, "title_right_btn", NULL);

	if (icon1) {
		btn = __cst_create_title_btn_icon_style_cancel(parent, icon1, label1, cb1, data);
		ret_if(NULL == btn);
		elm_object_item_part_content_set(navi_it, "title_left_btn", btn);
		if (c_item)
			c_item[0] = btn;
	} else if (label1) {
		btn = __cst_create_title_left_btn_text_style(parent, label1, cb1, data);
		ret_if(NULL == btn);
		elm_object_item_part_content_set(navi_it, "title_left_btn", btn);
		if (c_item)
			c_item[0] = btn;
	}

	if (icon2) {
		btn = __cst_create_title_btn_icon_style(parent, icon2, label2, cb2, data);
		ret_if(NULL == btn);
		elm_object_item_part_content_set(navi_it, "title_right_btn", btn);
		if (c_item)
			c_item[1] = btn;
	} else if (label2) {
		btn = __cst_create_title_right_btn_text_style(parent, label2, cb2, data);
		ret_if(NULL == btn);
		elm_object_item_part_content_set(navi_it, "title_right_btn", btn);
		if (c_item)
			c_item[1] = btn;
	}
}

void _cst_create_2btn_navi_toolbar(Evas_Object *parent, char *label1,
		const char *icon1, Evas_Smart_Cb cb1, char *label2, const char *icon2,
		Evas_Smart_Cb cb2, void *data, Elm_Object_Item *navi_it,
		Elm_Object_Item **c_obj_item)
{
	ENTER(_cst_create_2btn_navi_toolbar);
	ret_if(NULL == parent);
	ret_if(NULL == navi_it);
	Elm_Object_Item *obj = NULL;
	Evas_Object *toolbar = NULL;

	/* create toolbar */
	toolbar = elm_toolbar_add(parent);
	if (toolbar == NULL)
		return;

	elm_object_style_set(toolbar, "default");
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_NONE);

	if (label1) {
		obj = elm_toolbar_item_append(toolbar, icon1, label1, cb1, data);
		cst_util_item_domain_text_translatable_set(obj, label1);
		if (c_obj_item)
			c_obj_item[0] = obj;
	}

	if (label2) {
		obj = elm_toolbar_item_append(toolbar, icon2, label2, cb2, data);
		cst_util_item_domain_text_translatable_set(obj, label2);
		if (c_obj_item)
			c_obj_item[1] = obj;
	}
	elm_object_item_part_content_set(navi_it, "toolbar", toolbar);
	return;
}

static void __cst_gl_del_seperator(void *data, Evas_Object *obj)
{
	ret_if(data == NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

void _cst_create_genlist_separator(Evas_Object *genlist, Eina_Bool need_item_data)
{
	Elm_Object_Item *item = NULL;
	static Elm_Genlist_Item_Class itc_seperator;
	itc_seperator.item_style = "dialogue/separator";
	itc_seperator.func.text_get = NULL;
	itc_seperator.func.content_get = NULL;
	itc_seperator.func.state_get = NULL;
	itc_seperator.func.del = NULL;

	if (need_item_data) {
		CstGlItemData_t *item_data;
		itc_seperator.func.del = __cst_gl_del_seperator;
		item_data = calloc(1, sizeof(CstGlItemData_t));
		ret_if(NULL == item_data);
		item_data->index = -1;
		item = elm_genlist_item_append(genlist, &itc_seperator, item_data, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item_data->gl_item = item;
	} else {
		item = elm_genlist_item_append(genlist, &itc_seperator, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
}

#if 0 /* Function not being used. */
static void cst_toast_popup_block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
}

static void cst_toast_popup_timeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
}
#endif

void _cst_create_toast_popup(const char *text)
{
	ret_if(text == NULL);
	notification_status_message_post(text);

	return ;

}

void _cst_create_indicator_popup(const char *text)
{
	ret_if(text == NULL);

	notification_status_message_post(text);

	return ;

}

Evas_Object *_cst_create_onoff_button(Evas_Object *obj, int state_value, Evas_Smart_Cb cb_func, void *cb_data)
{
	ENTER(_cst_create_onoff_button);
	retv_if(NULL == obj, NULL);

	Evas_Object *btn = NULL;

	btn = elm_check_add(obj);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_style_set(btn, "on&off");
	elm_check_state_set(btn, state_value);

	if (cb_func) {
		evas_object_smart_callback_add(btn, "changed", cb_func, cb_data);
	}
	evas_object_propagate_events_set(btn, EINA_FALSE);

	return btn;
}

Evas_Object *_cst_create_radio_icon(Evas_Object *obj,
									Evas_Object *rdg, int value, int offset, const char *vconf_key)
{
	retv_if(NULL == obj, NULL);

	int rd_value = 0;
	Evas_Object *radio = elm_radio_add(obj);
	elm_radio_state_value_set(radio, value);
	elm_radio_group_add(radio, rdg);
	if (vconf_key == NULL) {
		elm_radio_value_set(rdg, offset);
	} else {
		Eina_Bool result = _cst_vconf_get_int(vconf_key, &rd_value);
		if (result == EINA_FALSE) {
			ERR("_cst_vconf_get_int(%s) failed!!!", vconf_key);
		} else {
			elm_radio_value_set(rdg, (rd_value + offset));
		}
	}

	evas_object_propagate_events_set(radio, EINA_FALSE);

	return radio;
}

Evas_Object *_cst_create_title_btn_icon_only(Evas_Object *parent,
								   const char *text, Evas_Smart_Cb func, void *data)
{
	retv_if(NULL == parent, NULL);

	Evas_Object *btn = elm_button_add(parent);
	if (!btn) {
		return NULL;
	}
	if (strcmp(text, (char *)T_(CST_STR_POP_CANCEL)) == 0)
		elm_object_style_set(btn, "naviframe/back_btn/default");
	else {
		Evas_Object *ic = NULL;
		elm_object_style_set(btn, "naviframe/title_icon");
		ic = elm_icon_add(btn);
		if (strcmp(text, (char *)T_(CST_STR_SAVE)) == 0) {
			elm_image_file_set(ic, CST_TITLE_BTN_ICON_PLUS, NULL);
		} else if (strcmp(text, (char *)T_(CST_STR_SELECT_ALL)) == 0) {
			elm_image_file_set(ic, CST_TITLE_BTN_ICON_SELECT_ALL, NULL);
		} else {
			elm_image_file_set(ic, CST_TITLE_BTN_ICON_EDIT, NULL);
		}
		elm_image_aspect_fixed_set(ic, EINA_TRUE);
		elm_image_fill_outside_set(ic, EINA_TRUE);
		evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
						EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
						 EVAS_HINT_EXPAND);
		elm_image_resizable_set(ic, EINA_TRUE, EINA_TRUE);
		elm_object_content_set(btn, ic);
	}
	evas_object_smart_callback_add(btn, "clicked", func, data);
	return btn;
}

void _cst_transition_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	Elm_Object_Item *it1 = (Elm_Object_Item *)data;

	ENTER("_cst_transition_cb");
	if (it == it1) {
		DBG("g_entry = 0x%x", g_entry);
		elm_object_focus_set(g_entry, EINA_TRUE);
	}
}

static void __cst_widget_entry_next_pressed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	DBG("__cst_widget_entry_next_pressed_cb");
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (obj && ugd->nxt_dg_entry_focus) {
		elm_object_focus_set(obj, EINA_FALSE);
		elm_entry_cursor_end_set(ugd->nxt_dg_entry_focus);
		elm_object_focus_set(ugd->nxt_dg_entry_focus, EINA_TRUE);
	} else if (obj) {
		elm_object_focus_set(obj, EINA_FALSE);
	}
}

static void __cst_widget_entry_done_pressed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	DBG("__cst_widget_entry_done_pressed_cb");

	if (obj)
		elm_object_focus_set(obj, EINA_FALSE);
}

Evas_Object *_cst_create_ime_editfield(CstUgData_t *ugd, Evas_Object *parent,
		CstImeType_t ime_type, char *input_string)
{
	retv_if((NULL == ugd || NULL == parent), NULL);
	Ecore_IMF_Input_Panel_Layout panel_layout;
	Elm_Entry_Filter_Limit_Size limit_filter_data;
	Elm_Entry_Filter_Accept_Set digits_filter_data;
	Ecore_IMF_Context *imf_context;
	void (*input_panel_cb)() = NULL;
	void (*entry_changed_cb)() = NULL;
	void (*entry_activated_cb)() = NULL;
	Eina_Bool ret = EINA_FALSE;
	Evas_Object *layout = NULL;
	Evas_Object *entry_layout = NULL;
	Evas_Object *eo = NULL;

	if (CST_IME_CALL_FORWARD == ime_type ||
			CST_IME_FDN_SETTING == ime_type || CST_IME_FDN_DELETE == ime_type ||
			CST_IME_FDN_CONTACT_PIN2 == ime_type || CST_IME_CALL_BAR == ime_type ||
			CST_IME_FDN_CONTACT_NAME == ime_type || CST_IME_FDN_CONTACT_NUMBER == ime_type) {
		/* Set the Entry part of the Edit-field */
		entry_layout = editfield_create(parent, ET_SINGLELINE, NULL);
		ugd->dg_entry = editfield_get_entry(entry_layout);
		eo = entry_layout;
	} else if (CST_IME_REJECT_MSG == ime_type) {
		layout = elm_layout_add(parent);
		elm_layout_file_set(layout, EDJ_NAME, "editfield_layout");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);

		Evas_Object *entry_bg = elm_layout_add(layout);
		elm_layout_file_set(entry_bg, EDJ_NAME, "editfield_bg");

		entry_layout = editfield_create(parent, ET_MULTILINE, NULL);
		elm_object_part_content_set(entry_bg, "editfield", entry_layout);
		ugd->dg_entry = editfield_get_entry(entry_layout);
		elm_object_part_content_set(layout, "entry_part", entry_bg);

		ugd->entry_count = layout;

		eo = layout;
	} else {
		layout = elm_layout_add(parent);
		ret = elm_layout_theme_set(layout, "layout", "entry",
				"default");

		if (ret == EINA_FALSE) {
			DBG("elm_layout_theme_set() failed");
			evas_object_del(layout);
			return NULL;
		}
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(layout, EVAS_HINT_FILL,
				0.0);

		/* Set the Entry part of the Edit-field layout */
		entry_layout = editfield_create(parent, ET_SINGLELINE, NULL);

		elm_object_part_content_set(layout, "elm.icon.entry", entry_layout);
		ugd->dg_entry = editfield_get_entry(entry_layout);
		eo = layout;
	}

	switch (ime_type) {
	case CST_IME_REJECT_MSG:
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_ENTER_MESSAGE));
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NORMAL;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		elm_entry_markup_filter_append(ugd->dg_entry, _cst_reject_msg_entry_filter_check_boundary, ugd);
		input_panel_cb = _cst_reject_msg_input_panel_event_callback;
		entry_changed_cb = _cst_reject_msg_changed_editfield_cb;
		break;
	case CST_IME_CALL_FORWARD:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		if (strlen(ugd->entry_string) > 0) {
			elm_entry_entry_set(ugd->dg_entry, ugd->entry_string);
		}
		if (NULL == input_string) {
			cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_NUMBER));
		}
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_PHONENUMBER;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = CST_MAX_CF_NUMBER_LEN;
		digits_filter_data.accepted = "0123456789+*#";
		digits_filter_data.rejected = NULL;
		elm_entry_markup_filter_append(ugd->dg_entry,
				elm_entry_filter_accept_set, &digits_filter_data);
		input_panel_cb = _cst_call_forwarding_input_panel_event_callback;
		entry_changed_cb = _cst_call_forward_entry_changed_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		break;

#ifdef _CALL_SET_BARRING_SUPPORT
	case CST_IME_CALL_BAR:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_ENTER_BARRING_PASSWORD));

		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NUMBERONLY;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		elm_entry_password_set(ugd->dg_entry, EINA_TRUE);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = CST_MAX_PASSWORD_LEN;
		input_panel_cb = _cst_call_barring_input_panel_event_callback;
		entry_changed_cb = _cst_call_barring_entry_changed_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		break;
#endif  /* _CALL_SET_BARRING_SUPPORT */

#ifdef _CALL_SET_FDN_SUPPORT
	case CST_IME_FDN_SETTING:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_PIN2));
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NUMBERONLY;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		elm_entry_password_set(ugd->dg_entry, EINA_TRUE);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = CST_MAX_PASSWORD_LEN*2;
		input_panel_cb = _cst_FDN_setting_input_panel_event_cb;
		entry_changed_cb = _cst_FDN_setting_changed_editfield_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		break;
	case CST_IME_FDN_CONTACT_NAME:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_BODY_NAME));
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NORMAL;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = ugd->fdn_contact_name_len_max;
		ugd->dg_entry_contact_name = ugd->dg_entry;
		input_panel_cb = _cst_fdn_list_input_panel_event_cb;
		entry_changed_cb = _cst_fdn_list_changed_editfield_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
		entry_activated_cb = __cst_widget_entry_next_pressed_cb;
		ugd->nxt_dg_entry_focus = NULL;
		break;

	case CST_IME_FDN_CONTACT_NUMBER:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_NUMBER));
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_PHONENUMBER;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = ugd->fdn_contact_number_len_max;
		digits_filter_data.accepted = "0123456789+*#,(/)N.;- ";
		digits_filter_data.rejected = NULL;
		elm_entry_markup_filter_append(ugd->dg_entry,
				elm_entry_filter_accept_set, &digits_filter_data);
		ugd->dg_entry_contact_number = ugd->dg_entry;
		input_panel_cb = _cst_fdn_list_input_panel_event_cb;
		entry_changed_cb = _cst_fdn_list_changed_editfield_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		ugd->nxt_dg_entry_focus = ugd->dg_entry_contact_number;
		break;
	case CST_IME_FDN_CONTACT_PIN2:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_PIN2));
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NUMBERONLY;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		elm_entry_password_set(ugd->dg_entry, EINA_TRUE);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = CST_MAX_PASSWORD_LEN*2;
		ugd->dg_entry_pin2 = ugd->dg_entry;
		input_panel_cb = _cst_fdn_list_input_panel_event_cb;
		entry_changed_cb = _cst_fdn_list_changed_editfield_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		break;
	case CST_IME_FDN_DELETE:
		elm_entry_single_line_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ugd->dg_entry, EINA_TRUE);
		cst_util_domain_translatable_part_text_set(ugd->dg_entry, "elm.guide", I_(CST_STR_PIN2));
		panel_layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NUMBERONLY;
		elm_entry_input_panel_layout_set(ugd->dg_entry, panel_layout);
		elm_entry_password_set(ugd->dg_entry, EINA_TRUE);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = CST_MAX_PASSWORD_LEN*2;
		input_panel_cb = _cst_FDN_delete_list_input_panel_event_cb;
		entry_changed_cb = _cst_FDN_delete_list_changed_editfield_cb;
		elm_entry_input_panel_return_key_type_set(ugd->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		break;
#endif  /* _CALL_SET_FDN_SUPPORT */

	default:
		ERR("Invalid ime type.");
		return NULL;
	}

	if (CST_IME_REJECT_MSG != ime_type) {
		elm_entry_markup_filter_append(ugd->dg_entry,
				elm_entry_filter_limit_size, &limit_filter_data);
	}
	elm_entry_cnp_mode_set(ugd->dg_entry, ELM_CNP_MODE_PLAINTEXT);

	if (input_string != NULL) {
		elm_entry_entry_set(ugd->dg_entry, input_string);
	}

	elm_entry_cursor_end_set(ugd->dg_entry);

	imf_context = elm_entry_imf_context_get(ugd->dg_entry);

	DBG("ugd->entry = 0x%x, imf_context = 0x%x", ugd->dg_entry, imf_context);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_add(imf_context,
				ECORE_IMF_INPUT_PANEL_STATE_EVENT, input_panel_cb, ugd);
		if (ime_type == CST_IME_FDN_CONTACT_NAME) {
			if (0 > ugd->genlist_editfield_initialized) {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_FALSE);
				ugd->genlist_editfield_initialized++;
			} else {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_TRUE);
			}
		} else if (ime_type ==
				CST_IME_FDN_SETTING || ime_type == CST_IME_FDN_DELETE ||
				ime_type == CST_IME_CALL_BAR) {
			if (0 > ugd->genlist_editfield_initialized) {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_FALSE);
				ugd->genlist_editfield_initialized++;
			} else {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_TRUE);
			}
 		} else if (ime_type != CST_IME_REJECT_MSG && ime_type !=
				CST_IME_FDN_CONTACT_NUMBER && ime_type !=
				CST_IME_FDN_CONTACT_PIN2) {
			if (0 > ugd->genlist_editfield_initialized) {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_FALSE);
				ugd->genlist_editfield_initialized++;
			} else {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_TRUE);
			}
		}
	}

	if (ugd->is_app_control_invoked) {
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	if (NULL != entry_changed_cb) {
		DBG("entry_changed_cb = 0x%x", entry_changed_cb);
		evas_object_smart_callback_add(ugd->dg_entry, "changed",
				entry_changed_cb, ugd);
		evas_object_smart_callback_add(ugd->dg_entry, "preedit,changed",
				entry_changed_cb, ugd);
	}

	if (NULL != entry_activated_cb) {
		evas_object_smart_callback_add(ugd->dg_entry, "activated",
				entry_activated_cb, ugd);
	}

	if (ugd->b_expanded == EINA_FALSE) {
		evas_object_show(ugd->dg_entry);
		if (ime_type != CST_IME_FDN_CONTACT_NUMBER && ime_type !=
				CST_IME_FDN_CONTACT_PIN2) {
			g_entry = ugd->dg_entry;
		}

	}

	return eo;
}

Evas_Object *_cst_create_ime_contacts_btn_obj(Evas_Object *parent, void *data)
{
	retv_if(NULL == parent, NULL);
	retv_if(NULL == data, NULL);

	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *contact_icon = NULL;

	if (ugd->contact_btn != NULL) {
		ugd->contact_btn = NULL;
	}
	ugd->contact_btn = elm_button_add(parent);
	elm_object_style_set(ugd->contact_btn, "cst_transparent");
	contact_icon = elm_image_add(ugd->contact_btn);
	if (!contact_icon) {
		DBG("elm_icon_add() failed");
		return NULL;
	}
	elm_image_file_set(contact_icon, CST_CTRL_ICON_CONTACTS_DEF_IMG, NULL);
	elm_image_resizable_set(contact_icon, EINA_TRUE, EINA_TRUE);
	evas_object_color_set(contact_icon, CONTACT_BTN_COLOR_NORMAL);
	elm_object_part_content_set(ugd->contact_btn, "icon", contact_icon);

	evas_object_smart_callback_add(ugd->contact_btn, "pressed",
					_cst_on_press_ime_contact_btn, (void *)ugd);
	evas_object_smart_callback_add(ugd->contact_btn, "unpressed",
					_cst_on_unpress_ime_contact_btn, (void *)ugd);
	evas_object_smart_callback_add(ugd->contact_btn, "clicked",
					_cst_on_click_ime_contact_btn, (void *)ugd);
	evas_object_show(ugd->contact_btn);

	return ugd->contact_btn;
}

Elm_Genlist_Item_Class *_cst_create_genlist_item_class(
	const char *item_style,
	Elm_Gen_Item_Text_Get_Cb text_get_fn,
	Elm_Gen_Item_Content_Get_Cb content_get_fn,
	Elm_Gen_Item_State_Get_Cb state_get_fn, Elm_Gen_Item_Del_Cb del_fn)
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	retvm_if(!itc, NULL, "itc is NULL");
	itc->item_style = item_style;
	itc->func.text_get = text_get_fn;
	itc->func.content_get = content_get_fn;
	itc->func.state_get = state_get_fn;
	itc->func.del = del_fn;
	return itc;
}

void _cst_update_genlist_item_class(Elm_Object_Item *item, Elm_Genlist_Item_Class *itc)
{
	ret_if(NULL == item);
	ret_if(NULL == itc);
	const Elm_Genlist_Item_Class *prev_itc = NULL;

	prev_itc = elm_genlist_item_item_class_get(item);
	if (prev_itc) {
		elm_genlist_item_class_unref((Elm_Genlist_Item_Class *)prev_itc);
	}
	elm_genlist_item_item_class_update(item, itc);
	elm_genlist_item_class_ref(itc);
}

void _cst_destroy_genlist_item_class(Elm_Genlist_Item_Class *itc)
{
	if (itc) {
		itc->func.text_get = NULL;
		itc->func.content_get = NULL;
		itc->func.state_get = NULL;
		itc->func.del = NULL;
		elm_genlist_item_class_free(itc);
		itc = NULL;
	}
}

void _cst_terminate_service(app_control_h service)
{
	ENTER(_cst_terminate_service);
	ret_if(NULL == service);

	if (service) {
		app_control_send_terminate_request(service);
		app_control_destroy(service);
	}
}

void _cst_destroy_all_items(CstUgData_t *ugd)
{
	ret_if(ugd == NULL);

	_cst_destroy_answering_call();
	_cst_destroy_delete_list();
	_cst_destroy_call_forwarding();
	_cst_destroy_more_call_setting();
	_cst_destroy_reject_message(ugd);

#ifdef _CALL_SET_DUAL_SIM_ALWAYSON
	_cst_destroy_dualsim_alwayson(ugd);
#endif

#ifdef _CALL_SET_BARRING_SUPPORT
	_cst_destroy_call_barring();
#endif  /* _CALL_SET_BARRING_SUPPORT */

#ifdef _CALL_SET_FDN_SUPPORT
	_cst_destroy_fdn_list(ugd);
	_cst_destroy_fdn_setting();
#endif  /* _CALL_SET_FDN_SUPPORT */
}

#ifdef _TIZEN_LITE_CODE
static void __cst_set_toolbar_item_icon(Elm_Object_Item *item, int simIndex)
{
	if (simIndex == 0) {
		elm_toolbar_item_icon_file_set(item, EDJ_NAME, "SIM1_CARD");
	} else if (simIndex == 1) {
		elm_toolbar_item_icon_file_set(item, EDJ_NAME, "SIM2_CARD");
	}
}
void _cst_create_dual_sim_tabbar(Elm_Object_Item *navi_it, Evas_Smart_Cb
		sim1_func, Evas_Smart_Cb sim2_func, void *data)
{
	ENTER(_cst_create_dual_sim_tabbar);
	ret_if(NULL == data);

	Evas_Object *tabbar = NULL;
	Elm_Object_Item *item[2] = {0, };
	CstUgData_t *ugd = (CstUgData_t *)data;
	Eina_Bool is_sim1_present = EINA_FALSE;
	Eina_Bool is_sim2_present = EINA_FALSE;
	char *sim_name = NULL;
	/* create toolbar */
	tabbar = elm_toolbar_add(ugd->nf);
	if (tabbar == NULL) {
		DBG("tabbar is NULL");
		return;
	}

	elm_object_style_set(tabbar, "tabbar_with_title");
	elm_toolbar_shrink_mode_set(tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_reorder_mode_set(tabbar, EINA_FALSE);
	elm_toolbar_transverse_expanded_set(tabbar, EINA_TRUE);

	sim_name = vconf_get_str(VCONFKEY_SETAPPL_SIM1_NAME);
	item[0] = elm_toolbar_item_append(tabbar, NULL, sim_name, sim1_func, data);
	__cst_set_toolbar_item_icon(item[0], 0);
	cst_util_item_domain_text_translatable_set(item[0], sim_name);

	sim_name = vconf_get_str(VCONFKEY_SETAPPL_SIM2_NAME);
	item[1] = elm_toolbar_item_append(tabbar, NULL, sim_name, sim2_func, data);
	__cst_set_toolbar_item_icon(item[1], 1);
	cst_util_item_domain_text_translatable_set(item[1], sim_name);

	elm_object_item_part_content_set(navi_it, "tabbar", tabbar);

	elm_toolbar_select_mode_set(tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	ugd->sim1_btn = item[0];
	ugd->sim2_btn = item[1];

	is_sim1_present = _cst_is_sim_present_by_simslot(data, CST_SELECTED_SIM1);
	is_sim2_present = _cst_is_sim_present_by_simslot(data, CST_SELECTED_SIM2);

	if (is_sim1_present && is_sim2_present) { /*Both SIM's inserted*/
		DBG("Both SIM's inserted");
		elm_toolbar_item_selected_set(item[0], EINA_TRUE);
		ugd->sel_sim = CST_SELECTED_SIM1;
		_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM1);
	} else if (is_sim2_present && !is_sim1_present) { /*Only SIM2 inserted*/
		DBG("Only SIM2 inserted");
		elm_toolbar_item_selected_set(item[1], EINA_TRUE);
		ugd->sel_sim = CST_SELECTED_SIM2;
		_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM2);
		elm_object_item_disabled_set(ugd->sim1_btn, EINA_TRUE);
	} else { /*Only SIM1 inserted*/
		DBG("Only SIM1 inserted");
		elm_toolbar_item_selected_set(item[0], EINA_TRUE);
		ugd->sel_sim = CST_SELECTED_SIM1;
		_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM1);
		elm_object_item_disabled_set(ugd->sim2_btn, EINA_TRUE);
	}
}
#endif /* _TIZEN_LITE_CODE */
