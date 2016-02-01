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

#ifndef _CST_FORWARDING_H_
#define _CST_FORWARDING_H_

#include <Elementary.h>

enum {
	CST_CF_WAIT_TIME_5_SEC = 5,
	CST_CF_WAIT_TIME_10_SEC = 10,
	CST_CF_WAIT_TIME_15_SEC = 15,
	CST_CF_WAIT_TIME_20_SEC = 20,
	CST_CF_WAIT_TIME_25_SEC = 25,
	CST_CF_WAIT_TIME_30_SEC = 30,

	CST_CF_WAIT_TIME_NONE,
};

enum {
	CST_CF_WAIT_TIME_IDX_5_SEC = 0,
	CST_CF_WAIT_TIME_IDX_10_SEC,
	CST_CF_WAIT_TIME_IDX_15_SEC,
	CST_CF_WAIT_TIME_IDX_20_SEC,
	CST_CF_WAIT_TIME_IDX_25_SEC,
	CST_CF_WAIT_TIME_IDX_30_SEC,

	CST_CF_WAIT_TIME_IDX_NONE,
};

void _cst_on_click_call_forwarding(void *data, Evas *evas, Evas_Object *obj, void *event_info);
void _cst_call_forwarding_input_panel_event_callback(void *data, Ecore_IMF_Context *imf_context, int value);
void _cst_call_forward_entry_changed_cb(void *data, Evas_Object *obj, void *event_info);
void _cst_destroy_call_forwarding(void);
Evas_Object* _cst_create_cf_popup(Evas_Object *parent, void *data,
		const char* info_text, char* editstring, Evas_Smart_Cb cb, void *cb_data);

#endif	/* _CST_FORWARDING_H_ */

