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
#include <vconf.h>
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

static Elm_Genlist_Item_Class *itc_1text = NULL;
static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_title = NULL;
static Elm_Genlist_Item_Class *itc_help = NULL;
Elm_Genlist_Item_Class *itc_2text = NULL;
static Elm_Genlist_Item_Class *itc_2text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_multiline = NULL;

static CstGlItemDisplayInfo_t list_dep1[] = {
	{1, CST_STR_REJECT_MESSAGES, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_EXPANDABLE, _cst_on_click_reject_message},
	{1, CST_STR_CALL_ANSWERING_ENDING, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, _cst_on_click_answering_call},
	{1, CST_STR_MORE_CALL_SETTINGS, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_TEXT, _cst_on_click_more_call_setting},
	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

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
	CstAppData_t *ad = (CstAppData_t *)item_data->ad;
	ret_if(NULL == ad);
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (list_dep1[item_data->index].func) {
		if (list_dep1[item_data->index].style == CST_GL_ITEM_1TEXT_ONOFF) {
			list_dep1[item_data->index].func((void *)item_data, NULL, obj, event_info);
		} else {
			list_dep1[item_data->index].func((void *)ad, NULL, obj, event_info);
		}
	}
	return;
}

static Evas_Object *__cst_gl_icon_get_phone(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (!strcmp(part, "elm.icon.right") || !strcmp(part, "elm.icon")) {
		return item_data->eo_check;
	}
	return NULL;
}

static Eina_Bool __cst_on_clicked_back_btn(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_clicked_back_btn);
	retv_if(NULL == data, EINA_TRUE);

	ui_app_exit();

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
	CstAppData_t *ad = (CstAppData_t *)data;
	CstGlItemData_t *item_data;
	Evas_Object *genlist;
	int i;

	genlist = elm_genlist_add(ad->nf);
	retv_if(NULL == genlist, NULL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	for (i = 0; list_dep1[i].style != CST_GL_ITEM_NONE; ++i) {
		item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(NULL == item_data, NULL);
		DBG("list_dep1[i].str_id = %d", list_dep1[i].str_id);
 		item_data->index = i;
		item_data->ad = ad;
		item_data->style = list_dep1[i].style;
		if (list_dep1[i].style == CST_GL_ITEM_TEXT) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text,
								 (void *)item_data, NULL, list_dep1[i].flags,
								 __cst_gl_sel_phone, item_data);
		} else if (list_dep1[i].style == CST_GL_ITEM_EXPANDABLE) {
					item_data->gl_item = elm_genlist_item_append(genlist, itc_multiline,
								(void *)item_data, NULL, list_dep1[i].flags,
								__cst_gl_sel_phone, item_data);
		} else if (list_dep1[i].style == CST_GL_ITEM_1TEXT_ONOFF) {
			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon,
							 (void *)item_data, NULL, list_dep1[i].flags,
							 __cst_gl_sel_phone, item_data);
		} else {
			DBG("No style");
			free(item_data);
			item_data = NULL;
			return NULL;
		}
	}

	return genlist;
}

void _cst_create_call_setting(void *data)
{
	ENTER(_cst_create_call_setting);
	ret_if(NULL == data);
	CstAppData_t *ad = (CstAppData_t *)data;
	int title = CST_STR_CALL_SETTINGS;
	Elm_Object_Item *navi_it = NULL;

	__cst_set_genlist_item_styles_phone();
	Evas_Object *genlist = __cst_create_genlist_phone(ad);

	navi_it = elm_naviframe_item_push(ad->nf, I_(title), NULL, NULL, genlist, NULL);
	cst_util_item_domain_text_translatable_set(navi_it, I_(title));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_clicked_back_btn, ad);

	LEAVE();
}
