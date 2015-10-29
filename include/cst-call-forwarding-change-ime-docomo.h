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

#ifndef _CST_CALL_FORWARDING_CHANGE_IME_DOCOMO_H_
#define _CST_CALL_FORWARDING_CHANGE_IME_DOCOMO_H_

#include <Elementary.h>
Evas_Object *_cst_create_cf_change_ime_docomo(void *data, char *number);
void _cst_cf_change_ime_destroy_genlist_item_class_docomo(void);
void _cst_call_forwarding_change_ime_number_entry_changed_docomo_cb(void *data,
		Evas_Object *obj, void *event_info);
void _cst_call_forwarding_change_ime_input_panel_event_docomo_callback(void *data,
		Ecore_IMF_Context *imf_context, int value);
#endif

