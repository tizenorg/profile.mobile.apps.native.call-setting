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

#ifndef _MODEL_REQUEST_LISTENER_H_
#define _MODEL_REQUEST_LISTENER_H_

#include "TelephonyTypes.h"
#include "BaseTelRequestListener.h"

namespace CallSettings { namespace Model {

	class ITelephonyManager;
	class BaseTelRequestListener;

	class IBaseTelRequestListener {
	public:
		virtual ~IBaseTelRequestListener(){}
		/*
		 * @brief Called when Telephony service start processing request
		 * @param[in]	telephonyManager	Telephony manager instance
		 * @param[in]	requestId			request id in TelephonyManager request que
		 * @return true on success, otherwise false
		 */
		virtual bool onAttach(ITelephonyManager *telephonyManager, int requestId) = 0;

		/*
		 * @brief Called when Telephony manager no more processing request
		 * This can be when request is completed, canceled or failed.
		 */
		virtual void onDetach() = 0;
	};

	/*
	 * @brief Interface for Request listeners. Uses for get state of Telephony supplementary service option
	 * When request is finished result code and option state data is provided to listener
	 */
	template <typename VALUE_TYPE>
	class ITelRequestListener : public IBaseTelRequestListener {
	public:
		virtual void onRequestComplete(TelResultCode result, const VALUE_TYPE *value) = 0;
	};

	/*
	 * @brief Interface for Simple request listeners. Uses for setup Telephony supplementary service option so only result code is
	 * provided to listener
	 */
	class ISimpleTelRequestListener : public IBaseTelRequestListener {
	public:
		virtual void onRequestComplete(TelResultCode result) = 0;
	};

	template <typename VALUE_TYPE>
	class TelRequestListener : public BaseTelRequestListener,
							public ITelRequestListener<VALUE_TYPE> {
	public:
		TelRequestListener(){}
		virtual ~TelRequestListener(){}

		virtual bool onAttach(ITelephonyManager *telephonyManager, int requestId);
		virtual void onDetach();
	};

	class SimpleTelRequestListener : public BaseTelRequestListener,
								public ISimpleTelRequestListener {
	public:
		SimpleTelRequestListener(){}
		virtual ~SimpleTelRequestListener(){}
		virtual bool onAttach(ITelephonyManager *telephonyManager, int requestId);
		virtual void onDetach();
	};

} }

#endif /* _MODEL_REQUEST_LISTENER_H_ */
