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

namespace Model { namespace Telephony {

	typedef enum {
		TELEPHONY_RES_SUCCESS,
		TELEPHONY_RES_FAIL_NETWORK_API_FAILED,
		TELEPHONY_RES_FAIL_SERVICE_NOT_AVAILABLE,
		TELEPHONY_RES_FAIL_OPTION_NOT_SUPPORTED,
		TELEPHONY_RES_FAIL_DATA_MISSING,
		TELEPHONY_RES_FAIL_UNEXPECTED_DATA,
		TELEPHONY_RES_FAIL_UNKNOWN_ERROR,
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

	typedef struct {
		CallType callType;
		CallWaitingMode mode;
	} CallWaitingReqData;

	typedef struct {
		CallType callType;
		CallFwdType cfType;
		const char* telNumber;
		int waitTime = 0;
	} CallFwdReqData;

} }

#endif /* TELEPHONY_TYPES_H_ */
