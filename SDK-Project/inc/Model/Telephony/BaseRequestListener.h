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

#ifndef BASE_REQUEST_LISTENER_H_
#define BASE_REQUEST_LISTENER_H_

namespace Model { namespace Telephony {
	class ITelephonyManager;

	class BaseRequestListener {
			public:
				void cancelRequest();
				bool isAttached();
			protected:
				BaseRequestListener();
				virtual ~BaseRequestListener();
				bool onAttachImpl(ITelephonyManager *telephonyManager, int requestId);
				void onDetachImpl();
			private:
				int m_requestId;
				ITelephonyManager* m_pTelephonyManager;
	};
} }

#endif /* BASE_REQUEST_LISTENER_H_ */
