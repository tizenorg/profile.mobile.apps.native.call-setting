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

#ifndef _MODEL_TELEPHONY_TYPES_H_
#define _MODEL_TELEPHONY_TYPES_H_

#include "Common.h"

namespace CallSettings { namespace Model {

	/*
	 * @brief TelephonyManager Result codes
	 */
	typedef enum {
		TELEPHONY_RES_SUCCESS,
		TELEPHONY_RES_FAIL_NETWORK_API_FAILED,
		TELEPHONY_RES_FAIL_SERVICE_NOT_AVAILABLE,
		TELEPHONY_RES_FAIL_OPTION_NOT_SUPPORTED,
		TELEPHONY_RES_FAIL_DATA_MISSING,
		TELEPHONY_RES_FAIL_UNEXPECTED_DATA,
		TELEPHONY_RES_FAIL_REJECT_BY_NETWORK,
		TELEPHONY_RES_FAIL_MEMORY_BAD_ALLOC,
		TELEPHONY_RES_FAIL_UNKNOWN_ERROR,
	} TelResultCode;

	/*
	 * @brief Telephony manager call types
	 */
	typedef enum {
		TELEPHONY_CALLTYPE_VOICE,
		TELEPHONY_CALLTYPE_VIDEO,
		TELEPHONY_CALLTYPE_ALL,
		TELEPHONY_CALLTYPE_UNDEFINED,
	} CallType;

	/*
	 * @brief Call forwarding condition types
	 */
	typedef enum {
		TELEPHONY_CF_UNCONDITIONAL,
		TELEPHONY_CF_IF_BUSY,
		TELEPHONY_CF_IF_NO_REPLY,
		TELEPHONY_CF_IF_NO_REACHABLE,
		TELEPHONY_CF_IF_UNDEFINED,
	} CallFwdCondition;

	/*
	 * @brief Time intervals for call forward when unreachable condition
	 */
	typedef enum {
		TELEPHONY_CF_NO_REPLY_5_SEC,
		TELEPHONY_CF_NO_REPLY_10_SEC,
		TELEPHONY_CF_NO_REPLY_15_SEC,
		TELEPHONY_CF_NO_REPLY_20_SEC,
		TELEPHONY_CF_NO_REPLY_25_SEC,
		TELEPHONY_CF_NO_REPLY_30_SEC,
		TELEPHONY_CF_NO_REPLY_UNDEFINED,
	} CallFwdNoReplyTime;

	/*
	 * @brief Call waiting modes
	 */
	typedef enum {
		TELEPHONY_CW_MODE_ACTIVATE,
		TELEPHONY_CW_MODE_DEACTIVATE,
		TELEPHONY_CW_MODE_UNDEFINED,
	} CallWaitingMode;

	/*
	 * @brief Call forwarding setting modes
	 */
	typedef enum {
		TELEPHONY_CF_MODE_ACTIVATE,		/**Call forwarding is active*/
		TELEPHONY_CF_MODE_REGISTER,		/**Call forwarding registration state. Needed for save new settings*/
		TELEPHONY_CF_MODE_DEACTIVATE,	/**Call forwarding is not set*/
		TELEPHONY_CF_MODE_UNDEFINED,	/**Call forwarding state undefined*/
	} CallForwardMode;

	/*
	 * @brief Sim card state types
	 */
	typedef enum {
		TELEPHONY_SIMCARD_STATE_REMOVED,
		TELEPHONY_SIMCARD_STATE_READY,
		TELEPHONY_SIMCARD_STATE_PROCESSING,
		TELEPHONY_SIMCARD_STATE_BLOCKED,
		TELEPHONY_SIMCARD_STATE_CRASHED,
		TELEPHONY_SIMCARD_STATE_UNKNOWN,
	} SimCardState;

	/*
	 * @brief Sim slot types. Uses for multisim devices
	 */
	typedef enum {
		TELEPHONY_SIM_SLOT_NONE,
		TELEPHONY_SIM_SLOT_1,
		TELEPHONY_SIM_SLOT_2,
	} SimSlot;

	/*
	 * @brief call waiting setting data. Describes type and status of call waiting option
	 */
	struct CallWaitingReqData {
		CallType callType;
		CallWaitingMode mode;
		CallWaitingReqData() : callType(TELEPHONY_CALLTYPE_UNDEFINED),
			mode(TELEPHONY_CW_MODE_UNDEFINED) {}
	};

	/*
	 * @brief Call forward setting data. Describes call forward status and conditions settings
	 */
	struct CallFwdReqData {
		CallType callType;						/**Call type*/
		CallForwardMode mode;					/**Mode for current call forward condition*/
		CallFwdCondition condition;				/**Forward condition*/
		std::string telNumber;					/**Telephone number on which call forwarding will be done*/
		CallFwdNoReplyTime waitTime;			/**Timing related to TELEPHONY_CF_IF_NO_REPLY condition only */
		CallFwdReqData() : callType(TELEPHONY_CALLTYPE_UNDEFINED),
			mode(TELEPHONY_CF_MODE_UNDEFINED),
			condition(TELEPHONY_CF_IF_UNDEFINED),
			telNumber("undefined"),
			waitTime(TELEPHONY_CF_NO_REPLY_UNDEFINED) {}
	};

	/*
	 * @brief String consist from allowed only symbols which can be used in telephone number for SS setup
	 */
	extern const std::string TELEPHONY_NUMBER_ALLOWED_SYMBOLS;

	/*
	 * @brief Max telephony number string length
	 */
	extern const int TELEPHONY_NUMBER_LENGTH_MAX;

} }

#endif /* _MODEL_TELEPHONY_TYPES_H_ */
