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

#ifndef SETTINGS_TYPES_H_
#define SETTINGS_TYPES_H_

#include "Utils/Common.h"

namespace Model { namespace Settings {

	typedef Delegate<void(bool)> BoolPropertyHandler;
	typedef Delegate<void(int)> IntPropertyHandler;
	typedef Delegate<void(std::string)> StringPropertyHandler;

	typedef enum {
		SETTINGS_RES_SUCCESS,
		SETTINGS_RES_FAIL_KEY_UNKNOWN,
		SETTINGS_RES_FAIL_KEY_TYPE_WRONG,
		SETTINGS_RES_FAIL_DATA_MISSING,
		SETTINGS_RES_FAIL_UNEXPECTED_DATA,
		SETTINGS_RES_FAIL_UNKNOWN_ERROR,
	} ResultCode;

	typedef enum {
		BOOL_KEY_UNDEFINED = -1,
		BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
		BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
	} BoolKey;

	typedef enum {
		INT_KEY_UNDEFINED = -1,
		INT_KEY_SHOW_CALLER_ID,
		INT_KEY_REJECT_MSG_COUNT,
	} IntKey;

	typedef enum {
		STRING_KEY_UNDEFINED = -1,
		STRING_KEY_REJECT_MSG_ITEM_1,
		STRING_KEY_REJECT_MSG_ITEM_2,
		STRING_KEY_REJECT_MSG_ITEM_3,
		STRING_KEY_REJECT_MSG_ITEM_4,
		STRING_KEY_REJECT_MSG_ITEM_5,
		STRING_KEY_REJECT_MSG_ITEM_6,
	} StringKey;
} }
#endif /* SETTINGS_TYPES_H_ */
