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

#ifndef TELEPHONY_REQUEST_H_
#define TELEPHONY_REQUEST_H_

#include "Model/Telephony/TelephonyTypes.h"
#include "Model/Telephony/BaseRequestListener.h"

namespace Model {
	namespace Telephony {
		typedef enum {
			RequestStatus,
			RequestSetup
		} RequestType;

		typedef enum {
			CallWaiting,
			CallForwarding,
		} RequestOperation;

		class TelephonyRequest {
		public:
			TelephonyRequest(RequestType reqType, RequestOperation reqOperation, CallType callType);
			~TelephonyRequest();
			RequestType getRequestType();
			RequestOperation getRequestOperation();
			void addListener(BaseRequestListener* listener);
			void removeListener();

		private:
			RequestType m_reqType;
			RequestOperation m_reqOperation;
			CallType m_callType;
			BaseRequestListener* m_pListener;
		};
	}
}

#endif /* TELEPHONY_REQUEST_H_ */
