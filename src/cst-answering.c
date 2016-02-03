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
#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-call-setting.h"
#include "cst-answering.h"
#include "cst-widget.h"
#include "cst-util.h"

static void __cst_on_click_onoff(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_on_click_power_key_ends_call(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void __cst_set_genlist_item_styles(void);
static void __cst_destroy_genlist_item_styles(void);
static Eina_Bool __cst_answering_call_back_btn_clicked_cb(void *data, Elm_Object_Item *it);
static Elm_Genlist_Item_Class *itc_1text_1icon = NULL;
static Elm_Genlist_Item_Class *itc_group_index = NULL;
static Elm_Genlist_Item_Class *itc_help = NULL;
static Evas_Object *answering_gl = NULL;


static CstGlItemDisplayInfo_t list_call_answering[] = {
	{1, CST_STR_ANSWER_CALLS_BY, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_GROUP_INDEX, NULL},
	{1, CST_STR_PRESSING_THE_HOME_KEY, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF, __cst_on_click_onoff},
	{1, CST_STR_END_CALLS_BY, ELM_GENLIST_ITEM_TREE, CST_GL_ITEM_GROUP_INDEX, NULL},
	{1, CST_STR_PRESSING_THE_POWER_KEY, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_1TEXT_ONOFF, __cst_on_click_power_key_ends_call},
	{1, -1, ELM_GENLIST_ITEM_NONE, CST_GL_ITEM_NONE, NULL},
};

static void __cst_on_click_onoff(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	Eina_Bool check_state = elm_check_state_get(item_data->eo_check);
	if (CST_STR_PRESSING_THE_HOME_KEY == list_call_answering[item_data->index].str_id) {
		_cst_vconf_set_bool(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, !check_state);
	}
}

static void __cst_on_click_power_key_ends_call(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	DBG("__cst_on_click_power_key_ends_call");
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	Eina_Bool check_state = elm_check_state_get(item_data->eo_check);

	_cst_vconf_set_bool(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, !check_state);
}

static void __cst_on_changed_answering_key(void *data, Evas_Object *obj, void *event_info)
{
	Eina_Bool check_state;
	check_state = elm_check_state_get(obj);

	_cst_vconf_set_bool(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, check_state);
}

static void __cst_on_changed_power_key_ends_call(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	Eina_Bool check_state = elm_check_state_get(obj);

	_cst_vconf_set_bool(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, check_state);
	elm_genlist_item_update(item_data->gl_item);
}

static char *__cst_gl_answering_ending_label_get(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (!strcmp(part, "elm.text")) {
		if (list_call_answering[item_data->index].str_id != -1) {
			return strdup(T_(list_call_answering[item_data->index].str_id));
		}
	}
	return NULL;
}

static Evas_Object *__cst_gl_answering_ending_icon_get(void *data, Evas_Object *obj, const char *part)
{
	retv_if(NULL == data, NULL);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;

	if (!strcmp(part, "elm.swallow.end")) {
		int value = 0;
		if (CST_STR_PRESSING_THE_HOME_KEY == list_call_answering[item_data->index].str_id) {
			_cst_vconf_get_bool(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, &value);
			item_data->eo_check = _cst_create_onoff_button(obj, value,
								  __cst_on_changed_answering_key, item_data);
		} else if (CST_STR_PRESSING_THE_POWER_KEY == list_call_answering[item_data->index].str_id) {
			_cst_vconf_get_bool(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, &value);
			item_data->eo_check = _cst_create_onoff_button(obj, value,
								  __cst_on_changed_power_key_ends_call, item_data);
		}

		return item_data->eo_check;
	}
	return NULL;
}

static void __cst_gl_answering_ending_del(void *data, Evas_Object *obj)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		free(item_data);
	}
	return;
}

static void __cst_gl_answering_ending_sel(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (list_call_answering[item_data->index].func) {
		list_call_answering[item_data->index].func((void *)item_data, NULL, obj, event_info);
	}
	return;
}

static void __cst_set_genlist_item_styles(void)
{
	ENTER(__cst_set_genlist_item_styles);
	if (!itc_group_index) {
		itc_group_index = _cst_create_genlist_item_class("group_index",
			__cst_gl_answering_ending_label_get,
			NULL, NULL, __cst_gl_answering_ending_del);
	}

	if (!itc_1text_1icon) {
		itc_1text_1icon = _cst_create_genlist_item_class("type2",
			__cst_gl_answering_ending_label_get,
			__cst_gl_answering_ending_icon_get,
			NULL, __cst_gl_answering_ending_del);
	}

	if (!itc_help) {
		itc_help = _cst_create_genlist_item_class("multiline",
				__cst_gl_answering_ending_label_get,
				NULL, NULL, __cst_gl_answering_ending_del);
	}
}

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_group_index) {
		_cst_destroy_genlist_item_class(itc_group_index);
		itc_group_index = NULL;
	}

	if (itc_1text_1icon) {
		_cst_destroy_genlist_item_class(itc_1text_1icon);
		itc_1text_1icon = NULL;
	}
	if (itc_help) {
		_cst_destroy_genlist_item_class(itc_help);
		itc_help = NULL;
	}
}

static Evas_Object *__cst_create_gl_answering_ending(void *data)
{
	ENTER(__cst_create_gl_answering_ending);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	int index = 0;
	Evas_Object *genlist;
	CstGlItemData_t *item_data;

	/* Create genlist */
	genlist = elm_genlist_add(ugd->nf);

	/* Genlist item append */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	for (index = 0; list_call_answering[index].style != CST_GL_ITEM_NONE; ++index) {

		item_data = (CstGlItemData_t *)calloc(1, sizeof(CstGlItemData_t));
		retv_if(NULL == item_data, NULL);
		item_data->index = index;
		item_data->ugd = ugd;

		if (list_call_answering[index].style == CST_GL_ITEM_GROUP_INDEX) {

			item_data->gl_item = elm_genlist_item_append(genlist, itc_group_index,
					(void *)item_data, NULL, list_call_answering[index].flags,
					NULL, NULL);

			elm_genlist_item_select_mode_set(item_data->gl_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

		} else if (list_call_answering[index].style == CST_GL_ITEM_1TEXT_ONOFF) {

			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text_1icon,
					(void *)item_data, NULL, list_call_answering[index].flags,
					__cst_gl_answering_ending_sel, item_data);

			if (CST_STR_PRESSING_THE_HOME_KEY == list_call_answering[index].str_id) {
				vconf_notify_key_changed(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL,
						_cst_vconfkey_change_notify_cb, item_data->gl_item);
			}

			if (CST_STR_PRESSING_THE_POWER_KEY == list_call_answering[index].str_id) {
				vconf_notify_key_changed(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL,
						_cst_vconfkey_change_notify_cb, item_data->gl_item);
			}

		} else {
			DBG("No style");
			free(item_data);
			item_data = NULL;
			return NULL;
		}
	}

	return genlist;
}

static Eina_Bool __cst_answering_call_back_btn_clicked_cb(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_answering_call_back_btn_clicked_cb);
	CstUgData_t *ugd = (CstUgData_t *)data;
	retv_if(ugd == NULL, EINA_TRUE);

	_cst_destroy_answering_call();

	if (ugd->ug_req_type == CST_UG_REQ_ANSWER_END_CALLS) {
		ug_destroy_me(ugd->ug);
	}

	return EINA_TRUE;
}

static void __cst_create_answering_call(Evas_Object *parent, void *data)
{
	ENTER(__cst_create_answering_call);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Elm_Object_Item *navi_it = NULL;
	Evas_Object *back_btn = NULL;

	__cst_set_genlist_item_styles();
	answering_gl = __cst_create_gl_answering_ending(ugd);

	back_btn = _cst_util_navi_back_btn_create(ugd->nf);

	navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_CALL_ANSWERING_ENDING),
			back_btn, NULL, answering_gl, NULL);
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_CALL_ANSWERING_ENDING));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_answering_call_back_btn_clicked_cb, ugd);

	LEAVE();
}

void _cst_on_click_answering_call(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_answering_call);

	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	__cst_create_answering_call(ugd->nf, ugd);
}

void _cst_destroy_answering_call(void)
{
	ENTER(_cst_destroy_answering_call);

	__cst_destroy_genlist_item_styles();

	if (answering_gl != NULL) {
		answering_gl = NULL;
	}

	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL,
			_cst_vconfkey_change_notify_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL,
			_cst_vconfkey_change_notify_cb);
}

