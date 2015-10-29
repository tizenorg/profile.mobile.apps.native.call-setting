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

#ifndef _CST_FDN_SETTING_H_
#define _CST_FDN_SETTING_H_

#ifdef _CALL_SET_FDN_SUPPORT

#include <Elementary.h>
#include "cst-common.h"

typedef enum {
	CST_FDN_STATUS_FAIL = -2,
	CST_FDN_STATUS_UNKNOWN,
	CST_FDN_STATUS_DISABLED,
	CST_FDN_STATUS_ENABLED
} CstFdnStatus_t;

void _cst_on_click_fdn_setting(void *data, Evas *evas, Evas_Object *obj, void *event_info);
void _cst_FDN_setting_input_panel_event_cb(void *data, Ecore_IMF_Context *imf_context, int value);
void cst_FDN_setting_show_popup(CstErrPopupType_t err_popup_type, void *data, char *title_txt, const char *info_txt);
void _cst_FDN_setting_changed_editfield_cb(void *data, Evas_Object *obj, void *event_info);
void _cst_destroy_fdn_setting(void);
void _cst_pin2_blocked_popup_confirm_btn_cb(void *data, Evas_Object *obj, void *event_info);
void _cst_create_pin2_confirm_popup(CstUgData_t *ugd, CstImeType_t ime_type,
		const char *done_btn_text, Evas_Smart_Cb done_cb, Evas_Smart_Cb cancel_cb,
		void *cb_data, Evas_Object **c_item);
void _cst_fdn_disable_main_screen_show_unlock_pin2_popup(void *data);
#endif  /* _CALL_SET_FDN_SUPPORT */
#endif	/* _CST_FDN_SETTING_H_ */

