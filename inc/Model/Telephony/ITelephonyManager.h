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

#ifndef _MODEL_I_TELEPHONY_MANAGER_H_
#define _MODEL_I_TELEPHONY_MANAGER_H_

#include "TelephonyTypes.h"

namespace CallSettings { namespace Model {

	class SimpleTelRequestListener;
	template <typename VALUE_TYPE> class TelRequestListener;
	/*
	 * @brief Represents Telephony manager interface for operations with GSM Suplementary services
	 */
	class ITelephonyManager {
	public:
		virtual ~ITelephonyManager(){}

		/*
		 * @brief Add handler for call waiting option settings change event
		 * @param[in]	handler 	Event handler
		 * @return Result code
		 */
		virtual TelResultCode addCallWaitChangeHandler(NotiHandler handler) = 0;

		/*
		 * @brief Remove handler for call waiting option settings change event
		 * @param[in]	handler 	Event handler
		 */
		virtual void removeCallWaitChangeHandler(NotiHandler handler) = 0;

		/*
		 * @brief Add handler for call forwarding option settings change event
		 * @param[in]	handler 	Event handler
		 * @return Result code
		 */
		virtual TelResultCode addCallFwdChangeHandler(NotiHandler handler) = 0;

		/*
		 * @brief Remove handler for forwarding option settings change event
		 * @param[in]	handler 	Event handler
		 */
		virtual void removeCallFwdChangeHandler(NotiHandler handler) = 0;

		/*
		 * @brief Get Call waiting settings state
		 * @param[in]	reqData		Call wait data
		 * @param[in]	listener	Request listener
		 * @note Request state operation is async and needed mobile network connection. This API can be used for Voice call and Video call
		 * either. To determine call type need to set it in reqData. When get request is completed listener callback will be invoked
		 */
		virtual TelResultCode requestCallWaitState(CallWaitingReqData *reqData, TelRequestListener<CallWaitingReqData> *listener) = 0;

		/*
		 * @brief Setup for Call waiting settings
		 * @param[in]	reqData		Call wait data
		 * @param[in]	listener	Request listener
		 * @note Request call waiting setup operation is async and needed mobile network connection. This API can be used for Voice call and Video call
		 * either. When get request is completed listener callback will be invoked
		 */
		virtual TelResultCode requestCallWaitSetup(CallWaitingReqData *reqData, SimpleTelRequestListener *listener) = 0;

		/*
		 * @brief Get Call forwarding settings state
		 * @param[in]	reqData		Call forwarding data
		 * param[in]	listener	Request listener
		 * @note Request call forwarding settings state is async and needed mobile network connection. This API can be used for Voice call and Video call
		 * and for different types of forward conditions. Needed settings must be set in reqData
		 *  When get request is completed listener callback will be invoked
		 */
		virtual TelResultCode requestCallFwdState(CallFwdReqData *reqData, TelRequestListener<CallFwdReqData> *listener) = 0;

		/*
		 * @brief Setup Call forwarding settings
		 * @param[in]	reqData		Call forwarding data
		 * @param[in]	listener	Request listener
		 * @note Request call forwarding settings setup is async and needed mobile network connection. This API can be used for Voice call and Video call
		 * and for different types of forward conditions. Needed settings must be set in reqData
		 *  When get request is completed listener callback will be invoked
		 */
		virtual TelResultCode requestCallFwdSetup(CallFwdReqData *reqData, SimpleTelRequestListener *listener) = 0;

		/*
		 * @brief Cancel request by request id
		 * param[in] requestId	id of request
		 */
		virtual void cancelRequest(int requestId) = 0;

		/*
		 * @brief get SIM card state for current active SIM Slot
		 * @return SIM card state
		 */
		virtual SimCardState getSimState() = 0;

		/*
		 * @brief Get count of SIM slots available in device
		 * @return Number of SIM slots
		 */
		virtual int getSimSlotCount() = 0;

		/*
		 * @brief Setup active SIM slot. After SIM slot is setted up all operations with Telephony manager are related to active slot only
		 * @param[in]	slotId	SIM slot id
		 * @return	result code
		 */
		virtual TelResultCode setActiveSlot(SimSlot slotId) = 0;

		/*
		 * @brief Get SIM slot id which is currently set as active
		 * @return Active SIM slot id
		 */
		virtual SimSlot getActiveSlot() = 0;
	};
} }

#endif /* _MODEL_I_TELEPHONY_MANAGER_H_ */
