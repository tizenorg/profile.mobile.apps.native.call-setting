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

#ifndef _MODEL_BASE_REQUEST_LISTENER_H_
#define _MODEL_BASE_REQUEST_LISTENER_H_

namespace CallSettings { namespace Model {

	class ITelephonyManager;
	/*
	 * @brief Base class for representation Telephony request listener instance
	 */
	class BaseTelRequestListener {
	public:
		/*
		 * @brief Cancel currently launched request which belongs to request listener instance
		 */
		void cancelRequest();

		/*
		 * @return true if request is attached to Telephony manager and processing, otherwise false
		 */
		bool isAttached();
	protected:
		BaseTelRequestListener();
		virtual ~BaseTelRequestListener();
		bool onAttachImpl(ITelephonyManager *telephonyManager, int requestId);
		void onDetachImpl();
	private:
		int m_requestId;
		ITelephonyManager* m_pTelephonyManager;
	};
} }

#endif /* _MODEL_BASE_REQUEST_LISTENER_H_ */
