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

namespace Model { namespace Settings {
	typedef enum {
		SETTINGS_RES_SUCCESS,
		SETTINGS_RES_FAIL_KEY_UNKNOWN,
		SETTINGS_RES_FAIL_KEY_TYPE_WRONG,
		SETTINGS_RES_FAIL_DATA_MISSING,
		SETTINGS_RES_FAIL_UNEXPECTED_DATA,
		SETTINGS_RES_FAIL_UNKNOWN_ERROR,
	} ResultCode;

	typedef enum {
		SETTINGS_KEY_CALL_ANSWERING_BY_HOME_KEY,
		SETTINGS_KEY_CALL_ENDING_BY_POWER_KEY,
	} BoolKey;

	typedef enum {
		SETTINGS_KEY_SHOW_CALLER_ID,
		SETTINGS_KEY_REJECT_MSG_COUNT,
	} IntKey;

	typedef enum {
		SETTINGS_KEY_REJECT_MSG_ITEM_1,
		SETTINGS_KEY_REJECT_MSG_ITEM_2,
		SETTINGS_KEY_REJECT_MSG_ITEM_3,
		SETTINGS_KEY_REJECT_MSG_ITEM_4,
		SETTINGS_KEY_REJECT_MSG_ITEM_5,
		SETTINGS_KEY_REJECT_MSG_ITEM_6,
	} StringKey;
} }
#endif /* SETTINGS_TYPES_H_ */
