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
#include <string.h>
#include "cst-common.h"
#include "cst-common-string.h"

#include "cst-widget.h"
#include "cst-util.h"
#include "cst-forwarding.h"

static Elm_Genlist_Item_Class *itc_ime = NULL;
static Elm_Genlist_Item_Class *itc_sep = NULL;
static char *def_ime_number = NULL;

static Elm_Genlist_Item_Class *itc_waiting_time = NULL;
static Elm_Genlist_Item_Class *itc_waiting_time_expand = NULL;
static CstGlItemData_t *waitingtime_item = NULL;
static int curr_wait_time = -1;
static Evas_Object *waitingtime_radio_btn = NULL;
static Evas_Object *waiting_time_popup = NULL;
static CstGlItemDisplayInfo_t list_cf_ime_dep2_waiting_time[] = {
	{2, CST_CF_WAIT_TIME_5_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_5_SEC},
	{2, CST_CF_WAIT_TIME_10_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_10_SEC},
	{2, CST_CF_WAIT_TIME_15_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_15_SEC},
	{2, CST_CF_WAIT_TIME_20_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_20_SEC},
	{2, CST_CF_WAIT_TIME_25_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_25_SEC},
	{2, CST_CF_WAIT_TIME_30_SEC, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_RADIO_1TEXT, (void *)CST_CF_WAIT_TIME_IDX_30_SEC},

	{2, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static int __cst_cf_ime_get_wait_time_index(int wait_time);
static int __cst_cf_ime_get_wait_time(int wait_time_idx);
static void __cst_cf_ime_gl_sel_waiting_time(void *data, Evas_Object *obj, void *event_info);
#if 0 /* Function not used */
static void __cst_cf_ime_waiting_time_collapse_req(void *data, Evas_Object *obj, void *event_info);
static void __cst_cf_ime_waiting_time_expand_req(void *data, Evas_Object *obj, void *event_info);
#endif
static void __cst_cf_ime_waiting_time_expand(void *data, Evas_Object *obj, void *event_info);
static void __cst_cf_ime_on_click_waitingtime_expand(void *data, Evas_Object *obj, void *event_info);
static void __cst_cf_ime_gl_del(void *data, Evas_Object *obj);
static Evas_Object *__cst_cf_ime_create_wait_time_radio_icon(Evas_Object *obj, Evas_Object *rdg, int value, int offset);
static Evas_Object *__cst_cf_ime_get_wait_time_sub_exp_icon(void *data, Evas_Object *obj, const char *part);
static char *__cst_cf_ime_get_wait_time_sub_exp_label(void *data, Evas_Object *obj, const char *part);
static char *__cst_cf_ime_cf_onoff_get_waiting_time_label(void *data, Evas_Object *obj, const char *part);
static Eina_Bool __cst_cf_is_waiting_time_available(int wait_time);

int _cst_get_input_number(char *buffer, void *data)
{
	ENTER(_cst_get_input_number);
	CstUgData_t *ugd = (CstUgData_t *)data;
	const char *entry_input;
	Ecore_IMF_Context *imf_ctx;

	entry_input = elm_entry_entry_get(ugd->dg_entry);
	retv_if(NULL == entry_input, -1);
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

void _cst_cf_ime_destroy_genlist_item_class(void)
{
	ENTER(_cst_cf_ime_destroy_genlist_item_class);
	DBG("\n\n *** Testing: CF-IME screen *** \n\n");

	if (itc_ime) {
		_cst_destroy_genlist_item_class(itc_ime);
		itc_ime = NULL;
	}
	if (itc_sep) {
		_cst_destroy_genlist_item_class(itc_sep);
		itc_sep = NULL;
	}
	if (itc_waiting_time) {
		_cst_destroy_genlist_item_class(itc_waiting_time);
		itc_waiting_time = NULL;
	}
	if (itc_waiting_time_expand) {
		_cst_destroy_genlist_item_class(itc_waiting_time_expand);
		itc_waiting_time_expand = NULL;
	}
}

static Evas_Object *__cst_gl_icon_get_cf_ime(void *data, Evas_Object *obj,
		const char *part)
{
	ENTER(__cst_gl_icon_get_cf_ime);
	retv_if(data == NULL, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;

	if (!strcmp(part, "elm.icon.entry")) {
		if (strlen(ugd->entry_string) > 0) {
			def_ime_number = ugd->entry_string;
		}
		Evas_Object *editfield = _cst_create_ime_editfield(ugd, obj,
				CST_IME_CALL_FORWARD, def_ime_number);
		retv_if(editfield == NULL, NULL);
		return editfield;
	} else if (!strcmp(part, "elm.icon")) {
		return _cst_create_ime_contacts_btn_obj(obj, (void *)ugd);
	}

	return NULL;
}

static int __cst_cf_ime_get_wait_time_index(int wait_time)
{
	ENTER(__cst_cf_ime_get_wait_time_index);
	int index = -1;

	if (wait_time == CST_CF_WAIT_TIME_5_SEC) {
		index = CST_CF_WAIT_TIME_IDX_5_SEC;
	} else if (wait_time == CST_CF_WAIT_TIME_10_SEC) {
		index = CST_CF_WAIT_TIME_IDX_10_SEC;
	} else if (wait_time == CST_CF_WAIT_TIME_15_SEC) {
		index = CST_CF_WAIT_TIME_IDX_15_SEC;
	} else if (wait_time == CST_CF_WAIT_TIME_20_SEC) {
		index = CST_CF_WAIT_TIME_IDX_20_SEC;
	}  else if (wait_time == CST_CF_WAIT_TIME_25_SEC) {
		index = CST_CF_WAIT_TIME_IDX_25_SEC;
	}  else {
		index = CST_CF_WAIT_TIME_IDX_30_SEC;
	}

	return index;
}

static int __cst_cf_ime_get_wait_time(int wait_time_idx)
{
	ENTER(__cst_cf_ime_get_wait_time);
	int wait_time = -1;

	if (wait_time_idx == CST_CF_WAIT_TIME_IDX_5_SEC) {
		wait_time = CST_CF_WAIT_TIME_5_SEC;
	} else if (wait_time_idx == CST_CF_WAIT_TIME_IDX_10_SEC) {
		wait_time = CST_CF_WAIT_TIME_10_SEC;
	} else if (wait_time_idx == CST_CF_WAIT_TIME_IDX_15_SEC) {
		wait_time = CST_CF_WAIT_TIME_15_SEC;
	} else if (wait_time_idx == CST_CF_WAIT_TIME_IDX_20_SEC) {
		wait_time = CST_CF_WAIT_TIME_20_SEC;
	}  else if (wait_time_idx == CST_CF_WAIT_TIME_IDX_25_SEC) {
		wait_time = CST_CF_WAIT_TIME_25_SEC;
	}  else {
		wait_time = CST_CF_WAIT_TIME_30_SEC;
	}

	return wait_time;
}

static void __cst_cf_ime_gl_sel_waiting_time(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	curr_wait_time = __cst_cf_ime_get_wait_time((long)list_cf_ime_dep2_waiting_time[item_data->index].func);

	if (waitingtime_radio_btn) {
		elm_radio_value_set(waitingtime_radio_btn,
							(long)list_cf_ime_dep2_waiting_time[item_data->index].func);
		elm_genlist_item_update(item_data->parent_gl_item);
	}
	evas_object_del(waiting_time_popup);
	waiting_time_popup = NULL;
	return;
}

#if 0 /*Function not used */

static void __cst_cf_ime_waiting_time_collapse_req(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_cf_ime_waiting_time_collapse_req);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_expanded_set(item, EINA_FALSE);
	elm_genlist_item_subitems_clear(item);
	elm_genlist_item_update(item);
	elm_object_item_signal_emit(item, "elm,state,bottom", "");
	ugd->b_expanded = EINA_FALSE;
	ugd->gl_expanded_item = NULL;
	elm_object_focus_set(ugd->dg_entry, EINA_TRUE);
	ecore_imf_context_input_panel_show(elm_entry_imf_context_get(ugd->dg_entry));
}

static void __cst_cf_ime_waiting_time_expand_req(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_cf_ime_waiting_time_expand_req);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Elm_Object_Item *item = event_info;
	elm_genlist_item_expanded_set(item, EINA_TRUE);
	ugd->b_expanded = EINA_TRUE;
	ugd->gl_expanded_item = item;
}
#endif

static void __cst_waiting_time_popup_block_clicked_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
	evas_object_del(waiting_time_popup);
	waiting_time_popup = NULL;
}

static void __cst_cf_ime_waiting_time_expand(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_cf_ime_waiting_time_expand);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	ret_if(NULL == it);
	CstGlItemData_t *sub_item_data;
	Evas_Object *genlist = NULL;
	Evas_Object *box = NULL;
	int i = 0;
	Ecore_IMF_Context *imf_ctx;

	/* Hide the IMF if exists */
	imf_ctx = elm_entry_imf_context_get(ugd->dg_entry);
	if (imf_ctx) {
		ecore_imf_context_input_panel_hide(imf_ctx);
	}
	waiting_time_popup = elm_popup_add(ugd->nf);
	cst_util_domain_translatable_part_text_set(waiting_time_popup, "title,text", I_(CST_STR_WAITING_TIME));
	eext_object_event_callback_add(waiting_time_popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(waiting_time_popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(waiting_time_popup, "block,clicked", __cst_waiting_time_popup_block_clicked_cb, ugd->nf);
	/* box */
	box = elm_box_add(waiting_time_popup);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	/* genlist */
	genlist = elm_genlist_add(box);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	/* radio */

	waitingtime_radio_btn = elm_radio_add(genlist);
	elm_radio_value_set(waitingtime_radio_btn, -1);
	for (i = 0; list_cf_ime_dep2_waiting_time[i].style != CST_GL_ITEM_NONE; ++i) {
		sub_item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		ret_if(sub_item_data == NULL);
		sub_item_data->index = i;
		sub_item_data->ugd = ugd;
		sub_item_data->parent_gl_item = it;
		sub_item_data->gl_item = elm_genlist_item_append(genlist, itc_waiting_time_expand,
								(const void *)sub_item_data, NULL,
								list_cf_ime_dep2_waiting_time[i].flags,
								__cst_cf_ime_gl_sel_waiting_time, sub_item_data);
	}

	elm_box_pack_end(box, genlist);
	evas_object_show(genlist);
	evas_object_size_hint_min_set(box, -1, 592);
	elm_object_content_set(waiting_time_popup, box);
	evas_object_show(box);
	evas_object_show(waiting_time_popup);

}

static void __cst_cf_ime_on_click_waitingtime_expand(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_cf_ime_on_click_waitingtime_expand);
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
		__cst_cf_ime_waiting_time_expand(data, obj, item_data->gl_item);
}

static void __cst_cf_ime_gl_del(void *data, Evas_Object *obj)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

static Evas_Object *__cst_cf_ime_create_wait_time_radio_icon(Evas_Object *obj, Evas_Object *rdg, int value, int offset)
{
	retv_if(NULL == obj, NULL);

	int rd_value = 0;
	Evas_Object *radio = elm_radio_add(obj);
	elm_radio_state_value_set(radio, value);
	elm_radio_group_add(radio, rdg);

	rd_value = __cst_cf_ime_get_wait_time_index(curr_wait_time);
	elm_radio_value_set(rdg, (rd_value + offset));

	evas_object_propagate_events_set(radio, EINA_FALSE);

	return radio;
}

static Evas_Object *__cst_cf_ime_get_wait_time_sub_exp_icon(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (!strcmp(part, "elm.icon.right")) {
		Evas_Object *radio = NULL;
		if (NULL == waitingtime_radio_btn) {
			waitingtime_radio_btn = elm_radio_add(obj);
			elm_radio_value_set(waitingtime_radio_btn, -1);
		}
		radio = __cst_cf_ime_create_wait_time_radio_icon(obj, waitingtime_radio_btn,
				(long)list_cf_ime_dep2_waiting_time[item_data->index].func, 0);

		evas_object_smart_callback_add(radio, "changed",
				__cst_cf_ime_gl_sel_waiting_time, item_data);
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

static char *__cst_cf_ime_get_wait_time_sub_exp_label(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	char *text = NULL;
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (strcmp(part, "elm.text.main.left") == 0) {
		int time = list_cf_ime_dep2_waiting_time[item_data->index].str_id;
		text = g_strdup_printf(T_(CST_STR_WAITING_TIME_PERCENT_S_SECONDS), time);
		return text;
	}
	return NULL;
}

static char *__cst_cf_ime_cf_onoff_get_waiting_time_label(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	char *text = NULL;
	char *ret;
	if (strcmp(part, "elm.text.main.left.top") == 0) {
		return strdup(T_(CST_STR_WAITING_TIME));
	} else if (strcmp(part, "elm.text.sub.left.bottom") == 0) {
		int cf_wait_time_index = __cst_cf_ime_get_wait_time_index(curr_wait_time);
		if (list_cf_ime_dep2_waiting_time[cf_wait_time_index].str_id != -1) {
			text = g_strdup_printf(T_(CST_STR_WAITING_TIME_PERCENT_S_SECONDS), list_cf_ime_dep2_waiting_time[cf_wait_time_index].str_id);
			ret = g_strdup_printf("<color=#%02x%02x%02x%02x>%s</color>", 61, 185, 204, 255, text);
			g_free(text);
			return ret;
		}
	}
	return NULL;
}

static void __cst_cf_ime_gl_realized(void *data, Evas_Object *obj, void *ei)
{
	ENTER(__cst_cf_ime_gl_realized);
	CstGlItemData_t *item_data = elm_object_item_data_get(ei);
	ret_if(NULL == item_data);
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;

	DBG("Idx = %d", item_data->index);
	if (item_data->index == CST_GENLIST_ENTRY_ITEM) {
		elm_object_item_signal_emit(ei, "elm,state,edit,enabled", "");
		elm_object_focus_set(ugd->dg_entry, EINA_TRUE);
		elm_entry_cursor_end_set(ugd->dg_entry);
#ifdef _CALL_SET_TTS_SUPPORT
		/* ===== Accessibility ==== */
		if (elm_config_access_get()) {
			Eina_List *access_list = NULL;
			Evas_Object *entry = NULL;
			Evas_Object *edit_icon = NULL;

			elm_object_item_access_unregister(ei);
			elm_object_item_access_order_unset(ei);
			entry = elm_object_item_part_content_get(ei, "elm.icon.entry");
			edit_icon = elm_object_item_part_content_get(ei, "elm.icon.edit");

			elm_access_info_cb_set(entry, ELM_ACCESS_INFO, _cst_util_entry_number_access_info_cb, entry);
			if (entry)
				access_list = eina_list_append(access_list, entry);
			if (edit_icon)
				access_list = eina_list_append(access_list, edit_icon);
			if (access_list)
				elm_object_item_access_order_set(ei, access_list);
		}
#endif
	}
}

static Evas_Object *__cst_create_cf_ime_genlist(CstUgData_t *ugd, int cf_flavour)
{
	ENTER(__cst_create_cf_ime_genlist);
	Evas_Object *genlist;
	CstGlItemData_t *item_data = NULL;
	Elm_Object_Item *item = NULL;

	genlist = elm_genlist_add(ugd->nf);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	_cst_create_genlist_separator(genlist, EINA_FALSE);

	memset(ugd->entry_string, 0, sizeof(ugd->entry_string));

	evas_object_smart_callback_add(genlist, "realized", __cst_cf_ime_gl_realized, ugd);

	ugd->genlist_editfield_initialized = 0;

	if (!itc_ime) {
		itc_ime = _cst_create_genlist_item_class("entry.icon",
				NULL, __cst_gl_icon_get_cf_ime,
				NULL, __cst_cf_ime_gl_del);
	}
	if (!itc_sep) {
		itc_sep = _cst_create_genlist_item_class("dialogue/separator.2",
				NULL, NULL, NULL, NULL);
	}
	if (cf_flavour == CST_SSTYPE_CF_NO_REPLY) {
		if (!itc_waiting_time) {
			itc_waiting_time = _cst_create_genlist_item_class("2line.top",
					__cst_cf_ime_cf_onoff_get_waiting_time_label,
					NULL, NULL, NULL);
		}
		if (!itc_waiting_time_expand) {
			itc_waiting_time_expand = _cst_create_genlist_item_class("1line",
					__cst_cf_ime_get_wait_time_sub_exp_label,
					__cst_cf_ime_get_wait_time_sub_exp_icon,
					NULL, __cst_cf_ime_gl_del);
		}
		waitingtime_item = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(waitingtime_item == NULL, NULL);
		waitingtime_item->ugd = ugd;
		waitingtime_item->gl_item = NULL;
	}

	item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
	retv_if(NULL == item_data, NULL);
	item_data->ugd = ugd;
	item_data->index = CST_GENLIST_ENTRY_ITEM;
	item = elm_genlist_item_append(genlist, itc_ime,
					   (void *)item_data, NULL, ELM_GENLIST_ITEM_NONE,
					   NULL, (void *)item_data);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	item_data->gl_item = item;

#ifdef _CALL_SET_OLD_SEPERATOR
	elm_genlist_item_append(genlist, itc_sep,
					   NULL, NULL, ELM_GENLIST_ITEM_NONE,
					   NULL, NULL);
#endif

	if (cf_flavour == CST_SSTYPE_CF_NO_REPLY) {
#ifdef _CALL_SET_OLD_SEPERATOR
		elm_genlist_item_append(genlist, itc_sep,
						   NULL, NULL, ELM_GENLIST_ITEM_NONE,
						   NULL, NULL);
#endif
		waitingtime_item->gl_item = elm_genlist_item_append(genlist, itc_waiting_time,
									(const void *)waitingtime_item, NULL, ELM_GENLIST_ITEM_TREE,
									__cst_cf_ime_on_click_waitingtime_expand, waitingtime_item);
		evas_object_smart_callback_add(genlist, "realized",
						_cst_gl_realized_cb, (const void *)CST_DIALOG_GROUP_CALL_FORWARDING_WAITING_TIME);
	} else {
		_cst_create_genlist_separator(genlist, EINA_FALSE);
		evas_object_smart_callback_add(genlist, "realized",
						_cst_gl_realized_cb, (const void *)CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST);
	}

	return genlist;
}

int _cst_cf_ime_fetch_latest_wait_time(void)
{
	ENTER(_cst_cf_ime_fetch_latest_wait_time);

	return curr_wait_time;
}

static Eina_Bool __cst_cf_is_waiting_time_available(int wait_time)
{
	Eina_Bool ret = EINA_FALSE;

	switch (wait_time) {
	case CST_CF_WAIT_TIME_5_SEC:
	case CST_CF_WAIT_TIME_10_SEC:
	case CST_CF_WAIT_TIME_15_SEC:
	case CST_CF_WAIT_TIME_20_SEC:
	case CST_CF_WAIT_TIME_25_SEC:
	case CST_CF_WAIT_TIME_30_SEC:
		ret = EINA_TRUE;
		break;

	case CST_CF_WAIT_TIME_NONE:
	default:
		ret = EINA_FALSE;
		break;
	}

	return ret;
}

Evas_Object *_cst_create_cf_ime(Evas_Object *parent, char *edit_string,
								int wait_time, int cf_flavour, void *data)
{
	ENTER(_cst_create_cf_ime);
	retv_if(data == NULL, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Eina_Bool ret = EINA_FALSE;

	if ((edit_string != NULL) && (strlen(edit_string) > 0)) {
		def_ime_number = edit_string;
	} else {
		def_ime_number = NULL;
	}

	ret = __cst_cf_is_waiting_time_available(wait_time);
	if (ret == EINA_TRUE) {
		curr_wait_time = wait_time;
	} else {
		/* set default as 30 seconds */
		curr_wait_time = CST_CF_WAIT_TIME_30_SEC;
	}

	Evas_Object *genlist = __cst_create_cf_ime_genlist(ugd, cf_flavour);
	return genlist;
}
