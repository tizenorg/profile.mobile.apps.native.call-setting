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

#include <Elementary.h>

#include "cst-common.h"
#include "cst-common-string.h"

#include "cst-widget.h"
#include "cst-reject-msg-ime.h"

static char *input_string;

Evas_Object *_cst_create_reject_message_ime(Evas_Object *parent, char *edit_string, void *data)
{
	ENTER(_cst_create_reject_message_ime);
	retv_if(NULL == data, NULL);
	CstAppData_t *ad = (CstAppData_t *)data;

	input_string = edit_string;

	return _cst_create_ime_editfield(ad, parent, CST_IME_REJECT_MSG, input_string);
}
