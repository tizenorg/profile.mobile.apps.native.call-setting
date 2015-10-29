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

#ifndef _CST_FDN_CONTACT_H_
#define _CST_FDN_CONTACT_H_

#ifdef _CALL_SET_FDN_SUPPORT
#include <Elementary.h>

Evas_Object *_cst_create_fdn_contact_list(void *data, CstFDNContactOperation_t operation_type, CstFDNContactEditInfo_t *edit_info);
int _cst_get_first_available_free_index();
void _cst_fdn_contact_edit_done(void *data, Evas_Object *obj, void *event_info);
void _cst_fdn_contact_create_done(void *data, Evas_Object *obj, void *event_info);
void _cst_fdn_contact_entry_focus_set(Evas_Object *entry, CstUgData_t *ugd);

#endif  /* _CALL_SET_FDN_SUPPORT */
#endif

