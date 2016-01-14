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
#include <ITapiSim.h>

#include <string.h>
#include "cst-widget.h"
#include "cst-call-setting.h"
#include "cst-delete-list.h"
#include "cst-widget.h"
#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-util.h"

#include "msg.h"
#include "cst-reject-msg.h"
#include "cst-fdn-setting.h"
#include "cst-fdn-list.h"
#include "cst-tapi-request.h"

#define MAX_PRECONFIG_NUM		8
#define DEF_BUF_LEN	(512)

static Elm_Genlist_Item_Class *itc_1text_2text1icon = NULL;
static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_select_all = NULL;

typedef struct {
	Evas_Object *main_layout;
	Evas_Object *genlist;
	Evas_Object *check;
	Eina_Bool select_all_checked;

	int total_count;
	int checked_count;
	char *title;

	void *ugd;
} Delete_List_View_Data;

static void __cst_destroy_genlist_item_styles(void);
static void __cst_select_all_check_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void __cst_check_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void __cst_update_check_state(Delete_List_View_Data *vd);

#ifdef _CALL_SET_FDN_SUPPORT
static char *__cst_gl_label_get_fdn_delete_list(void *data, Evas_Object *obj, const char *part);
static void __cst_on_click_FDN_delete_list_btn_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void __cst_click_FDN_delete_list_ime_done_cb(void *data, Evas_Object *obj, void *event_info);
static Elm_Object_Item *__cst_fdn_delete_list_get_contact_list_item_by_index(void *data, int item_index);
static void __cst_click_FDN_delete_list_ime_cancel_cb(void *data,
		Evas_Object *obj, void *event_info);

static gboolean keypad_enabled = EINA_FALSE;
#endif  /* _CALL_SET_FDN_SUPPORT */
static Delete_List_View_Data *g_vd = NULL;

static int kind_of_delete_list;
static Evas_Object *ec_item[3];
static Evas_Object *ec_obj_item[3];

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_1text_2text1icon) {
		_cst_destroy_genlist_item_class(itc_1text_2text1icon);
		itc_1text_2text1icon = NULL;
	}
	if (itc_1text_1icon) {
		_cst_destroy_genlist_item_class(itc_1text_1icon);
		itc_1text_1icon = NULL;
	}
	if (itc_select_all) {
		_cst_destroy_genlist_item_class(itc_select_all);
		itc_select_all = NULL;
	}
}

static Eina_Bool __cst_on_click_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_back_button);
	retv_if(data == NULL, EINA_TRUE);

	CstUgData_t *ugd = (CstUgData_t *)data;
	retv_if(ugd == NULL, EINA_TRUE);

	elm_naviframe_item_pop_cb_set(elm_naviframe_top_item_get(ugd->nf), NULL, NULL);

	elm_naviframe_item_pop(ugd->nf);

	memset(ec_item, 0, sizeof(ec_item));

	_cst_destroy_delete_list();

	return EINA_TRUE;
}

static void __cst_on_click_delete_confirm(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_delete_confirm);
	ret_if(NULL == data);
	Delete_List_View_Data *vd = NULL;
	CstUgData_t *ugd = NULL;
	Elm_Object_Item *prev_it = NULL;
	Elm_Object_Item *del_list_it = NULL;
	Elm_Object_Item *del_list_select_all = NULL;
	CstGlItemData_t *del_item_data, *prev_item_data;
	int i = 0;
	vd = (Delete_List_View_Data *)data;
	ret_if(vd == NULL);

	ugd = vd->ugd;
	ret_if(ugd == NULL);

	/* Destroy the popup */
	if (ugd && ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	if (vd->checked_count < 1) {
		ERR("No items selected in the genlist to delete, returning!");
		return;
	}

	/* Fetch the first item which is a seperator and ignore it */
	prev_it = elm_genlist_first_item_get(ugd->backup_genlist);
	if (prev_it == NULL) {
		DBG("prev_it = NULL, returning");
		return;
	}

	/* First item is select_all...so ignoring it */
	del_list_select_all = elm_genlist_first_item_get(vd->genlist);
	del_list_it = elm_genlist_item_next_get(del_list_select_all);
	if (del_list_it == NULL) {
		DBG("del_list_it = NULL, returning");
		return;
	}

	switch (ugd->kind_of_delete_list) {
	case CST_DL_FDN_CONTACTS:
		__cst_on_click_FDN_delete_list_btn_delete_cb(data, obj, event_info);
		return;
	case CST_DL_REJECT_CALL_WITH_MSG:
		/*In Reject Message "Select All" is as first Item */
		while (prev_it != NULL && del_list_it != NULL) {
			prev_item_data = (CstGlItemData_t *)elm_object_item_data_get(prev_it);
			if (prev_item_data == NULL) {
				DBG("prev_item_data = NULL, returning");
				return;
			}

			if (prev_item_data->index != -1) {
				del_item_data = (CstGlItemData_t *)elm_object_item_data_get(del_list_it);
				if (del_item_data == NULL) {
					DBG("del_item_data = NULL, returning");
					return;
				}
				if (del_item_data->check == EINA_FALSE) {
					char *new_message = elm_entry_markup_to_utf8(_cst_get_reject_message(prev_item_data->index, EINA_FALSE, EINA_FALSE));
					_cst_set_reject_message(i, new_message);
					free(new_message);
					new_message = NULL;
					i++;
				}
				del_list_it = elm_genlist_item_next_get(del_list_it);
			}
			prev_it = elm_genlist_item_next_get(prev_it);
		}
		_cst_set_num_of_reject_message(i);
		break;

	default:
		DBG("wrong request");
		return;
		break;
	}

	__cst_on_click_back_button(ugd, NULL);
	return;
}

static void __cst_delete_list_lang_changed(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_delete_list_lang_changed);
	ret_if(NULL == data);
	Delete_List_View_Data *vd = (Delete_List_View_Data *)data;
	CstUgData_t *ugd = vd->ugd;
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ugd->nf);
	char *txt = NULL;

	/* Update the popup text on language change */
	if (ugd->popup) {
		txt = g_strdup(I_(CST_STR_DELETE_Q));
		cst_util_domain_translatable_text_set(ugd->popup, txt);

		if (txt) {
			g_free(txt);
		}
	}

	/* Update the title text on language change */
	if (kind_of_delete_list == CST_DL_REJECT_CALL_WITH_MSG) {
		txt = g_strdup_printf("%d/%d", vd->checked_count, vd->total_count);
	} else {
		txt = g_strdup_printf(T_(CST_STR_PD_SELECTED), vd->checked_count);
	}
	elm_object_item_part_text_set(navi_it, "elm.text.title", txt);

	if (txt) {
		g_free(txt);
	}
}

#if 0 /* Function not used. */

static void __cst_on_click_delete_button(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_delete_button);
	ret_if(NULL == data);
	Delete_List_View_Data *vd = (Delete_List_View_Data *)data;
	CstUgData_t *ugd = vd->ugd;
	char *txt = NULL;

	txt = g_strdup(I_(CST_STR_DELETE_Q));

#ifdef _CALL_SET_FDN_SUPPORT
	if (CST_DL_FDN_CONTACTS == ugd->kind_of_delete_list) {
		__cst_on_click_FDN_delete_list_btn_delete_cb(data, obj, event_info);
	} else {
		ugd->popup = _cst_create_2btn_delete_popup(ugd->nf, ugd, txt, __cst_on_click_delete_confirm, data);
	}
#else	  /* _CALL_SET_FDN_SUPPORT */
	ugd->popup = _cst_create_2btn_delete_popup(ugd->nf, ugd, txt, __cst_on_click_delete_confirm, data);
#endif    /* !_CALL_SET_FDN_SUPPORT */

	if (txt) {
		g_free(txt);
	}
}
#endif

static void __cst_on_click_cancel_button(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_on_click_cancel_button);
	ret_if(NULL == data);
	Delete_List_View_Data *vd = (Delete_List_View_Data *)data;

	__cst_on_click_back_button(vd->ugd, NULL);
}

#ifdef _CALL_SET_FDN_SUPPORT
void _cst_FDN_delete_list_changed_editfield_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	const char *entry_str = elm_entry_entry_get(obj);
	DBG("entry_str = %s", entry_str);

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

void _cst_FDN_delete_list_input_panel_event_cb(void *data,
		Ecore_IMF_Context *imf_context, int value)
{
	ENTER(_cst_FDN_delete_list_input_panel_event_cb);
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

static void __cst_on_click_FDN_delete_list_btn_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_FDN_delete_list_btn_delete_cb);
	ret_if(!data);
	Delete_List_View_Data *vd = (Delete_List_View_Data *)data;
	CstUgData_t *ugd = vd->ugd;
	const char *entry_input = NULL;

	memset(ec_item, 0, sizeof(ec_item));

	_cst_create_pin2_confirm_popup(ugd, CST_IME_FDN_DELETE, I_(CST_STR_HEADER_DELETE),
			__cst_click_FDN_delete_list_ime_done_cb,
			__cst_click_FDN_delete_list_ime_cancel_cb, (void *)ugd, ec_item);

	entry_input = elm_entry_entry_get(ugd->dg_entry);

	if (ec_item[1] != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	}
	g_vd = vd;
}

static Eina_Bool _cst__FDN_delete_list_naviframe_item_pop_cb(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;

	ugd->dg_entry = NULL;
	memset(ec_item, 0, sizeof(ec_item));

	__cst_on_click_back_button(ugd, NULL);
	ugd->back_button = _cst_get_navifr_prev_btn(ugd->nf);

	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool _cst_click_FDN_delete_list_ime_back_cb(void *data,
		Elm_Object_Item *it)
{
	ENTER(_cst_click_FDN_delete_list_ime_back_cb);
	retv_if(!data, EINA_TRUE);
	CstUgData_t *ugd = (CstUgData_t *)data;

	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);

	evas_object_smart_callback_del(ugd->dg_entry, "changed",
			_cst_FDN_delete_list_changed_editfield_cb);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context,
				ECORE_IMF_INPUT_PANEL_STATE_EVENT,
				_cst_FDN_delete_list_input_panel_event_cb);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	keypad_enabled = EINA_FALSE;

	ecore_timer_add(CST_NAVIFRAME_ITEM_POP_TIMER,
			_cst__FDN_delete_list_naviframe_item_pop_cb, ugd);

	return EINA_FALSE;
}

static void __cst_click_FDN_delete_list_ime_done_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_click_FDN_delete_list_ime_done_cb);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Delete_List_View_Data *vd  = g_vd;
	char *password = NULL;
	int len_password = 0;

	password = elm_entry_markup_to_utf8(elm_entry_entry_get((const Evas_Object *)ugd->dg_entry));
	if (password == NULL) {
		_cst_click_FDN_delete_list_ime_back_cb(data, NULL);

		/* Show error popup */
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);

		return;
	}

	len_password = strlen(password);
	SEC_DBG("* password len [%d]", len_password);
	SEC_DBG("* password [%s]", password);

	if (ugd->dg_entry) {
		evas_object_del(ugd->dg_entry);
		ugd->dg_entry = NULL;
	}

	if (4 > len_password) {  /* Back to FDN Delete ListView */
		SEC_ERR("Invalid PIN2 entered = %s", password);

		_cst_click_FDN_delete_list_ime_back_cb(data, NULL);
		/* Show error popup */
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);
	} else { /* Back to FDN Contact ListView */
		/* Authenticate the PIN2 entered */
		CallSettingSimNxtReqParams_t nxt_re_params = {0, };
		nxt_re_params.pin2 = (unsigned char *)password;
		nxt_re_params.name = NULL;
		nxt_re_params.number = NULL;
		nxt_re_params.pin2_len = len_password;
		nxt_re_params.req_id = TAPI_SIM_REQ_DELETE_PHNBUK_REC;
		_cst_verify_pin2(ugd, &nxt_re_params, cst_fdn_list_tapi_sim_response_update_cb, NULL);

		/* Update the FDN contact flag for checked items inorder to delete after successfull authentication */
		Elm_Object_Item *del_list_it = NULL;
		Elm_Object_Item *del_list_select_all = NULL;
		CstGlItemData_t *del_item_data = NULL;
		int item_index = 0;

		/* First item is select_all...so ignoring it */
		del_list_select_all = elm_genlist_first_item_get(vd->genlist);
		del_list_it = elm_genlist_item_next_get(del_list_select_all);
		if (del_list_it == NULL) {
			DBG("del_list_it = NULL, returning");
			return;
		}

		while (del_list_it != NULL) {
			del_item_data = (CstGlItemData_t *)elm_object_item_data_get(del_list_it);
			if (del_item_data == NULL) {
				DBG("del_item_data = NULL, returning");
				return;
			}

			if (del_item_data->check == EINA_TRUE) { /* Delete the FDN Contact*/
				Elm_Object_Item *del_item = NULL;
				CstFDNGlItemData_t *item_data = NULL;

				del_item = __cst_fdn_delete_list_get_contact_list_item_by_index(ugd, item_index);
				item_data =	(CstFDNGlItemData_t *)elm_object_item_data_get(del_item);
				if (item_data == NULL) {
					DBG("item_data = NULL, returning");
					return;
				}
				item_data->item_delete_status = EINA_TRUE;
			}
			del_list_it = elm_genlist_item_next_get(del_list_it);
			item_index++;
		}
	}

	if (password) {
		g_free(password);
		password = NULL;
	}

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

static void __cst_click_FDN_delete_list_ime_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_click_FDN_delete_list_ime_cancel_cb);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}
}

static char *__cst_gl_label_get_fdn_delete_list(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Elm_Object_Item *del_list_item = NULL;

	del_list_item = __cst_fdn_delete_list_get_contact_list_item_by_index(ugd, item_data->index-1);
	if (del_list_item == NULL) {
		DBG("Fetching the contact failed.... returning");
		return NULL;
	}

	CstFDNGlItemData_t *list_item_data = (CstFDNGlItemData_t *)elm_object_item_data_get(del_list_item);
	if (list_item_data == NULL) {
		DBG("Fetching the item_data failed.... returning");
		return NULL;
	}

	if (strcmp(part, "elm.text.main.left.top") == 0) {
		return g_strdup((const char *)list_item_data->phbk_info->name);
	} else if (strcmp(part, "elm.text.sub.left.bottom") == 0) {
		return g_strdup((const char *)list_item_data->phbk_info->number);
	}
	return NULL;
}

static Elm_Object_Item *__cst_fdn_delete_list_get_contact_list_item_by_index(void *data, int item_index)
{
	ENTER(__cst_fdn_delete_list_get_contact_list_item_by_index);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int sel_index = 0;
	Elm_Object_Item *del_list_item = NULL;

	/* Fetch the first item which is a separator and ignore it */
	del_list_item = elm_genlist_first_item_get(ugd->backup_genlist);
	if (del_list_item == NULL) {
		DBG("Fetching the item_data failed.... returning");
		return NULL;
	}

	/* Traverse to the next item which is the FDN contact item */
	del_list_item = elm_genlist_item_next_get(del_list_item);

	while ((sel_index != item_index) && (del_list_item != NULL)) {
		del_list_item = elm_genlist_item_next_get(del_list_item);
		sel_index++;
	}

	return del_list_item;
}
#endif  /* _CALL_SET_FDN_SUPPORT */

static void __cst_update_check_state(Delete_List_View_Data *vd)
{
	ret_if(vd == NULL);
	ENTER(__cst_update_check_state);
	CstUgData_t *ugd = (CstUgData_t *)vd->ugd;
	ret_if(ugd == NULL);
	char *label = NULL;
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ugd->nf);

	label = calloc(CST_MAX_ITEM_TEXT_BUFFER_LEN + 1, sizeof(char));
	ret_if(label == NULL);

	if (kind_of_delete_list == CST_DL_REJECT_CALL_WITH_MSG) {
		snprintf(label, CST_MAX_ITEM_TEXT_BUFFER_LEN + 1, "%d/%d", vd->checked_count, vd->total_count);
	} else {
		snprintf(label, CST_MAX_ITEM_TEXT_BUFFER_LEN + 1, T_(CST_STR_PD_SELECTED), vd->checked_count);
	}

	elm_object_item_part_text_set(navi_it, "elm.text.title", label);
	if (ec_obj_item[1] != NULL) {
		if (vd->checked_count > 0) {
			elm_object_disabled_set(ec_obj_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_obj_item[1], EINA_TRUE);
		}
	}
	free(label);
}


static void __cst_select_all_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_select_all_check_changed_cb);
	CstGlItemData_t *it_data;
	Elm_Object_Item *it;
	Delete_List_View_Data *vd = (Delete_List_View_Data *)data;
	ret_if(NULL == vd);

	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);

	vd->select_all_checked = !vd->select_all_checked;
	if (vd->select_all_checked) {
		vd->checked_count = vd->total_count;
	} else {
		vd->checked_count = 0;
	}

	elm_check_state_set(vd->check, vd->select_all_checked);

	it = elm_genlist_first_item_get(vd->genlist);
	it = elm_genlist_item_next_get(it);
	while (it) {
		it_data = elm_object_item_data_get(it);
		if (it_data) {
			it_data->check = vd->select_all_checked;
			elm_check_state_set(it_data->eo_check, it_data->check);
		}
		it = elm_genlist_item_next_get(it);
	}
	__cst_update_check_state(vd);
}

static void __cst_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	ENTER(__cst_check_changed_cb);

	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	Delete_List_View_Data *vd =
		(Delete_List_View_Data *)evas_object_data_get(
			elm_object_item_widget_get(item_data->gl_item), "delete_view_data");

	ret_if(NULL == vd);

	if (item_data->index != 0) {
		if (item_data->check) {
			vd->checked_count++;
		} else {
			vd->checked_count--;
		}
	}

	/* update select all check button */
	if (vd->total_count == vd->checked_count) {
		vd->select_all_checked = EINA_TRUE;
	} else {
		vd->select_all_checked = EINA_FALSE;
	}
	elm_check_state_set(vd->check, vd->select_all_checked);
	__cst_update_check_state(vd);
}

static char *__cst_gl_label_get_delete_list(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (!strcmp(part, "elm.text")) {
		char *buf = strdup(dgettext(UGNAME, (const char *)item_data->label));
		return buf;
	}

	return NULL;
}

static Evas_Object *__cst_gl_icon_get_delete_list(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (!strcmp(part, "elm.swallow.end")) {
		item_data->eo_check = elm_check_add(obj);
		evas_object_size_hint_weight_set(item_data->eo_check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(item_data->eo_check, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_check_state_pointer_set(item_data->eo_check, &item_data->check);
		if (item_data->index != 0) {
			evas_object_repeat_events_set(item_data->eo_check, EINA_FALSE);
			evas_object_smart_callback_add(item_data->eo_check, "changed",
					__cst_check_changed_cb, item_data);
			evas_object_propagate_events_set(item_data->eo_check, EINA_FALSE);
		}
#ifdef _CALL_SET_TTS_SUPPORT
		/* Unregister the access object */
		if (elm_config_access_get() && item_data->eo_check) {
			elm_access_object_unregister(item_data->eo_check);
		}
#endif
		return item_data->eo_check;
	}

	return NULL;
}

static void __cst_gl_del_delete_list(void *data, Evas_Object *obj)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		if (item_data->label) {
			free(item_data->label);
		}
		free(item_data);
	}

	return;
}

static void __cst_gl_sel_delete_list(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_gl_sel_delete_list);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	Delete_List_View_Data *vd =
		(Delete_List_View_Data *)evas_object_data_get(obj, "delete_view_data");
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	ret_if(NULL == vd);

	item_data->check = !item_data->check;
	if (item_data->check) {
		vd->checked_count++;
	} else {
		vd->checked_count--;
	}
	elm_check_state_pointer_set(item_data->eo_check, &item_data->check);

	if (vd->total_count == vd->checked_count) {
		vd->select_all_checked = EINA_TRUE;
	} else {
		vd->select_all_checked = EINA_FALSE;
	}

	elm_check_state_set(vd->check, vd->select_all_checked);
	__cst_update_check_state(vd);
}

static char *__cst_gl_label_get_select_all(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part, "elm.text")) {
		return strdup(T_(CST_STR_BODY_SELECT_ALL));
	}
	return NULL;
}

static Evas_Object *__cst_gl_content_get_select_all(void *data, Evas_Object *obj, const char *part)
{
	Delete_List_View_Data *vd = (Delete_List_View_Data *)data;

	if (!strcmp(part, "elm.swallow.end")) {
		vd->check = elm_check_add(obj);
		evas_object_propagate_events_set(vd->check, EINA_FALSE);
		evas_object_smart_callback_add(vd->check, "changed", __cst_select_all_check_changed_cb, vd);
		return vd->check;
	}
	return NULL;
}

static Evas_Object *__cst_create_delete_list_genlist(Delete_List_View_Data *vd)
{
	ENTER(__cst_create_delete_list_genlist);
	retv_if(NULL == vd, NULL);
	int index = 0, i = 0;
	CstGlItemData_t *item_data;
	CstUgData_t *ugd = vd->ugd;
	retv_if(ugd == NULL, NULL);

	vd->genlist = elm_genlist_add(ugd->nf);
	retv_if(NULL == vd->genlist, NULL);
	elm_genlist_mode_set(vd->genlist, ELM_LIST_COMPRESS);
	evas_object_size_hint_weight_set(vd->genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(vd->genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	if (!itc_select_all) {
		itc_select_all = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_select_all,
				__cst_gl_content_get_select_all,
				NULL, NULL);
	}

	if (ugd->kind_of_delete_list == CST_DL_FDN_CONTACTS) {
#ifdef _CALL_SET_FDN_SUPPORT
		if (!itc_1text_2text1icon) {
			itc_1text_2text1icon = _cst_create_genlist_item_class("2line.top",
					__cst_gl_label_get_fdn_delete_list,
					__cst_gl_icon_get_delete_list,
					NULL, __cst_gl_del_delete_list);
		}
		itc_1text_2text1icon->decorate_all_item_style = NULL;
		elm_genlist_decorate_mode_set(vd->genlist, EINA_FALSE);
#endif  /* _CALL_SET_FDN_SUPPORT */
	} else if (ugd->kind_of_delete_list == CST_DL_REJECT_CALL_WITH_MSG) {
		if (!itc_1text_2text1icon) {
			itc_1text_2text1icon = _cst_create_genlist_item_class("type1",
					__cst_gl_label_get_delete_list,
					__cst_gl_icon_get_delete_list,
					NULL, __cst_gl_del_delete_list);
		}
		if (!itc_1text_1icon) {
			itc_1text_1icon = _cst_create_genlist_item_class("1line",
					__cst_gl_label_get_delete_list,
					__cst_gl_icon_get_delete_list,
					NULL, __cst_gl_del_delete_list);
		}
	} else {
		if (!itc_1text_2text1icon) {
			itc_1text_2text1icon = _cst_create_genlist_item_class("1line",
					__cst_gl_label_get_delete_list,
					__cst_gl_icon_get_delete_list,
					NULL, __cst_gl_del_delete_list);
		}
		if (!itc_1text_1icon) {
			itc_1text_1icon = _cst_create_genlist_item_class("1line",
					__cst_gl_label_get_delete_list,
					__cst_gl_icon_get_delete_list,
					NULL, __cst_gl_del_delete_list);
		}
	}
	elm_genlist_item_append(vd->genlist, itc_select_all, vd,
		NULL, ELM_GENLIST_ITEM_NONE, __cst_select_all_check_changed_cb, (void *)vd);

	for (index = i; index < vd->total_count; index++) {
		item_data = calloc(1, sizeof(CstGlItemData_t));
		retv_if(item_data == NULL, NULL);
		switch (ugd->kind_of_delete_list) {
		case CST_DL_REJECT_CALL_WITH_MSG:
			item_data->label = _cst_get_reject_message(index, EINA_TRUE, EINA_FALSE);
			break;

#ifdef _CALL_SET_FDN_SUPPORT
		case CST_DL_FDN_CONTACTS:
			break;
#endif  /* _CALL_SET_FDN_SUPPORT */
		default:
			DBG("wrong request");
			free(item_data);
			item_data = NULL;
			return NULL;
		}

		item_data->index = index+1;
		item_data->ugd = ugd;
		item_data->gl_item = elm_genlist_item_append(vd->genlist,
							 itc_1text_2text1icon,
							 item_data,
							 NULL, ELM_GENLIST_ITEM_NONE,
							 __cst_gl_sel_delete_list, item_data);
	}
	elm_genlist_select_mode_set(vd->genlist, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_scroller_policy_set(vd->genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(vd->genlist);
	return vd->genlist;
}

void _cst_create_delete_list(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_create_delete_list);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Delete_List_View_Data *delete_list_view = NULL;
	Elm_Object_Item *navi_it;
	Elm_Object_Item *top_it = NULL;
	Elm_Object_Item *gen_item = NULL;
	int top_item_idx = -1;
	int posret = 0;;
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	if (ugd->rejct_popup) {
		evas_object_del(ugd->rejct_popup);
		ugd->rejct_popup = NULL;
	}
	if (ugd->rejctlist_popup) {
		evas_object_del(ugd->rejctlist_popup);
		ugd->rejctlist_popup = NULL;
	}
	if (ugd->fdnmore_popup) {
		evas_object_del(ugd->fdnmore_popup);
		ugd->fdnmore_popup = NULL;
	}
#ifdef _CALL_SET_FDN_SUPPORT
	if (CST_DL_FDN_CONTACTS == ugd->kind_of_delete_list &&
		ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) {
		_cst_fdn_disable_main_screen_show_unlock_pin2_popup((void *)ugd);
		return;
	}
#endif

	memset(ec_obj_item, 0, sizeof(ec_obj_item));

	kind_of_delete_list = ugd->kind_of_delete_list;
	delete_list_view = (Delete_List_View_Data *)calloc(1, sizeof(Delete_List_View_Data));
	ret_if(delete_list_view == NULL);
	delete_list_view->ugd = ugd;
	DBG("Request list : %d", kind_of_delete_list);
	switch (kind_of_delete_list) {
	case CST_DL_REJECT_CALL_WITH_MSG:
		delete_list_view->total_count = _cst_get_num_of_reject_message();
		delete_list_view->title = g_strdup_printf("%d/%d", delete_list_view->checked_count, delete_list_view->total_count);
		break;
#ifdef _CALL_SET_FDN_SUPPORT
	case CST_DL_FDN_CONTACTS:
		delete_list_view->total_count = ugd->fdn_contacts_count;
		delete_list_view->title = g_strdup_printf(T_(CST_STR_PD_SELECTED), delete_list_view->checked_count);
		break;
#endif  /* _CALL_SET_FDN_SUPPORT */
	default:
		DBG("wrong request");
		free(delete_list_view);
		delete_list_view = NULL;
		return;
	}

	if (0 == delete_list_view->total_count) {
		free(delete_list_view);
		delete_list_view = NULL;
		return;
	}

	__cst_create_delete_list_genlist(delete_list_view);
	evas_object_data_set(delete_list_view->genlist, "delete_view_data", delete_list_view);

	/* Move to previous top item */
	evas_object_geometry_get(ugd->backup_genlist, &x, &y, &w, &h);
	gen_item = elm_genlist_at_xy_item_get(ugd->backup_genlist, x, y+48, &posret);	/* if half of the item is hidden, consider the next item as the top item */
	if (gen_item) {
		top_item_idx = elm_genlist_item_index_get(gen_item);
	}

	if (top_item_idx > 1) {
		top_it = elm_genlist_nth_item_get(delete_list_view->genlist, top_item_idx);
		elm_genlist_item_show(top_it, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}

	/*Create layout */
	delete_list_view->main_layout = elm_layout_add(ugd->nf);
	elm_layout_file_set(delete_list_view->main_layout, EDJ_NAME, "toolbar");
	evas_object_size_hint_weight_set(delete_list_view->main_layout,
									 EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(delete_list_view->main_layout,
									EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_part_content_set(delete_list_view->main_layout,
								"elm.swallow.contents", delete_list_view->genlist);

	navi_it = elm_naviframe_item_push(ugd->nf,
									  delete_list_view->title, NULL, NULL,
									  delete_list_view->main_layout, NULL);

	evas_object_smart_callback_add(delete_list_view->genlist,
			"language,changed", __cst_delete_list_lang_changed, delete_list_view);
	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_NAVI_BTN_CANCEL), NULL,
										__cst_on_click_cancel_button,
										(char *)I_(CST_STR_NAVI_BTN_DELETE), NULL,
										__cst_on_click_delete_confirm,
										(void *)delete_list_view, navi_it,
										ec_obj_item);
	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_click_back_button, ugd);
	if (ec_obj_item[1] != NULL) {
		elm_object_disabled_set(ec_obj_item[1], EINA_TRUE);
	}
	__cst_update_check_state(delete_list_view);

	g_vd = delete_list_view;

	if (delete_list_view->title) {
		g_free(delete_list_view->title);
		delete_list_view->title = NULL;
	}
	LEAVE();
}

void _cst_destroy_delete_list(void)
{
	ENTER(_cst_destroy_delete_list);

	if (g_vd) {
		if (g_vd->title) {
			free(g_vd->title);
			g_vd->title = NULL;
		}
		evas_object_smart_callback_del(g_vd->genlist, "language,changed",
				__cst_delete_list_lang_changed);
		evas_object_data_set(g_vd->genlist, "delete_view_data", NULL);
		free(g_vd);
		g_vd = NULL;
	}
	__cst_destroy_genlist_item_styles();
}

