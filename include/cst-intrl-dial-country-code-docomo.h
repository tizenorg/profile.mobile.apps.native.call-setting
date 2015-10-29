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

#ifndef _CST_INTRL_DIAL_COUNTRY_CODE_DOCOMO_H_
#define _CST_INTRL_DIAL_COUNTRY_CODE_DOCOMO_H_

#include <Elementary.h>

void _cst_on_click_intrl_dial_country_code_docomo(void *data, Evas *evas,
		Evas_Object *obj, void *event_info);
void _cst_intrl_dial_country_code_entry_changed_docomo_cb(void *data,
		Evas_Object *obj, void *event_info);
int _cst_intrl_dial_country_code_get_count();
void _cst_update_genlist_intrl_dial_country_code_docomo(void *data);
void _cst_intrl_dial_country_code_unlisten_vconf_change_docomo(void);

#endif /*_CST_INTRL_DIAL_COUNTRY_CODE_DOCOMO_H_*/

