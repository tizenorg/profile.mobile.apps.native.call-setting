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

#ifndef _CST_DELETE_LIST_H_
#define _CST_DELETE_LIST_H_

#include <Elementary.h>

void _cst_create_delete_list(void *data, Evas_Object *obj, void *event_info);
void _cst_update_delete_list(void *data);
void _cst_destroy_delete_list(void);
void _cst_FDN_delete_list_input_panel_event_cb(void *data, Ecore_IMF_Context *imf_context, int value);
void _cst_FDN_delete_list_changed_editfield_cb(void *data, Evas_Object *obj, void *event_info);
Eina_Bool _cst_click_FDN_delete_list_ime_back_cb(void *data,
		Elm_Object_Item *it);

#endif	/* _CST_DELETE_LIST_H_ */

