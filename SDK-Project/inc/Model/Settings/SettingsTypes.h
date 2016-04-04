/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _MODEL_SETTINGS_TYPES_H_
#define _MODEL_SETTINGS_TYPES_H_

#include "Common.h"

namespace CallSettings { namespace Model {

	typedef enum {
		SETTINGS_RES_SUCCESS,
		SETTINGS_RES_FAIL_KEY_UNKNOWN,
		SETTINGS_RES_FAIL_KEY_TYPE_WRONG,
		SETTINGS_RES_FAIL_DATA_MISSING,
		SETTINGS_RES_FAIL_UNEXPECTED_DATA,
		SETTINGS_RES_FAIL_UNKNOWN_ERROR,
	} SettingResultCode;

	typedef enum {
		INT_KEY_SHOW_CALLER_ID = 0,
		INT_KEY_REJECT_MSG_COUNT,
	} IntKey;

	typedef enum {
		BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY = 10,
		BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
	} BoolKey;

	typedef enum {
		STRING_KEY_REJECT_MSG_ITEM_1 = 20,
		STRING_KEY_REJECT_MSG_ITEM_2,
		STRING_KEY_REJECT_MSG_ITEM_3,
		STRING_KEY_REJECT_MSG_ITEM_4,
		STRING_KEY_REJECT_MSG_ITEM_5,
		STRING_KEY_REJECT_MSG_ITEM_6,
	} StringKey;
} }

#endif /* _MODEL_SETTINGS_TYPES_H_ */
