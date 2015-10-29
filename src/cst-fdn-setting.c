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

#ifdef _CALL_SET_FDN_SUPPORT

#include <vconf.h>
#include <ITapiSim.h>
#include "cst-fdn-setting.h"
#include "cst-fdn-list.h"
#include "cst-fdn-contact.h"
#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-widget.h"
#include "cst-tapi-request.h"
#include "cst-util.h"
#include "cst-more-call-setting.h"
#include "cst-editfield.h"
#include <efl_extension.h>

static Elm_Object_Item *fdn_main_screen_navi_it = NULL;
static Elm_Genlist_Item_Class *itc_1text = NULL;
static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Evas_Object *fdnsetting_gl = NULL;
static Evas_Object *ctxpopup = NULL;
#if 0	/* Do not delete this. Its for future use. */
static Evas_Object *passpopup = NULL;
static Evas_Object *passpopup_entry = NULL;
static CstPaswdPopupType_t paswd_popup_type;
static char *puk2 = NULL;
static char *pin2_old = NULL;
static char *pin2_new = NULL;
#endif

static CstGlItemData_t *g_item_data = NULL;

static gboolean keypad_enabled = EINA_FALSE;

static void __cst_set_genlist_item_styles_fdn(void);
static void __cst_destroy_genlist_item_styles(void);
static Eina_Bool __cst_on_click_fdn_setting_back_button(void *data, Elm_Object_Item *it);
static void __cst_on_fdn_list_click(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static char *__cst_get_fdn_label(void *data, Evas_Object *obj, const char *part);
static void __cst_fdn_del_phone(void *data, Evas_Object *obj);
#if 0	/* Do not delete this. Its for future use. */
static int __cst_create_password_popup(CstPaswdPopupType_t paswd_type, void *data);
#endif
static void __cst_fdn_tapi_sim_response_update_cb(CstUgData_t *ugd, void *req_data, void *resp_data);
static Evas_Object *__cst_create_genlist_fdn(void *data);
static void __cst_fdn_setting_popup_resize_more_ctxpopup_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info);

static Evas_Object *ec_item[3];

static void __cst_on_click_FDN_mode(void *data, Evas *evas, Evas_Object *obj, void *event_info);

static CstGlItemDisplayInfo_t list_fdn_setting[] = {
	{1, CST_STR_FIXED_DIALLING_MODE, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF_PROGRESS, __cst_on_click_FDN_mode},
	/*	{1, CST_STR_FDN_STATUS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_SUB_TEXT, __cst_on_enable_fdn_click},
		{1, CST_STR_FDN_CHANGE_PIN2, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, __cst_on_pin2_change_click},
		{1, CST_STR_FDN_PUK2_LOCKED, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, __cst_on_puk2_locked_click},
		{1, CST_STR_FDN_UNLOCK_PIN2, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, __cst_on_pin2_unblock_click},*/
	{1, CST_STR_FDN_CONTACTS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, __cst_on_fdn_list_click},

	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_1text) {
		_cst_destroy_genlist_item_class(itc_1text);
		itc_1text = NULL;
	}
	if (itc_1text_1icon) {
		_cst_destroy_genlist_item_class(itc_1text_1icon);
		itc_1text_1icon = NULL;
	}
}

static Eina_Bool __cst_on_click_fdn_setting_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_fdn_setting_back_button);
	CstUgData_t *ugd = (CstUgData_t *)data;

	_cst_destroy_fdn_setting();
	retv_if(NULL == ugd, EINA_TRUE);
	if (ugd->fdn_initial_state != ugd->fdn_status) {
		CstGlItemData_t *item_data = ugd->cw_gl_item_data;
		if (ugd->cw_gl_item_data) {
			if (ugd->fdn_status == CST_FDN_STATUS_ENABLED) {
				ugd->cw_state = CST_SS_STATE_OFF;
				elm_genlist_item_update(item_data->gl_item);
				elm_object_item_disabled_set(item_data->gl_item, EINA_TRUE);
			} else {
				ugd->cw_state = CST_SS_STATE_PROGRESS;
				elm_genlist_item_update(item_data->gl_item);
				elm_object_item_disabled_set(item_data->gl_item, EINA_FALSE);
				_cst_add_ss_request(&ugd->req_queue, CST_ACTION_QUERY,
									CST_CALLTYPE_VOICE, CST_SSTYPE_CW, EINA_FALSE, NULL,
									__cst_on_update_cw_state, ugd->cw_gl_item_data, -1, ugd);
			}
		}
	}

	return EINA_TRUE;
}

void _cst_FDN_setting_changed_editfield_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	const char *entry_str = elm_entry_entry_get(obj);

	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ugd->nf);
	ret_if(!navi_it);

	if (NULL == entry_str || '\0' == entry_str[0] || 4 > strlen(entry_str)) {
		if (ec_item[1] != NULL) {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	} else {
		if (ec_item[1] != NULL) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		}
	}
}

static Eina_Bool __cst_FDN_setting_naviframe_item_pop_cb(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;

	ugd->dg_entry = NULL;
	memset(ec_item, 0, sizeof(ec_item));

	return ECORE_CALLBACK_CANCEL;
}

static void __cst_FDN_setting_back_to_prev(CstUgData_t *ugd)
{
	ret_if(!ugd);
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);

	evas_object_smart_callback_del(ugd->dg_entry, "changed",
			_cst_FDN_setting_changed_editfield_cb);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context,
				ECORE_IMF_INPUT_PANEL_STATE_EVENT,
				_cst_FDN_setting_input_panel_event_cb);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	keypad_enabled = EINA_FALSE;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
/*	ecore_timer_add(CST_NAVIFRAME_ITEM_POP_TIMER, __cst_FDN_setting_naviframe_item_pop_cb, ugd);
*/
	__cst_FDN_setting_naviframe_item_pop_cb(ugd);
}

static void __cst_click_FDN_setting_done(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_click_FDN_setting_done);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	char *password = NULL;
	int len_password = 0;

	password = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry));
	ret_if(!password);

	if (ugd->dg_entry) {
		evas_object_del(ugd->dg_entry);
		ugd->dg_entry = NULL;
	}

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	len_password = strlen(password);

	if (ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) {
		_cst_fdn_disable_main_screen_show_unlock_pin2_popup((void *)ugd);
	} else if (4 > len_password) {
		SEC_ERR("Invalid PIN2 entered = %s", password);
		/* Show error popup */
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);
	} else {
		CallSettingSimNxtReqParams_t nxt_re_params = {0, };
		nxt_re_params.pin2 = (unsigned char *)password;
		nxt_re_params.pin2_len = len_password;
		nxt_re_params.req_id = TAPI_SIM_REQ_SET_FDN;
		_cst_verify_pin2(ugd, &nxt_re_params, __cst_fdn_tapi_sim_response_update_cb, g_item_data);
	}

	if (password) {
		g_free(password);
		password = NULL;
	}

/*	__cst_FDN_setting_back_to_prev(ugd);*/
}

static void __cst_click_FDN_setting_cancel(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_click_FDN_setting_cancel);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

/*
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
*/
	__cst_FDN_setting_back_to_prev(ugd);
}

void _cst_FDN_setting_input_panel_event_cb(void *data, Ecore_IMF_Context *imf_context, int value)
{
	ENTER(_cst_FDN_setting_input_panel_event_cb);
	ret_if(NULL == data);

	if (EINA_FALSE == keypad_enabled) {
		keypad_enabled = EINA_TRUE;
		return;
	}

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_SHOW");
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_HIDE");
	}
}

static void __cst_on_click_FDN_mode(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_FDN_mode);
	ret_if(!data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = item_data->ugd;
	const char *entry_input = NULL;
	const char *done_btn_text = NULL;

	if (((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account == 0))
			|| (ugd->sim_lock_status == TAPI_SIM_LOCK_PERM_BLOCKED)) {
		/* PUK2 blocked (PIN2 permanently blocked) */
		ugd->popup = _cst_create_1btn_popup(ugd->nf, ugd,
				T_(CST_STR_PUK2_BLOCKED), NULL,
				NULL, ugd);
		return;
	} else if (((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account > 0))
		|| ((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PIN2) && (ugd->sim_lock_retry_account == 0))) {
		_cst_fdn_disable_main_screen_show_unlock_pin2_popup((void *)ugd);
		return;
	}

	memset(ec_item, 0, sizeof(ec_item));
	g_item_data = item_data;

	if (CST_FDN_STATUS_ENABLED == ugd->fdn_status) {
		done_btn_text = I_(CST_STR_POP_DISABLE);
	} else {
		done_btn_text = I_(CST_STR_ENABLE);
	}
	_cst_create_pin2_confirm_popup(ugd, CST_IME_FDN_SETTING,
			done_btn_text, __cst_click_FDN_setting_done,
			__cst_click_FDN_setting_cancel, (void *)ugd, ec_item);

	entry_input = elm_entry_entry_get(ugd->dg_entry);
	if (ec_item[1] != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	}
}

#if 0	/* Do not delete this. Its for future use. */
static void _popup_ok_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(_popup_ok_cb);
	ret_if(!data);

	CstUgData_t *ugd = (CstUgData_t *)data;
	char *password = NULL;
	int len_password = 0;

	password = elm_entry_markup_to_utf8(elm_entry_entry_get(editfield_get_entry(passpopup_entry)));
	ret_if(!password);

	len_password = strlen(password);
	if (passpopup) {
		evas_object_del(passpopup);
		passpopup = NULL;
	}

	switch (paswd_popup_type) {
	case CST_PWD_POPUP_TYPE_ADD_FDN_CONTACT:
		break;

	case CST_PWD_POPUP_TYPE_CHANGE_PIN2:
		if (4 > len_password) {
			SEC_ERR("Invalid PIN2 entered = %s", password);
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_CHANGE_PIN2, ugd);
		} else {
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW, ugd);
			pin2_old = g_strdup(password);
			SEC_DBG("Old pin entered = %s", pin2_old);
		}
		break;

	case CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW:
		if (4 > len_password) {
			SEC_ERR("Invalid PIN2 entered = %s", password);
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW, ugd);
		} else {
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW_CONFIRM, ugd);
			pin2_new = g_strdup(password);
			SEC_DBG("New pin entered = %s", pin2_new);
		}
		break;

	case CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW_CONFIRM:
		if (!strcmp(password, pin2_new)) {
			SEC_DBG("New pin confirmed = %s", pin2_new);
			/* Send change pin request to tapi */
			_cst_change_pins(ugd, pin2_old, pin2_new, __cst_fdn_tapi_sim_response_update_cb, ugd);
			if (pin2_old) {
				g_free(pin2_old);
				pin2_old = NULL;
			}
		} else {
			/* Password confirm failed. Start again */
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW, ugd);
		}

		if (pin2_new) {
			g_free(pin2_new);
			pin2_new = NULL;
		}
		break;

	case CST_PWD_POPUP_TYPE_FDN_STATUS:
		if (4 > len_password) {
			SEC_ERR("Invalid PIN2 entered = %s", password);
			/* Show error popup */
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);
		} else {
			CallSettingSimNxtReqParams_t nxt_re_params = {0, };
			nxt_re_params.pin2 = (unsigned char *)password;
			nxt_re_params.pin2_len = len_password;
			nxt_re_params.req_id = TAPI_SIM_REQ_SET_FDN;
			_cst_verify_pin2(item_data->ugd, &nxt_re_params, __cst_fdn_tapi_sim_response_update_cb, item_data);
		}
		break;

	case CST_PWD_POPUP_TYPE_UNBLOCK_PIN2:
		if (8 > len_password) {
			SEC_ERR("Invalid PIN2 entered = %s", password);
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_UNBLOCK_PIN2, ugd);
		} else {
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW, ugd);
			puk2 = g_strdup(password);
			SEC_DBG("puk2 entered = %s", puk2);
		}
		break;

	case CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW:
		if (4 > len_password) {
			SEC_ERR("Invalid PIN2 entered = %s", password);
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW, ugd);
		} else {
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW_CONFIRM, ugd);
			pin2_new = g_strdup(password);
			SEC_DBG("New pin2 entered = %s", pin2_new);
		}
		break;

	case CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW_CONFIRM:
		if (!strcmp(password, pin2_new)) {
			/* Send unblock sim request to tapi */
			_cst_unblock_pin2(ugd, puk2, pin2_new, __cst_fdn_tapi_sim_response_update_cb, ugd);
			if (puk2) {
				g_free(puk2);
				puk2 = NULL;
			}
			if (pin2_old) {
				g_free(pin2_old);
				pin2_old = NULL;
			}
		} else {
			DBG("PIN2 confirm failed");
			/* Password confirm failed. Start again */
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW, ugd);
		}

		if (pin2_new) {
			g_free(pin2_new);
			pin2_new = NULL;
		}
		break;

	default:
		ERR("Unknown password popup type: %d", paswd_popup_type);
		break;
	}
	if (password) {
		g_free(password);
	}
	LEAVE();
}

static void _popup_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(_popup_cancel_cb);

	CstUgData_t *ugd = (CstUgData_t *)data;
	if (ugd->dg_entry_pin2) {
		elm_entry_entry_set(ugd->dg_entry_pin2, "");
		_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
	}

	if (passpopup) {
		evas_object_del(passpopup);
		passpopup = NULL;
	}

	if (pin2_old) {
		g_free(pin2_old);
		pin2_old = NULL;
	}

	if (pin2_new) {
		g_free(pin2_new);
		pin2_new = NULL;
	}

	if (puk2) {
		g_free(puk2);
		puk2 = NULL;
	}

	LEAVE();
}

static int __cst_create_password_popup(CstPaswdPopupType_t paswd_type, void *data)
{
	ENTER(__cst_create_password_popup);

	char *txt = NULL;
	char *guide_txt = NULL;
	Evas_Object *layout;
	Evas_Object *btn_ok;
	Evas_Object *btn_cancel;
	static Elm_Entry_Filter_Limit_Size limit_size = {
		.max_char_count = 8,
		.max_byte_count = 0
	};
	CstUgData_t *ugd = (CstUgData_t *)data;
	retv_if(NULL == ugd, FALSE);

	if (passpopup) {
		evas_object_del(passpopup);
	}

	passpopup = elm_popup_add(ugd->nf);
	evas_object_size_hint_weight_set(passpopup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	layout = elm_layout_add(passpopup);
	elm_layout_theme_set(layout, "layout", "popup", "entrypasswordview");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	paswd_popup_type = paswd_type;

	switch (paswd_type) {
	case CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW:
#if 0	/* Donot delete this. Its for future use. */
	case CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW:
#endif
		txt = g_strdup_printf("%s %s", T_(CST_STR_CHANGE), T_(CST_STR_PIN2));
		cst_util_domain_translatable_part_text_set(passpopup, "title,text", txt);
		guide_txt = g_strdup(I_(CST_STR_FDN_NEW_PIN2));
		g_free(txt);
		break;
	case CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW_CONFIRM:
#if 0	/* Donot delete this. Its for future use. */
	case CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW_CONFIRM:
#endif
		txt = g_strdup_printf("%s %s", T_(CST_STR_CHANGE), T_(CST_STR_PIN2));
		cst_util_domain_translatable_part_text_set(passpopup, "title,text", txt);
		g_free(txt);
		guide_txt = g_strdup(I_(CST_STR_FDN_CONFIRM_NEW_PIN2));
		break;
	case CST_PWD_POPUP_TYPE_UNBLOCK_PIN2:
		txt = g_strdup_printf("%s %s", T_(CST_STR_CHANGE), T_(CST_STR_PIN2));
		cst_util_domain_translatable_part_text_set(passpopup, "title,text", txt);
		g_free(txt);
		txt = (char *)T_(CST_STR_PUK2);
		if (ugd->sim_lock_retry_account > 0) {
			guide_txt = g_strdup_printf("%s (%d %s)", txt,
					ugd->sim_lock_retry_account,
					T_(CST_STR_INCORRECT_PIN2_ATTEMPTS_REMAINING));
			g_free(txt);
		} else {
			guide_txt = g_strdup(txt);
		}
		break;
#if 0	/* Do not delete this. Its for future use. */
	case CST_PWD_POPUP_TYPE_FDN_STATUS:
		if (CST_FDN_STATUS_ENABLED == ugd->fdn_status) {
			elm_object_part_text_set(passpopup, "title,text", POPUP_TITLE_DISABLE_FDN_STR);
		} else {
			elm_object_part_text_set(passpopup, "title,text", POPUP_TITLE_ENABLE_FDN_STR);
		}

		txt = ENTER_PIN2_STR;
		if (ugd->sim_lock_retry_account > 0) {
			guide_txt = g_strdup_printf("%s (%d %s)", txt,
					ugd->sim_lock_retry_account,
					T_(CST_STR_INCORRECT_PIN2_ATTEMPTS_REMAINING));
			g_free(txt);
		} else {
			guide_txt = g_strdup(txt);
		}
		break;
	case CST_PWD_POPUP_TYPE_CHANGE_PIN2:
		txt = OLD_PIN2_STR;
		elm_object_part_text_set(passpopup, "title,text", POPUP_TITLE_CHANGE_PIN2_STR);
		if (ugd->sim_lock_retry_account > 0) {
			guide_txt = g_strdup_printf("%s (%d %s)", txt,
					ugd->sim_lock_retry_account,
					T_(CST_STR_INCORRECT_PIN2_ATTEMPTS_REMAINING));
			g_free(txt);
		} else {
			guide_txt = g_strdup(txt);
		}
		break;
#endif
	default:
		ERR("Invalid password type received: %d", paswd_type);
		break;
	}

	passpopup_entry = editfield_create(layout, ET_SINGLELINE, NULL);
	Evas_Object *entry = editfield_get_entry(passpopup_entry);
	if (guide_txt) {
		cst_util_domain_translatable_part_text_set(entry, "elm.guide", guide_txt);
		g_free(guide_txt);
	} else {
		cst_util_domain_translatable_part_text_set(entry, "elm.guide", I_(CST_STR_ENTER_PASSWORD));
	}
	elm_object_part_content_set(layout, "elm.swallow.content", passpopup_entry);
	elm_entry_password_set(entry, EINA_TRUE);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size,
								   &limit_size);

	elm_object_content_set(passpopup, layout);

	btn_ok = elm_button_add(passpopup);
	elm_object_style_set(btn_ok, "popup_button/default");
	elm_object_text_set(btn_ok, OK_STR);
	elm_object_part_content_set(passpopup, "button1", btn_ok);
	evas_object_smart_callback_add(btn_ok, "clicked", _popup_ok_cb, ugd);
	btn_cancel = elm_button_add(passpopup);
	elm_object_style_set(btn_cancel, "popup_button/default");
	elm_object_text_set(btn_cancel, CANCEL_STR);
	elm_object_part_content_set(passpopup, "button2", btn_cancel);
	evas_object_smart_callback_add(btn_cancel, "clicked", _popup_cancel_cb, ugd);
	evas_object_show(passpopup);

	LEAVE();

	return TRUE;
}
#endif

static void __cst_on_fdn_list_click(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	_cst_on_click_fdn_list(item_data->ugd);
}

static void __cst_on_changed_FDN_mode(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_changed_FDN_mode);
	ret_if(!data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = item_data->ugd;
	Eina_Bool check_state = elm_check_state_get(obj);

	DBG("ugd->fdn_status = %d, check_state = %d", ugd->fdn_status, check_state);
	if (ugd->fdn_status != check_state) {
		elm_check_state_set(obj, !check_state);
		__cst_on_click_FDN_mode(data, NULL, obj, event_info);
	}
}

static void __cst_FDN_status_update(void *data)
{
	ENTER(__cst_FDN_status_update);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = item_data->ugd;

	/* Disable the FDN menu if fetching the "FDN_Status" fails */
	if ((fdnsetting_gl) && ((ugd->fdn_status == CST_FDN_STATUS_FAIL) ||
			(ugd->sim_lock_status == TAPI_SIM_LOCK_PERM_BLOCKED) ||
			(ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2))) {
		ERR("Fatal: SIM Get FDN Status Failed");
		Elm_Object_Item *item = elm_genlist_first_item_get(fdnsetting_gl);
		while (item) {
			elm_object_item_disabled_set(item, EINA_TRUE);
			item = elm_genlist_item_next_get(item);
		}
	}

	elm_genlist_item_update(item_data->gl_item);
}

static Evas_Object *__cst_gl_FDN_mode_icon_get(void *data, Evas_Object *obj, const char *part)
{
	ENTER(__cst_gl_FDN_mode_icon_get);
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	Evas_Object *ly = NULL;

	if (!strcmp(part, "elm.icon.2")) {
		int value = 0;
		if (CST_STR_FIXED_DIALLING_MODE == list_fdn_setting[item_data->index].str_id) {
			CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
			value = ugd->fdn_status;
			ly = elm_layout_add(obj);
			elm_layout_theme_set(ly, "layout", "list/C/type.3", "default");
			if (value >= CST_FDN_STATUS_DISABLED) {
				item_data->eo_check = _cst_create_onoff_button(ly, value,
								  __cst_on_changed_FDN_mode, item_data);
			} else if (value == CST_FDN_STATUS_FAIL) {
				item_data->eo_check = _cst_create_onoff_button(ly, 0,
								  __cst_on_changed_FDN_mode, item_data);
			} else {
				item_data->eo_check = _cst_create_progressbar(ly);
				ugd->fdn_gl_item_data = item_data;
			}
			elm_layout_content_set(ly, "elm.swallow.content", item_data->eo_check);
		}
#ifdef _CALL_SET_TTS_SUPPORT
		/* Unregister the access object */
		if (elm_config_access_get() && item_data->eo_check) {
			elm_access_object_unregister(item_data->eo_check);
		}
#endif
		return ly;
	}
	return NULL;
}

static void __cst_fdn_del_phone(void *data, Evas_Object *obj)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

static void __cst_set_genlist_item_styles_fdn(void)
{
	ENTER(__cst_set_genlist_item_styles_fdn);

	if (!itc_1text_1icon) {
		itc_1text_1icon = _cst_create_genlist_item_class("1line",
				__cst_get_fdn_label,
				__cst_gl_FDN_mode_icon_get, NULL,
				__cst_fdn_del_phone);
	}

	if (!itc_1text) {
		itc_1text = _cst_create_genlist_item_class("1line",
				__cst_get_fdn_label, NULL, NULL,
				__cst_fdn_del_phone);
	}
}

static char *__cst_get_fdn_label(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	ENTER(__cst_get_fdn_label);
	if ((strcmp(part, "elm.text.main.left") == 0) ||
			(strcmp(part, "elm.text.multiline") == 0)) {
		if (list_fdn_setting[item_data->index].str_id == CST_STR_FIXED_DIALLING_MODE) {
			return strdup(T_(CST_STR_FIXED_DIALLING_MODE));
		} else {
			return strdup(T_(CST_STR_FDN_CONTACTS));
		}
	}
	return NULL;
}

static void __cst_pin2_unblock_invoke_security_ug_layout_cb(ui_gadget_h ug,
		enum ug_mode mode, void *priv)
{
	ENTER(__cst_pin2_unblock_invoke_security_ug_layout_cb);
	Evas_Object *base = ug_get_layout(ug);
	ret_if(base == NULL);

	evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(base);
}

static void __cst_pin2_unblock_invoke_security_ug_result_cb(ui_gadget_h ug,
		service_h service, void *priv)
{
	ENTER(__cst_pin2_unblock_invoke_security_ug_result_cb);
	ret_if(priv == NULL);
	CstUgData_t *ugd = (CstUgData_t *)priv;
	char *result = NULL;

	service_get_extra_data(service, "result", &result);
	if (!result)
		return;

	DBG("**** Result = : %s ****", result);
	if (!g_strcmp0(result, "SETTING_PW_TYPE_PIN2_UNBLOCKED")) {
		ugd->sim_lock_retry_account = 3;
		ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PIN2;

		/* Enable the FDN menu */
		if (fdnsetting_gl) {
			Elm_Object_Item *item = elm_genlist_first_item_get(fdnsetting_gl);
			while (item) {
				elm_object_item_disabled_set(item, EINA_FALSE);
				item = elm_genlist_item_next_get(item);
			}
		}
	} else if (!g_strcmp0(result, "SETTING_PW_TYPE_PUK2_BLOCKED")) {
		ugd->sim_lock_status = TAPI_SIM_LOCK_PERM_BLOCKED;
		ugd->sim_lock_retry_account = 0;
	}

	_cst_get_sim_lock_info(ugd);

	free(result);
}

static void __cst_pin2_unblock_invoke_security_ug_destroy_cb(ui_gadget_h ug,
		void *priv)
{
	ENTER(__cst_pin2_unblock_invoke_security_ug_destroy_cb);
	if (ug) {
		ug_destroy(ug);
	}
}

static void __cst_pin2_unblock_invoke_security_ug(void *data)
{
	ENTER(__cst_pin2_unblock_invoke_security_ug);
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	struct ug_cbs cbs = {0, };
	ui_gadget_h ug = NULL;
	service_h svc;

	if (service_create(&svc)) {
		return;
	}

	cbs.layout_cb = __cst_pin2_unblock_invoke_security_ug_layout_cb;
	cbs.result_cb = __cst_pin2_unblock_invoke_security_ug_result_cb;
	cbs.destroy_cb = __cst_pin2_unblock_invoke_security_ug_destroy_cb;
	cbs.priv = (void *)data;

	service_add_extra_data(svc, "viewtype", "SETTING_PW_TYPE_PIN2_BLOCKED");
	DBG("sel_sim: %d", ugd->sel_sim);
	if (ugd->sel_sim == 0) {
		service_add_extra_data(svc, "sim_type", "SIM1");
	} else {
		service_add_extra_data(svc, "sim_type", "SIM2");
	}

	ug = ug_create((ui_gadget_h)ugd->ug, "setting-password-efl",
			UG_MODE_FULLVIEW, svc, &cbs);
	DBG("ug = 0x%x", ug);
	if (ug == NULL) {
		DBG("ug create fail...");
	}

	service_destroy(svc);
}

void _cst_pin2_blocked_popup_confirm_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (!data) {
		ERR("Invalid params!");
		return;
	}
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd && ugd->popup) {
		__cst_pin2_unblock_invoke_security_ug(data);
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

static void __cst_fdn_tapi_sim_response_update_cb(CstUgData_t *ugd, void *req_data, void *resp_data)
{
	ENTER(__cst_fdn_tapi_sim_response_update_cb);
	CallSettingSimReq_t *req = (CallSettingSimReq_t *)req_data;
	CallSettingSimResp_t *resp = (CallSettingSimResp_t *)resp_data;
	switch (req->req_id) {
	case TAPI_SIM_REQ_VERIFY_PIN2:
	{
		DBG("TAPI_SIM_REQ_VERIFY_PIN2 received");
		if (req->next_req_params) {
			if (TAPI_SIM_REQ_SET_FDN == req->next_req_params->req_id) {
				CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
				__cst_FDN_setting_back_to_prev(item_data->ugd);
				if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
					ugd->sim_lock_retry_account = 3;
					_cst_set_sim_fdn_status(ugd, req->next_req_params->pin2, &(req->next_req_params->pin2_len), req->func, req->data);
				} else if (resp->retry_count) {
					DBG("PIN2 verify failed for FDN set: retry_count(%d)", resp->retry_count);
					ugd->sim_lock_retry_account = resp->retry_count;
					/* Show PIN2 change error */

					const char *popupmsg = NULL;

					switch (resp->retry_count) {
					case 1:
						popupmsg = I_(CST_STR_INCORRECT_PIN2_1_ATTEMPT_LEFT);
						break;
					case 2:
						popupmsg = I_(CST_STR_INCORRECT_PIN2_2_ATTEMPTS_LEFT);
						break;
					default:
						popupmsg = I_(CST_STR_INCORRECT_PIN2);
						break;
					}

					cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, item_data->ugd, NULL, popupmsg);
					if (ugd->dg_entry) {
						elm_entry_entry_set(ugd->dg_entry, "");
					}
				} else {
					DBG("Max retries over !!");
					CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
					ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PUK2;
					ugd->sim_lock_retry_account = 10;

					_cst_fdn_disable_main_screen_show_unlock_pin2_popup(
							(void *)ugd);

					if (ugd->dg_entry) {
						elm_entry_entry_set(ugd->dg_entry, "");
					}
				}
			} else {
				ERR("Unknown request id: %d", req->next_req_params->req_id);
			}
		}
	}
	break;

	case TAPI_SIM_REQ_SET_FDN:
	{
		DBG("TAPI_SIM_REQ_SET_FDN received");
		CstGlItemData_t *item_data = (CstGlItemData_t *)req->data;
		elm_genlist_item_update(item_data->gl_item);
	}
	break;

#if 0	/* Do not delete this. Its for future use. */
	case TAPI_SIM_REQ_UNBLOCK_PIN2:
	{
		DBG("TAPI_SIM_REQ_UNBLOCK_PIN2 received");
		CstUgData_t *ugd = (CstUgData_t *)req->data;
		if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
			ugd->sim_lock_retry_account = 3;
			ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PIN2;
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, I_(CST_STR_PIN2_UNBLOCKED));
		} else if (resp->retry_count) { /* Show retry password popup */
			DBG("Pin2 change failed! Remaining Retry = %d", resp->retry_count);
			ugd->sim_lock_retry_account = resp->retry_count;
			/* Wrong attempt. Show PIN2 change popup to user again */
			__cst_create_password_popup(CST_PWD_POPUP_TYPE_UNBLOCK_PIN2, ugd);
		} else {
			DBG("Max retries over !! PUK2 blocked !!!");
			ugd->sim_lock_status = TAPI_SIM_LOCK_PERM_BLOCKED;
			ugd->sim_lock_retry_account = 0;
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, I_(CST_STR_PUK2_BLOCKED));
		}

		if (ugd->dg_entry_pin2) {
			elm_entry_entry_set(ugd->dg_entry_pin2, "");
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
		}

		if (ugd->dg_entry) {
			elm_entry_entry_set(ugd->dg_entry, "");
			elm_object_focus_set(ugd->dg_entry, EINA_TRUE);
		}
	}
	break;

	case TAPI_SIM_REQ_CHANGE_PIN2:
	{
		DBG("TAPI_SIM_REQ_CHANGE_PIN2 received");
		CstUgData_t *ugd = (CstUgData_t *)req->data;
		if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, PIN2_CHANGE_SUCCESS_STR);
			ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PIN2;
			ugd->sim_lock_retry_account = 3;
		} else if (resp->retry_count) { /* Show retry password popup */
			DBG("Pin2 change failed!");
			ugd->sim_lock_retry_account = resp->retry_count;
			/* Show PIN2 change error */
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, PIN2_CHANGE_FAIL_STR);
		} else {
			DBG("Max retries over !! PIN2 blocked !!!");
			ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PUK2;
			ugd->sim_lock_retry_account = 10;
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, T_(CST_STR_PIN2_BLOCKED));
		}
	}
	break;
#endif

	default:
		ERR("Response received for an Invalid Request id[%d]", req->req_id);
		break;
	}

	LEAVE();
	return;
}

static void __cst_gl_sel_expand_menu(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_sel_expand_menu);
	ret_if(NULL == data);

	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (list_fdn_setting[item_data->index].func) {
		list_fdn_setting[item_data->index].func((void *)item_data, NULL, obj, event_info);
	}
	LEAVE();
	return;
}
#ifdef _CALL_SET_TTS_SUPPORT
static void _cst_FDN_settings_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	CstGlItemData_t *item_data = elm_object_item_data_get(it);
	if (NULL != item_data && list_fdn_setting[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF_PROGRESS) {
		elm_access_info_cb_set(elm_object_item_access_object_get(it),
				ELM_ACCESS_INFO, _cst_util_on_off_btn_access_info_cb, it);
	}
}
#endif
static Evas_Object *__cst_create_genlist_fdn(void *data)
{
	ENTER(__cst_create_genlist_fdn);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	CstGlItemData_t *item_data;
	Evas_Object *genlist;
	int i;

	genlist = elm_genlist_add(ugd->nf);
	retv_if(NULL == genlist, NULL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	_cst_get_sim_lock_info(data);
	_cst_create_genlist_separator(genlist, EINA_FALSE);
	for (i = 0; list_fdn_setting[i].style != CST_GL_ITEM_NONE; ++i) {
		item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(NULL == item_data, NULL);

		item_data->index = i;
		item_data->ugd = ugd;
		if (list_fdn_setting[i].style == CST_GL_ITEM_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon,
								 (void *)item_data, NULL, list_fdn_setting[i].flags,
								 __cst_gl_sel_expand_menu, item_data);
		} else if (list_fdn_setting[i].style == CST_GL_ITEM_1TEXT_ONOFF_PROGRESS) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon,
								 (void *)item_data, NULL, list_fdn_setting[i].flags,
								 __cst_gl_sel_expand_menu, item_data);
		} else {
			DBG("No style");
			free(item_data);
			item_data = NULL;
			return NULL;
		}
	}
	_cst_create_genlist_separator(genlist, EINA_FALSE);

	evas_object_smart_callback_add(genlist, "realized",
						_cst_gl_realized_cb, NULL);
#ifdef _CALL_SET_TTS_SUPPORT
	evas_object_smart_callback_add(genlist, "realized",
						_cst_FDN_settings_realized_cb, NULL);
#endif
	return genlist;
}

void cst_FDN_setting_show_popup(CstErrPopupType_t err_popup_type, void *data, char *title_txt, const char *info_txt)
{
	Evas_Object *popup = NULL;
	CstUgData_t *ugd = (CstUgData_t *)data;
	if (!data || !info_txt) {
		ERR("Invalid params!");
		return;
	}

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
	switch (err_popup_type) {
	case CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY:
		popup = elm_popup_add(ugd->win_main);
		evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		cst_util_domain_translatable_text_set(popup, info_txt);

		elm_popup_timeout_set(popup, CST_POPUP_TIMER);
		evas_object_show(popup);
		break;
	case CST_ERR_POPUP_TYPE_TITLE_INFO_TEXT:
		popup = elm_popup_add(ugd->win_main);
		evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		cst_util_domain_translatable_text_set(popup, info_txt);
		cst_util_domain_translatable_part_text_set(popup, "title,text", title_txt);
		elm_popup_timeout_set(popup, CST_POPUP_TIMER);
		evas_object_show(popup);
		break;
	}
	ugd->popup = popup;
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
									eext_popup_back_cb, NULL);
}

static void __cst_fdn_setting_ctxpopup_rotation_changed(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_fdn_setting_ctxpopup_rotation_changed);
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
	}
}

static void __cst_fdn_setting_dismiss_ctxpopup_more_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_fdn_setting_dismiss_ctxpopup_more_cb);
	ret_if(NULL == data);

	if (ctxpopup) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static void __cst_on_click_change_pin2_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_change_pin2_cb);
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	struct ug_cbs cbs = {0, };
	ui_gadget_h ug = NULL;
	service_h svc;

	if (ctxpopup) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}

	if (((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account == 0))
			|| (ugd->sim_lock_status == TAPI_SIM_LOCK_PERM_BLOCKED)) {
		/* PUK2 blocked (PIN2 permanently blocked) */
		ugd->popup = _cst_create_1btn_popup(ugd->nf, ugd,
				T_(CST_STR_PUK2_BLOCKED), NULL,
				NULL, ugd);
		return;
	} else if (((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account > 0))
		|| ((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PIN2) && (ugd->sim_lock_retry_account == 0))) {
		__cst_pin2_unblock_invoke_security_ug(data);
		return;
	}

	if (service_create(&svc)) {
		return;
	}

	cbs.layout_cb = __cst_pin2_unblock_invoke_security_ug_layout_cb;
	cbs.result_cb = __cst_pin2_unblock_invoke_security_ug_result_cb;
	cbs.destroy_cb = __cst_pin2_unblock_invoke_security_ug_destroy_cb;
	cbs.priv = (void *)data;

	service_add_extra_data(svc, "viewtype", "SETTING_PW_TYPE_CHANGE_PIN2");
	DBG("sel_sim: %d", ugd->sel_sim);
	if (ugd->sel_sim == 0) {
		service_add_extra_data(svc, "sim_type", "SIM1");
	} else {
		service_add_extra_data(svc, "sim_type", "SIM2");
	}

	ug = ug_create((ui_gadget_h)ugd->ug, "setting-password-efl",
			UG_MODE_FULLVIEW, svc, &cbs);
	DBG("ug = 0x%x", ug);
	if (ug == NULL) {
		DBG("ug create fail...");
	}

	service_destroy(svc);
}

static void __cst_fdn_setting_popup_resize_more_ctxpopup_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_fdn_setting_popup_resize_more_ctxpopup_cb);
	ret_if(NULL == data);
	__cst_fdn_setting_ctxpopup_rotation_changed(data, NULL, NULL);
}

static void __cst_fdn_setting_popup_hide_more_option(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_fdn_setting_popup_resize_more_ctxpopup_cb);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ctxpopup) {
		if (ugd->nf) {
			evas_object_event_callback_del_full(ugd->nf, EVAS_CALLBACK_RESIZE,
					__cst_fdn_setting_popup_resize_more_ctxpopup_cb, data);
		}

		evas_object_smart_callback_del(ugd->win_main, "rotation,changed",
				__cst_fdn_setting_ctxpopup_rotation_changed);

		evas_object_smart_callback_del(ctxpopup, "dismissed",
				__cst_fdn_setting_dismiss_ctxpopup_more_cb);

		evas_object_event_callback_del_full(ctxpopup, EVAS_CALLBACK_DEL,
				__cst_fdn_setting_popup_hide_more_option, data);

		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static void __cst_fdn_setting_create_more_ctxpopup_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	DBG("__cst_fdn_setting_create_more_ctxpopup_cb");
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;

	if ((ugd->fdn_status == CST_FDN_STATUS_FAIL) ||
			(ugd->sim_lock_status == TAPI_SIM_LOCK_PERM_BLOCKED)) {
		ERR("Fatal: FDN Status Fetch is Failed, so return");
		return;
	}

	if (ctxpopup == NULL) {
		ctxpopup = elm_ctxpopup_add(ugd->win_main);
		elm_object_style_set(ctxpopup, "more/default");
		elm_ctxpopup_auto_hide_disabled_set(ctxpopup, EINA_TRUE);
		evas_object_smart_callback_add(ctxpopup, "dismissed",
				__cst_fdn_setting_dismiss_ctxpopup_more_cb, data);
		eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, NULL);
		eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_MORE, eext_ctxpopup_back_cb, NULL);
		evas_object_event_callback_add(ctxpopup, EVAS_CALLBACK_DEL,
				__cst_fdn_setting_popup_hide_more_option, data);
		evas_object_event_callback_add(ugd->nf, EVAS_CALLBACK_RESIZE,
				__cst_fdn_setting_popup_resize_more_ctxpopup_cb, data);

		if (((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account > 0))
				|| ((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PIN2) && (ugd->sim_lock_retry_account == 0))) {
			elm_ctxpopup_item_append(ctxpopup, T_(CST_STR_UNBLOCK_PIN2), NULL,
								__cst_on_click_change_pin2_cb, (void *)ugd);
		} else {
			elm_ctxpopup_item_append(ctxpopup, T_(CST_STR_CHANGE_PIN2), NULL,
					__cst_on_click_change_pin2_cb, (void *)ugd);
		}

		elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_UP,
							ELM_CTXPOPUP_DIRECTION_UNKNOWN,
							ELM_CTXPOPUP_DIRECTION_UNKNOWN,
							ELM_CTXPOPUP_DIRECTION_UNKNOWN);
		elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
		evas_object_smart_callback_add(ugd->win_main, "rotation,changed",
				__cst_fdn_setting_ctxpopup_rotation_changed, data);
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

void _cst_on_click_fdn_setting(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_fdn_setting);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(ugd == NULL);
	Evas_Object *more_btn = NULL;
	ugd->fdn_status = CST_FDN_STATUS_UNKNOWN;
	ugd->fdn_status_update_func = __cst_FDN_status_update;
	ugd->fdn_gl_item_data = NULL;

	/* Request TAPI to update the FDN status */
	_cst_update_sim_fdn_status(ugd);
	ugd->fdn_initial_state = ugd->fdn_status;
	__cst_set_genlist_item_styles_fdn();
	fdnsetting_gl = __cst_create_genlist_fdn(ugd);

	fdn_main_screen_navi_it = elm_naviframe_item_push(ugd->nf,
			I_(CST_STR_FIXED_DIALLING_NUMBERS), NULL, NULL, fdnsetting_gl,
			NULL);
	cst_util_item_domain_text_translatable_set(fdn_main_screen_navi_it,
			I_(CST_STR_FIXED_DIALLING_NUMBERS));

	elm_naviframe_item_pop_cb_set(fdn_main_screen_navi_it,
			__cst_on_click_fdn_setting_back_button, (void *)ugd);

	/*create more button*/
	more_btn = elm_button_add(ugd->nf);
	elm_object_style_set(more_btn, "naviframe/more/default");
	evas_object_smart_callback_add(more_btn, "clicked",
			__cst_fdn_setting_create_more_ctxpopup_cb, ugd);
	elm_object_item_part_content_set(fdn_main_screen_navi_it,
			"toolbar_more_btn", more_btn);

	LEAVE();
}

void _cst_destroy_fdn_setting(void)
{
	ENTER(_cst_destroy_fdn_setting);

	__cst_destroy_genlist_item_styles();

	if (fdnsetting_gl != NULL) {
		fdnsetting_gl = NULL;
	}
	fdn_main_screen_navi_it = NULL;
}

static void __cst_create_pin2_confirm_popup_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_create_pin2_confirm_popup_cancel_cb);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

void _cst_create_pin2_confirm_popup(CstUgData_t *ugd, CstImeType_t ime_type,
		const char *done_btn_text, Evas_Smart_Cb done_cb, Evas_Smart_Cb cancel_cb,
		void *cb_data, Evas_Object **c_item)
{
	ENTER(_cst_create_pin2_confirm_popup);
	ret_if(NULL == cb_data);

	Evas_Object *layout = NULL;
	Evas_Object *entry = NULL;
	Evas_Object *btn_done = NULL;
	Evas_Object *btn_cancel = NULL;
	char *temp_txt = NULL;

	/* Destroy popup if exists */
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	if ((ugd->sim_lock_retry_account > 3) ||
			(ugd->sim_lock_retry_account <= 0)) {
		ERR("Invalid ugd->sim_lock_retry_account[%d]", ugd->sim_lock_retry_account);
		return;
	}

	ugd->popup = elm_popup_add(ugd->nf);
	evas_object_size_hint_weight_set(ugd->popup, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	layout = elm_layout_add(ugd->popup);
	elm_layout_theme_set(layout, "layout", "popup", "entrypasswordview");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	cst_util_domain_translatable_part_text_set(ugd->popup, "title,text", I_(CST_STR_ENTER_PIN2));

	entry = _cst_create_ime_editfield(ugd, layout, ime_type, NULL);
/*	elm_object_style_set(entry, "editfield/password/popup");*/
	elm_object_signal_emit(entry, "elm,action,hide,search_icon", "");
	elm_object_part_content_set(layout, "elm.swallow.content", entry);
	elm_object_content_set(ugd->popup, layout);

	/*Update the guide-text if PIN2 attempts is less than '3'*/
	if (ugd->sim_lock_retry_account == 1) {
		temp_txt = g_strdup_printf("%s", T_(CST_STR_INCORRECT_PIN2_1_ATTEMPT_REMAINING));
	} else {
		temp_txt = g_strdup_printf(T_(CST_STR_INCORRECT_PIN2_ATTEMPTS_REMAINING), ugd->sim_lock_retry_account);
	}

	cst_util_domain_translatable_part_text_set(entry, "elm.guide", temp_txt);
	g_free(temp_txt);

	/* Cancel Button */
	btn_cancel = elm_button_add(ugd->popup);
	cst_util_domain_translatable_text_set(btn_cancel, I_(CST_STR_POP_CANCEL));
	elm_object_part_content_set(ugd->popup, "button1", btn_cancel);
	if (cancel_cb) {
		evas_object_smart_callback_add(btn_cancel, "clicked", cancel_cb, cb_data);
	} else {
		evas_object_smart_callback_add(btn_cancel, "clicked",
				__cst_create_pin2_confirm_popup_cancel_cb, ugd);
	}
	if (c_item)
		c_item[0] = btn_cancel;

	/* Done Button */
	btn_done = elm_button_add(ugd->popup);
	cst_util_domain_translatable_text_set(btn_done, done_btn_text);
	elm_object_part_content_set(ugd->popup, "button2", btn_done);
	evas_object_smart_callback_add(btn_done, "clicked", done_cb, cb_data);
	elm_object_disabled_set(btn_done, EINA_TRUE);
	if (c_item)
		c_item[1] = btn_done;

	if (cancel_cb) {
		eext_object_event_callback_add(ugd->popup, EEXT_CALLBACK_BACK, cancel_cb,
				cb_data);
	} else {
		eext_object_event_callback_add(ugd->popup, EEXT_CALLBACK_BACK,
				__cst_create_pin2_confirm_popup_cancel_cb, ugd);
	}
	evas_object_show(ugd->popup);
	elm_object_focus_set(ugd->dg_entry, EINA_TRUE);
	elm_entry_cursor_end_set(ugd->dg_entry);

	return;
}

void _cst_fdn_disable_main_screen_show_unlock_pin2_popup(void *data)
{
	ret_if(NULL == data);
	ENTER(_cst_fdn_disable_main_screen_show_unlock_pin2_popup);
	CstUgData_t *ugd = (CstUgData_t *)data;

	/* Come Back to FDN main screen */
	if (fdn_main_screen_navi_it) {
		elm_naviframe_item_pop_to(fdn_main_screen_navi_it);
	}

	/* Disable the FDN menu */
	if (fdnsetting_gl) {
		Elm_Object_Item *item = elm_genlist_first_item_get(fdnsetting_gl);
		while (item) {
			elm_object_item_disabled_set(item, EINA_TRUE);
			item = elm_genlist_item_next_get(item);
		}
	}

	/* PIN2 has been blocked. Ask if user wants to change the PIN2 */
	ugd->popup = _cst_create_1btn_popup(ugd->nf, ugd,
			T_(CST_STR_UNBLOCK_PIN2_MESSAGE), T_(CST_STR_PIN2_BLOCKED),
			_cst_pin2_blocked_popup_confirm_btn_cb, ugd);
}

#endif  /* _CALL_SET_FDN_SUPPORT */
