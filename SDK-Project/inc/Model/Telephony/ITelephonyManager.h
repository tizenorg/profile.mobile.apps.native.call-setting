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

	class SimpleRequestListener;
	template <typename VALUE_TYPE> class RequestListener;

	class ITelephonyManager {
	public:
		virtual ~ITelephonyManager(){}
		virtual TelResultCode requestCallWaitState(CallWaitingReqData *reqData, RequestListener<CallWaitingReqData> *listener) = 0;
		virtual TelResultCode requestCallWaitSetup(CallWaitingReqData *reqData, SimpleRequestListener *listener) = 0;
		virtual TelResultCode requestCallFwdState(CallFwdReqData *reqData, RequestListener<CallFwdReqData> *listener) = 0;
		virtual TelResultCode requestCallFwdSetup(CallFwdReqData *reqData, SimpleRequestListener *listener) = 0;
		virtual void cancelRequest(int requestId) = 0;
		virtual SimCardState getSimState() = 0;
		virtual int getSimSlotCount() = 0;
		virtual TelResultCode setActiveSlot(SimSlot slotId) = 0;
		virtual SimSlot getActiveSlot() = 0;
	};
} }

#endif /* _MODEL_I_TELEPHONY_MANAGER_H_ */
