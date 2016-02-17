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

#ifndef TELEPHONY_TYPES_H_
#define TELEPHONY_TYPES_H_

namespace Model {
	namespace Telephony {

		typedef enum {
			TELEPHONY_ERR_NONE,
			TELEPHONY_ERR_NETWORK_API_FAILED,
			TELEPHONY_ERR_SERVICE_NOT_AVAILABLE,
			TELEPHONY_ERR_OPTION_NOT_SUPPORTED,
			TELEPHONY_ERR_DATA_MISSING,
			TELEPHONY_ERR_UNEXPECTED_DATA,
			TELEPHONY_ERR_UNKNOWN,
		} ResultCode;

		typedef enum {
			TELEPHONY_CALLTYPE_UNKNOWN,
			TELEPHONY_CALLTYPE_VOICE,
			TELEPHONY_CALLTYPE_VIDEO,
			TELEPHONY_CALLTYPE_ALL,
		} CallType;

		typedef enum {
			TELEPHONY_CF_UNCONDITIONAL,
			TELEPHONY_CF_IF_BUSY,
			TELEPHONY_CF_IF_NO_REPLY,
			TELEPHONY_CF_IF_NO_REACHABLE,
		} CallFwdType;

		typedef enum {
			TELEPHONY_CW_ACTIVATE,
			TELEPHONY_CW_DEACTIVATE,
		} CallWaitingMode;

		typedef enum {
			TELEPHONY_SIMCARD_STATE_READY,
			TELEPHONY_SIMCARD_STATE_PROCESSING,
			TELEPHONY_SIMCARD_STATE_BLOCKED,
			TELEPHONY_SIMCARD_STATE_CRASHED,
			TELEPHONY_SIMCARD_STATE_UNKNOWN,
		} SimCardState;

		typedef enum {
			TELEPHONY_SIM_SLOT_1,
			TELEPHONY_SIM_SLOT_2,
		} SimSlot;
	}
}

#endif /* TELEPHONY_TYPES_H_ */
