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

#ifndef _CST_FDN_LIST_H_
#define _CST_FDN_LIST_H_

#ifdef _CALL_SET_FDN_SUPPORT

#include <Elementary.h>
#include "cst-common.h"

#define ORANGE_FRANCE_CODE	"FTM"

void _cst_on_click_fdn_list(void *data);
void _cst_set_index_used(int index, Eina_Bool used);
void _cst_fdn_list_input_panel_event_cb(void *data, Ecore_IMF_Context *imf_context, int value);
void _cst_add_new_fdn_contact_to_list(void *data, CallSettingFDNRec_t *fdn_rec, gboolean is_fdn_contact_view);
void _cst_update_fdn_list(void *data, CallSettingFDNRec_t *fdn_rec);
void _cst_fdn_list_back_to_prev(CstUgData_t *ugd);
void _cst_fdn_list_initialize_genlist(CstUgData_t *ugd);
void cst_fdn_list_tapi_sim_response_update_cb(CstUgData_t *ugd, void *req_data, void *resp_data);
int _cst_get_first_available_free_index();
void _cst_destroy_fdn_list(CstUgData_t *ugd);

#endif  /* _CALL_SET_FDN_SUPPORT */
#endif  /* _CST_FDN_LIST_H_ */

