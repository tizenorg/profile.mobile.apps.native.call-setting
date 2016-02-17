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

#ifndef I_TELEPHONY_MANAGER_H_
#define I_TELEPHONY_MANAGER_H_

#include "Model/Telephony/TelephonyTypes.h"

namespace Model {
	namespace Telephony {

		class ITelephonyManager {
		public:
			ITelephonyManager();
			virtual ~ITelephonyManager();
			virtual ResultCode addCWStatuspRequest(RequestListener *listener, CallType callType) = 0;
			virtual ResultCode addCWSetupRequest(RequestListener *listener, CallType callType, CallWaitingMode cwMode) = 0;
			virtual ResultCode addCFStatusRequest(RequestListener *listener, CallType callType, CallFwdType cfType) = 0;
			virtual ResultCode addCFSetupRequest(RequestListener *listener, CallType callType, CallFwdType cfType, const char *telNumber, int waitingTime = 0) = 0;
			virtual void cancelRequestById(int requestId) = 0;
			virtual void cancellAllRequests() = 0;
			virtual SimCardState getSimState() = 0;
			virtual int getSimSlotCount() = 0;
			virtual ResultCode setActiveSlot(SimSlot slotId) = 0;
			virtual SimSlot getActiveSlot() = 0;
		};
	}
}

#endif /* I_TELEPHONY_MANAGER_H_ */
