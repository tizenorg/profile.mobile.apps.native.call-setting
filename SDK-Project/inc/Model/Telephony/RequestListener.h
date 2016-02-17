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

#ifndef REQUEST_LISTENER_H_
#define REQUEST_LISTENER_H_


namespace Model { namespace Telephony {

	class ITelephonyManager;

	class RequestListenerBase {
	private:
		int m_requestId;
		ITelephonyManager* m_pTelephonyManager;

	public:
		RequestListenerBase();
		~RequestListenerBase();
		void cancelRequest();
		void attachRequest(ITelephonyManager *TelephonyManager, int requestId);
		void detachRequest(ITelephonyManager*TelephonyManager, int requestId);
		bool isRequestAttached();
	};

	template <typename VALUE_TYPE>
	class RequestListener : public virtual RequestListenerBase {
	public:
		RequestListener();
		virtual ~RequestListener();
		virtual void onRequestComplete(ResultCode result, const VALUE_TYPE &value) = 0;
	};

	class SimpleRequestListener : public virtual RequestListenerBase {
	public:
		SimpleRequestListener();
		virtual ~SimpleRequestListener();
		virtual void onRequestComplete(ResultCode result) = 0;
	};
} }

#endif /* REQUEST_LISTENER_H_ */
