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
#include "cst-forwarding.h"
#include "cst-util.h"
#include "cst-widget.h"
#include "cst-delete-list.h"
#include "cst-answering.h"
#include "cst-more-call-setting.h"
#include "cst-waiting.h"
#include "cst-tapi-request.h"
#include "cst-editfield.h"
#include <efl_extension.h>

static Evas_Object *g_entry = NULL;

static void _cst_on_press_ime_contact_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data);
	CstAppData_t *ad = (CstAppData_t *)data;
	Evas_Object *contact_icon = (Evas_Object *)elm_object_part_content_get(obj, "icon");

	if (contact_icon) {
		cst_util_feedback_play_tap_sound();
		edje_object_signal_emit(contact_icon, "icon,press", "cst");
	}

	if (ad->dg_entry) {
		elm_object_focus_set(ad->dg_entry, EINA_FALSE);
	}
}

static void _cst_on_unpress_ime_contact_btn(void *data, Evas_Object *obj,
		void *event_info)
{
	Evas_Object *contact_icon = (Evas_Object *)elm_object_part_content_get(obj, "icon");
	if (contact_icon) {
		edje_object_signal_emit(contact_icon, "icon,unpress", "cst");
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
	case CST_ERROR_NO_VOICEMAIL_NUM_CHANGED:
		popup_message = I_(CST_STR_NO_CHANGES_WERE_MADE);
		break;
	default:
		popup_message = I_(CST_STR_UNKNOWN_OPERATION);
		break;
	}

	return g_strdup(popup_message);
}

static void __cst_del_error_popup_with_ok_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstAppData_t *ad = (CstAppData_t *)data;

	evas_object_del(ad->popup);
	ad->popup = NULL;

	if (ad->dg_entry != NULL) {
		elm_object_focus_set(ad->dg_entry, EINA_TRUE);
		elm_entry_cursor_end_set(ad->dg_entry);
	}
}

void _cst_create_error_popup(int error)
{
	ENTER(_cst_create_error_popup);
	ret_if(error < 0);

	char *error_msg = _cst_get_error_string(error);
	DBG("Error message=%s", A_(error_msg));
	_cst_create_toast_popup(A_(error_msg));
	g_free(error_msg);

	return;
}

Evas_Object *_cst_create_error_popup_with_ok_btn(CstAppData_t *ad, int error)
{
	retv_if(error < 0, NULL);
	retv_if(ad == NULL, NULL);

	char *error_msg = _cst_get_error_string(error);
	DBG("Error message=%s", error_msg);

	Evas_Object *popup, *btn;
	ad->popup = popup = elm_popup_add(ad->nf);
	cst_util_domain_translatable_text_set(popup, error_msg);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	btn = elm_button_add(popup);
	cst_util_domain_translatable_text_set(btn, I_(CST_STR_SK_OK));
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", __cst_del_error_popup_with_ok_btn_cb, (void *)ad);

	evas_object_show(popup);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
								__cst_del_error_popup_with_ok_btn_cb, (void *)ad);

	g_free(error_msg);
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

void _cst_create_toast_popup(const char *text)
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

static void __cst_widget_entry_done_pressed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	DBG("__cst_widget_entry_done_pressed_cb");

	if (obj)
		elm_object_focus_set(obj, EINA_FALSE);
}

Evas_Object *_cst_create_ime_editfield(CstAppData_t *ad, Evas_Object *parent,
		CstImeType_t ime_type, char *input_string)
{
	retv_if((NULL == ad || NULL == parent), NULL);
	Elm_Input_Panel_Layout panel_layout;
	Elm_Entry_Filter_Limit_Size limit_filter_data;
	Elm_Entry_Filter_Accept_Set digits_filter_data;
	Ecore_IMF_Context *imf_context;
	void (*input_panel_cb)() = NULL;
	void (*entry_changed_cb)() = NULL;
	void (*entry_activated_cb)() = NULL;
	Evas_Object *layout = NULL;
	Evas_Object *entry_layout = NULL;
	Evas_Object *eo = NULL;

	if (CST_IME_REJECT_MSG == ime_type) {
		layout = elm_layout_add(parent);
		elm_layout_file_set(layout, THEME_NAME, "edit_reject_msg_layout");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);

		entry_layout = editfield_create(parent, ET_MULTILINE, NULL);
		ad->dg_entry = editfield_get_entry(entry_layout);
		elm_object_part_content_set(layout, "entry_part", entry_layout);

		ad->entry_count = layout;

		eo = layout;
	} else {
		/* Set the Entry part of the Edit-field */
		entry_layout = editfield_create(parent, ET_SINGLELINE, NULL);
		ad->dg_entry = editfield_get_entry(entry_layout);
		eo = entry_layout;
	}

	switch (ime_type) {
	case CST_IME_REJECT_MSG:
		cst_util_domain_translatable_part_text_set(ad->dg_entry, "elm.guide", I_(CST_STR_ENTER_MESSAGE));
		panel_layout = ELM_INPUT_PANEL_LAYOUT_NORMAL;
		elm_entry_input_panel_layout_set(ad->dg_entry, panel_layout);
		elm_entry_markup_filter_append(ad->dg_entry, _cst_reject_msg_entry_filter_check_boundary, ad);
		input_panel_cb = _cst_reject_msg_input_panel_event_callback;
		entry_changed_cb = _cst_reject_msg_changed_editfield_cb;
		break;
	case CST_IME_CALL_FORWARD:
		elm_entry_single_line_set(ad->dg_entry, EINA_TRUE);
		elm_entry_scrollable_set(ad->dg_entry, EINA_TRUE);
		if (strlen(ad->entry_string) > 0) {
			elm_entry_entry_set(ad->dg_entry, ad->entry_string);
		}
		if (NULL == input_string) {
			cst_util_domain_translatable_part_text_set(ad->dg_entry, "elm.guide", I_(CST_STR_NUMBER));
		}
		panel_layout = ELM_INPUT_PANEL_LAYOUT_PHONENUMBER;
		elm_entry_input_panel_layout_set(ad->dg_entry, panel_layout);
		limit_filter_data.max_char_count = 0;
		limit_filter_data.max_byte_count = CST_MAX_CF_NUMBER_LEN;
		digits_filter_data.accepted = "0123456789+*#";
		digits_filter_data.rejected = NULL;
		elm_entry_markup_filter_append(ad->dg_entry,
				elm_entry_filter_accept_set, &digits_filter_data);
		input_panel_cb = _cst_call_forwarding_input_panel_event_callback;
		entry_changed_cb = _cst_call_forward_entry_changed_cb;
		elm_entry_input_panel_return_key_type_set(ad->dg_entry,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		entry_activated_cb = __cst_widget_entry_done_pressed_cb;
		break;
	default:
		ERR("Invalid ime type.");
		return NULL;
	}

	if (CST_IME_REJECT_MSG != ime_type) {
		elm_entry_markup_filter_append(ad->dg_entry,
				elm_entry_filter_limit_size, &limit_filter_data);
	}
	elm_entry_cnp_mode_set(ad->dg_entry, ELM_CNP_MODE_PLAINTEXT);

	if (input_string != NULL) {
		elm_entry_entry_set(ad->dg_entry, input_string);
	}

	elm_entry_cursor_end_set(ad->dg_entry);

	imf_context = elm_entry_imf_context_get(ad->dg_entry);

	DBG("ad->entry = 0x%x, imf_context = 0x%x", ad->dg_entry, imf_context);

	if (imf_context && ime_type != CST_IME_REJECT_MSG) {
		ecore_imf_context_input_panel_event_callback_add(imf_context,
				ECORE_IMF_INPUT_PANEL_STATE_EVENT, input_panel_cb, ad);

			if (0 > ad->genlist_editfield_initialized) {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_FALSE);
				ad->genlist_editfield_initialized++;
			} else {
				ecore_imf_context_input_panel_enabled_set(imf_context,
						EINA_TRUE);
			}
	}

	if (ad->is_app_control_invoked) {
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	if (NULL != entry_changed_cb) {
		DBG("entry_changed_cb = 0x%x", entry_changed_cb);
		evas_object_smart_callback_add(ad->dg_entry, "changed",
				entry_changed_cb, ad);
		evas_object_smart_callback_add(ad->dg_entry, "preedit,changed",
				entry_changed_cb, ad);
	}

	if (NULL != entry_activated_cb) {
		evas_object_smart_callback_add(ad->dg_entry, "activated",
				entry_activated_cb, ad);
	}

	if (ad->b_expanded == EINA_FALSE) {
		evas_object_show(ad->dg_entry);
		g_entry = ad->dg_entry;
	}

	return eo;
}

Evas_Object *_cst_create_ime_contacts_btn_obj(Evas_Object *parent, void *data)
{
	retv_if(NULL == parent, NULL);
	retv_if(NULL == data, NULL);

	CstAppData_t *ad = (CstAppData_t *)data;
	Evas_Object *contact_icon = NULL;

	if (ad->contact_btn != NULL) {
		ad->contact_btn = NULL;
	}
	ad->contact_btn = elm_button_add(parent);
	elm_object_style_set(ad->contact_btn, "transparent");
	contact_icon = elm_image_add(ad->contact_btn);
	if (!contact_icon) {
		DBG("elm_icon_add() failed");
		return NULL;
	}
	elm_image_file_set(contact_icon, THEME_NAME, "CONTACT_ICON");
	elm_image_resizable_set(contact_icon, EINA_TRUE, EINA_TRUE);
	elm_object_part_content_set(ad->contact_btn, "icon", contact_icon);

	evas_object_smart_callback_add(ad->contact_btn, "pressed",
					_cst_on_press_ime_contact_btn, (void *)ad);
	evas_object_smart_callback_add(ad->contact_btn, "unpressed",
					_cst_on_unpress_ime_contact_btn, (void *)ad);
	evas_object_smart_callback_add(ad->contact_btn, "clicked",
					_cst_on_click_ime_contact_btn, (void *)ad);
	evas_object_show(ad->contact_btn);

	return ad->contact_btn;
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

void _cst_destroy_all_items(CstAppData_t *ad)
{
	ret_if(ad == NULL);

	_cst_destroy_answering_call();
	_cst_destroy_delete_list();
	_cst_destroy_call_forwarding();
	_cst_destroy_more_call_setting();
	_cst_destroy_reject_message(ad);
}

static void __cst_set_toolbar_item_icon(Elm_Object_Item *item, int simIndex)
{
	if (simIndex == 0) {
		elm_toolbar_item_icon_file_set(item, THEME_NAME, "SIM1_CARD");
	} else if (simIndex == 1) {
		elm_toolbar_item_icon_file_set(item, THEME_NAME, "SIM2_CARD");
	}
}
void _cst_create_dual_sim_tabbar(Elm_Object_Item *navi_it, Evas_Smart_Cb
		sim1_func, Evas_Smart_Cb sim2_func, void *data)
{
	ENTER(_cst_create_dual_sim_tabbar);
	ret_if(NULL == data);

	Evas_Object *tabbar = NULL;
	Elm_Object_Item *item[2] = {0, };
	CstAppData_t *ad = (CstAppData_t *)data;
	Eina_Bool is_sim1_present = EINA_FALSE;
	Eina_Bool is_sim2_present = EINA_FALSE;
	char *sim_name = NULL;
	/* create toolbar */
	tabbar = elm_toolbar_add(ad->nf);
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
	ad->sim1_btn = item[0];
	ad->sim2_btn = item[1];

	is_sim1_present = _cst_is_sim_present_by_simslot(data, CST_SELECTED_SIM1);
	is_sim2_present = _cst_is_sim_present_by_simslot(data, CST_SELECTED_SIM2);

	if (is_sim1_present && is_sim2_present) { /*Both SIM's inserted*/
		DBG("Both SIM's inserted");
		elm_toolbar_item_selected_set(item[0], EINA_TRUE);
		ad->sel_sim = CST_SELECTED_SIM1;
		_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM1);
	} else if (is_sim2_present && !is_sim1_present) { /*Only SIM2 inserted*/
		DBG("Only SIM2 inserted");
		elm_toolbar_item_selected_set(item[1], EINA_TRUE);
		ad->sel_sim = CST_SELECTED_SIM2;
		_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM2);
		elm_object_item_disabled_set(ad->sim1_btn, EINA_TRUE);
	} else { /*Only SIM1 inserted*/
		DBG("Only SIM1 inserted");
		elm_toolbar_item_selected_set(item[0], EINA_TRUE);
		ad->sel_sim = CST_SELECTED_SIM1;
		_cst_update_tapi_handle_by_simslot(data, CST_SELECTED_SIM1);
		elm_object_item_disabled_set(ad->sim2_btn, EINA_TRUE);
	}
}
