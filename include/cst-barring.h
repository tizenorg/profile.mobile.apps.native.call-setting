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

#ifndef _CST_BARRING_H_
#define _CST_BARRING_H_

#ifdef _CALL_SET_BARRING_SUPPORT

#include <Elementary.h>

void _cst_on_click_call_barring(void *data, Evas *evas, Evas_Object *obj, void *event_info);
void _cst_call_barring_input_panel_event_callback(void *data, Ecore_IMF_Context *imf_context, int value);
void _cst_call_barring_entry_changed_cb(void *data, Evas_Object *obj, void *event_info);
void _cst_destroy_call_barring(void);

#endif  /* _CALL_SET_BARRING_SUPPORT */
#endif	/* _CST_BARRING_H_ */

