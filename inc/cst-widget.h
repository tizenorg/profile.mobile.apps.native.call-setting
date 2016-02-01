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

#ifndef _CST_WIDGET_H_
#define _CST_WIDGET_H_

#include <Elementary.h>
#include <efl_extension.h>
#include "cst-common.h"

#define BOTTOM_PADDING_CONTENT_SIGNAL "elm,layout,content,bottom_padding"
#define DEFAULT_CONTENT_SIGNAL "elm,layout,content,default"
#define BOTTOM_PADDING_CONTENT_SOURCE "layout"

char *_cst_get_error_string(int error);
Evas_Object *_cst_show_error_popup_destroy_ug(CstUgData_t *ugd, int error);
void _cst_create_error_popup(int error_code);
Evas_Object *_cst_create_error_popup_with_ok_btn(CstUgData_t *ugd, int error);
Evas_Object *_cst_create_duplicate_msg_popup(Evas_Object *parent, const char *string);
Evas_Object *_cst_create_2btn_popup(Evas_Object *parent, void *data,
		const char *info_text, Evas_Smart_Cb cb, void *cb_data);
Evas_Object *_cst_create_1btn_popup(Evas_Object *parent, void *data,
		const char *info_text, const char *title_text, Evas_Smart_Cb cb,
		void *cb_data);
void _cst_create_navi_control_bar(Evas_Object *parent,
		char *label1, const char *icon1, Evas_Smart_Cb cb1,
		char *label2, const char *icon2, Evas_Smart_Cb cb2,
		void *data, Elm_Object_Item *navi_it,
		Evas_Object **c_item);
void _cst_create_genlist_separator(Evas_Object *genlist, Eina_Bool need_item_data);
Evas_Object *_cst_create_processing_popup(Evas_Object *parent);
Evas_Object *_cst_create_progressbar(Evas_Object *parent);
Evas_Object* _cst_create_scroller(Evas_Object* parent);
Evas_Object *_cst_create_box(Evas_Object* parent);
void _cst_create_entry_margin(Evas_Object* parent);
void _cst_create_entry(Evas_Object* parent, Evas_Object* (*p_func)(), void *data, char *dialog_group_style);
Evas_Object *_cst_get_navifr_prev_btn(Evas_Object *nf);
void _cst_create_indicator_popup(const char *text);
Evas_Object *_cst_create_onoff_button(Evas_Object *obj,
									  int state_value, Evas_Smart_Cb cb_func, void *cb_data);
Evas_Object *_cst_create_radio_icon(Evas_Object *obj,
									Evas_Object *rdg, int value, int offset, const char *vconf_key);
Evas_Object *_cst_create_title_btn_icon_only(Evas_Object *parent,
								   const char *text, Evas_Smart_Cb func, void *data);
void _cst_remove_naviframe(Evas_Object *nf);
void _cst_transition_cb(void *data, Evas_Object *obj, void *event_info);
Evas_Object *_cst_create_ime_editfield(CstUgData_t *ugd, Evas_Object *parent,
		CstImeType_t ime_type, char *input_string);
Elm_Genlist_Item_Class *_cst_create_genlist_item_class(
	const char *item_style,
	Elm_Gen_Item_Text_Get_Cb text_get_fn,
	Elm_Gen_Item_Content_Get_Cb content_get_fn,
	Elm_Gen_Item_State_Get_Cb state_get_fn, Elm_Gen_Item_Del_Cb del_fn);
void _cst_update_genlist_item_class(Elm_Object_Item *item, Elm_Genlist_Item_Class *itc);
void _cst_destroy_genlist_item_class(Elm_Genlist_Item_Class *itc);
void _cst_terminate_service(app_control_h service);
void _cst_destroy_all_items(CstUgData_t *ugd);
void _cst_gl_realized_cb(void *data, Evas_Object *obj, void *event_info);
Evas_Object *_cst_create_ime_contacts_btn_obj(Evas_Object *parent, void *data);
void _cst_create_editfield_dialogue_entry(Evas_Object* parent,
		Evas_Object* (*p_func)(), void *data, char *dialog_group_style);
void _cst_create_2btn_navi_toolbar(Evas_Object *parent, char *label1,
		const char *icon1, Evas_Smart_Cb cb1, char *label2, const char *icon2,
		Evas_Smart_Cb cb2, void *data, Elm_Object_Item *navi_it,
		Elm_Object_Item **c_obj_item);
Evas_Object *_cst_create_2btn_delete_popup(Evas_Object *parent, void *data,
		const char *info_text, Evas_Smart_Cb cb, void *cb_data);
Evas_Object *_cst_create_2btn_ok_cancel_popup(Evas_Object *parent,
		CstGlItemData_t *item_data, int btn1_str_id, int btn2_str_id,
		const char *info_text, Evas_Smart_Cb btn1_func, Evas_Smart_Cb btn2_func);
Evas_Object *_cst_create_2btn_check_popup(Evas_Object *parent, void *data,
		const char *info_text, const char *popup_title, Evas_Smart_Cb cb, Evas_Smart_Cb cancl_cb, void *cb_data, Evas_Smart_Cb check_callback_fn);
Eina_Bool _cst_popup_is_check_set(void *data);
void _cst_create_toast_popup(const char *text);
#ifdef _TIZEN_LITE_CODE
void _cst_create_dual_sim_tabbar(Elm_Object_Item *navi_it, Evas_Smart_Cb
		sim1_func, Evas_Smart_Cb sim2_func, void *data);
#endif /* _TIZEN_LITE_CODE */

#endif	/* _CALL_SETTING_WIDGET_H_ */

