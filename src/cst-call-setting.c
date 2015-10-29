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
#include <app_manager.h>

#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-call-setting.h"
#include "cst-widget.h"
#include "cst-tapi-request.h"
#include "cst-answering.h"
#include "cst-reject-msg.h"
#include "cst-util.h"
#include "cst-more-call-setting.h"
#include <ui-gadget-module.h>
#include <Elementary.h>
#include <vconf.h>
#include <app.h>

#define CST_RINGTONE_FILE_EXT ".ogg"
#define CST_MEDIA_FILE_EXT_LEN 4

static Elm_Genlist_Item_Class *itc_1text = NULL;
static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_title = NULL;
static Elm_Genlist_Item_Class *itc_help = NULL;
Elm_Genlist_Item_Class *itc_2text = NULL;
static Elm_Genlist_Item_Class *itc_2text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_multiline = NULL;

#if 0	/* not used */
static void __cst_on_click_speed_dial(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_on_click_extra_vol(void *data, Evas *evas, Evas_Object *obj, void *event_info);
#endif

static CstGlItemDisplayInfo_t list_dep1[] = {
	{1, CST_STR_REJECT_MESSAGES, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_EXPANDABLE, _cst_on_click_reject_message},
	{1, CST_STR_CALL_ANSWERING_ENDING, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, _cst_on_click_answering_call},
#ifdef _CALL_SET_VIDEO_CALL_IMAGE_SUPPORT
/*	{1, CST_STR_VIDEO_CALL_IMAGE, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_IMG_1TEXT, _cst_on_click_video_call_img},*/
	{1, CST_STR_SET_DEFAULT_HIDE_ME_IMAGE, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_HELP_TEXT, NULL},
#endif

#ifdef _CALL_SET_WITH_HELPTEXT
	{1, CST_STR_PROXIMITY_SENSOR_HELP_MSG, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_HELP_TEXT, NULL},
#endif
	{1, CST_STR_MORE_CALL_SETTINGS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, _cst_on_click_more_call_setting},
	/* {1, CST_STR_MODIFY_MY_EYES_TO_SEEM_LIKE_I_AM_LOOKING_AT_CAMERA_DURING_VIDEO_CALLS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_HELP_TEXT, NULL}, */
	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static void __cst_call_setting_gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *nxt_item = NULL;
	Elm_Object_Item *last_item = NULL;
	CstGlItemData_t *item_data = NULL;
	int i;
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	item_data = elm_object_item_data_get(it);
#ifdef _CALL_SET_TTS_SUPPORT
	if (NULL != item_data && list_dep1[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF) {
		elm_access_info_cb_set(elm_object_item_access_object_get(it),
				ELM_ACCESS_INFO, _cst_util_on_off_btn_access_info_cb, it);
	}
#endif
	nxt_item = elm_genlist_first_item_get(obj);

	while (nxt_item != elm_genlist_last_item_get(obj)) {
		item_data = (CstGlItemData_t *)elm_object_item_data_get(nxt_item);
		while (item_data == NULL || item_data->index == -1 ||
			item_data->style == CST_GL_ITEM_HELP_TEXT || item_data->style == CST_GL_ITEM_DG_TITLE) {
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

#if 0	/* unused function */
static void __cst_speed_dial_view_ug_destroy_cb(ui_gadget_h ug, void *priv)
{
	ENTER(__cst_speed_dial_view_ug_destroy_cb);
	if (ug) {
		ug_destroy(ug);
	}
}

static void __cst_speed_dial_view_ug_result_cb(ui_gadget_h ug, app_control_h service, void *priv)
{
	ENTER(__cst_speed_dial_view_ug_result_cb);
}

static void __cst_speed_dial_view_ug_layout_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
	ENTER(__cst_speed_dial_view_ug_layout_cb);
	Evas_Object *base = ug_get_layout(ug);
	ret_if(base == NULL);

	evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(base);
}

static void __cst_on_click_speed_dial(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_speed_dial);
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	struct ug_cbs cbs = { 0, };
	ui_gadget_h ug;

	cbs.layout_cb = __cst_speed_dial_view_ug_layout_cb;
	cbs.result_cb = __cst_speed_dial_view_ug_result_cb;
	cbs.destroy_cb = __cst_speed_dial_view_ug_destroy_cb;
	cbs.priv = (void *)ugd;

	ug = ug_create((ui_gadget_h)ugd->ug, "speeddial-efl", UG_MODE_FULLVIEW, NULL, &cbs);
	DBG("ug = 0x%x", ug);
	if (ug == NULL) {
		DBG("ug create fail...");
	}
}
#endif
static char *__cst_gl_label_get_phone(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if ((strcmp(part, "elm.text") == 0) || (strcmp(part, "elm.text.main.left.top") == 0) ||
			(strcmp(part, "elm.text.main.left") == 0) || (strcmp(part, "elm.text.main") == 0)) {
		if (list_dep1[item_data->index].str_id != -1) {
			return strdup(T_(list_dep1[item_data->index].str_id));
		}
	} else if ((strcmp(part, "elm.text.sub.left.bottom") == 0) || (strcmp(part, "elm.text.multiline") == 0)) {
		if (list_dep1[item_data->index].str_id == CST_STR_REJECT_MESSAGES) {
			return strdup(T_(CST_STR_REJECT_MSG_HELP_TEXT));
		} else if (list_dep1[item_data->index].str_id == CST_STR_NOTIFY_DURING_CALL) {
			return strdup(T_(CST_STR_NOTIFICATIONS_DURING_CALL));
		} else if (list_dep1[item_data->index].str_id == CST_STR_VIDEO_CALL_IMAGE) {
			return strdup(T_(CST_STR_ALTERNATE_IMAGE_FOR_VIDEO_CALL));
		}
	}
	return NULL;
}

static void __cst_gl_del_phone(void *data, Evas_Object *obj)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

static void __cst_gl_sel_phone(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(NULL == ugd);
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (list_dep1[item_data->index].func) {
		if (list_dep1[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF) {
			list_dep1[item_data->index].func((void *)item_data, NULL, obj, event_info);
		} else {
			list_dep1[item_data->index].func((void *)ugd, NULL, obj, event_info);
		}
	}
	return;
}

static Evas_Object *__cst_gl_icon_get_phone(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (!strcmp(part, "elm.icon.right") || !strcmp(part, "elm.icon")) {
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

static Eina_Bool __cst_on_clicked_back_btn(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_clicked_back_btn);
	retv_if(NULL == data, EINA_TRUE);

	CstUgData_t *ugd = (CstUgData_t *)data;

	ug_destroy_me((ui_gadget_h)ugd->ug);

	return EINA_FALSE;
}

static void __cst_set_genlist_item_styles_phone(void)
{
	ENTER(__cst_set_genlist_item_styles_phone);
	if (!itc_1text) {
		itc_1text = _cst_create_genlist_item_class("type1",
				__cst_gl_label_get_phone, NULL, NULL,
				__cst_gl_del_phone);
	}
	if (!itc_1text_1icon) {
		itc_1text_1icon = _cst_create_genlist_item_class("1line",
				__cst_gl_label_get_phone, __cst_gl_icon_get_phone,
				NULL, __cst_gl_del_phone);
	}
	if (!itc_title) {
		itc_title = _cst_create_genlist_item_class("groupindex",
				__cst_gl_label_get_phone, NULL, NULL,
				__cst_gl_del_phone);
	}
	if (!itc_help) {
		itc_help = _cst_create_genlist_item_class("multiline",
				__cst_gl_label_get_phone, NULL, NULL,
				__cst_gl_del_phone);
	}
	if (!itc_2text) {
		itc_2text = _cst_create_genlist_item_class("2line.top",
				__cst_gl_label_get_phone, NULL, NULL,
				__cst_gl_del_phone);
	}
	if (!itc_2text_1icon) {
		itc_2text_1icon = _cst_create_genlist_item_class("multiline_sub.main.1icon",
				__cst_gl_label_get_phone, __cst_gl_icon_get_phone,
				 NULL, __cst_gl_del_phone);
	}
	if (!itc_multiline) {
		itc_multiline = _cst_create_genlist_item_class("multiline",
				__cst_gl_label_get_phone, NULL,
				 NULL, __cst_gl_del_phone);
	}
}

static Evas_Object *__cst_create_genlist_phone(void *data)
{
	ENTER(__cst_create_genlist_phone);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	CstGlItemData_t *item_data;
	Evas_Object *genlist;
	int i;

	genlist = elm_genlist_add(ugd->nf);
	retv_if(NULL == genlist, NULL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

#if 0		/* Disable this code to remove the seprator as per Tizen-Lite GUI*/
	_cst_create_genlist_separator(genlist, EINA_FALSE);
#endif

	for (i = 0; list_dep1[i].style != CST_GL_ITEM_NONE; ++i) {
		item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(NULL == item_data, NULL);
		DBG("list_dep1[i].str_id = %d", list_dep1[i].str_id);
 		item_data->index = i;
		item_data->ugd = ugd;
		item_data->style = list_dep1[i].style;
		if (list_dep1[i].style == CST_GL_ITEM_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text,
								 (void *)item_data, NULL, list_dep1[i].flags,
								 __cst_gl_sel_phone, item_data);
		} else if (list_dep1[i].style == CST_GL_ITEM_EXPANDABLE) {
					item_data->gl_item = elm_genlist_item_append(genlist, itc_multiline,
								(void *)item_data, NULL, list_dep1[i].flags,
								__cst_gl_sel_phone, item_data);
		} else if (list_dep1[i].style == CST_GL_ITEM_IMG_1TEXT) {
			if (list_dep1[i].str_id == CST_STR_VIDEO_CALL_IMAGE) {
				item_data->gl_item = elm_genlist_item_append(genlist, itc_multiline,
								 (void *)item_data, NULL, list_dep1[i].flags,
								 __cst_gl_sel_phone, item_data);
			} else {
				item_data->gl_item = elm_genlist_item_append(genlist, itc_1text,
								 (void *)item_data, NULL, list_dep1[i].flags,
								 __cst_gl_sel_phone, item_data);
			}
		} else if (list_dep1[i].style == CST_GL_ITEM_1TEXT_ONOFF) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon,
							 (void *)item_data, NULL, list_dep1[i].flags,
							 __cst_gl_sel_phone, item_data);
		} else if (list_dep1[i].style == CST_GL_ITEM_DG_TITLE) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_title,
								 (void *)item_data, NULL, list_dep1[i].flags,
								 NULL, NULL);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		} else if (list_dep1[i].style == CST_GL_ITEM_HELP_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_help,
								 (void *)item_data, NULL, list_dep1[i].flags,
								 NULL, NULL);
			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			if (list_dep1[i+1].style != CST_GL_ITEM_DG_TITLE) {
				_cst_create_genlist_separator(genlist, EINA_FALSE);
			}
		} else if (list_dep1[i].style == CST_GL_ITEM_SUB_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_2text,
							 (void *)item_data, NULL, list_dep1[i].flags,
							 __cst_gl_sel_phone, item_data);

			ugd->vm_gl_item = item_data->gl_item;
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

void _cst_create_call_setting(void *data)
{
	ENTER(_cst_create_call_setting);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int title = CST_STR_CALL_SETTINGS;
	Elm_Object_Item *navi_it = NULL;

	__cst_set_genlist_item_styles_phone();
	Evas_Object *genlist = __cst_create_genlist_phone(ugd);

	evas_object_smart_callback_add(genlist, "realized",
				__cst_call_setting_gl_realized_cb, NULL);

	navi_it = elm_naviframe_item_push(ugd->nf, I_(title), NULL, NULL, genlist, NULL);
	cst_util_item_domain_text_translatable_set(navi_it, I_(title));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_clicked_back_btn, ugd);

	LEAVE();
}
