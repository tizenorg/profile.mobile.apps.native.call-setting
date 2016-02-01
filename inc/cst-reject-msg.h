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

#ifndef _CST_REJECT_MSG_H_
#define _CST_REJECT_MSG_H_

#include <Elementary.h>
void _cst_on_click_reject_message(void *data, Evas *evas, Evas_Object *obj, void *event_info);
char *_cst_get_reject_message(int index, gboolean b_parsing, gboolean b_translated);
void _cst_set_reject_message(int index, char *message);
int _cst_set_num_of_reject_message(int num);
void _cst_reject_msg_create_navi_control_bar(CstUgData_t *ugd);
void _cst_genlist_append_reject_list_item(CstUgData_t *ugd, Evas_Object *genlist);
int _cst_get_num_of_reject_message(void);
void _cst_reject_msg_input_panel_event_callback(void *data, Ecore_IMF_Context *imf_context, int value);
void _cst_reject_msg_changed_editfield_cb(void *data, Evas_Object *obj, void *event_info);
void _cst_destroy_reject_message(CstUgData_t *ugd);
void _cst_reject_msg_entry_filter_check_boundary(void *data, Evas_Object *entry, char **text);
void _cst_set_reject_message_util(int index, char *message);

#endif	/* _CST_REJECT_MSG_H_ */

