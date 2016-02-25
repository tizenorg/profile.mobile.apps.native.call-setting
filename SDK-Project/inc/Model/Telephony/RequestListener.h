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

#include "Model/Telephony/TelephonyTypes.h"
#include "Model/Telephony/BaseRequestListener.h"

namespace Model { namespace Telephony {

	class ITelephonyManager;
	class BaseRequestListener;

	class IBaseRequestListener {
	public:
		virtual ~IBaseRequestListener(){}
		virtual bool onAttach(ITelephonyManager *telephonyManager, int requestId) = 0;
		virtual void onDetach() = 0;
	};

	template <typename VALUE_TYPE>
	class IRequestListener : public IBaseRequestListener {
	public:
		virtual void onRequestComplete(ResultCode result, const VALUE_TYPE *value) = 0;
	};

	class ISimpleRequestListener : public IBaseRequestListener {
	public:
		virtual void onRequestComplete(ResultCode result) = 0;
	};

	template <typename VALUE_TYPE>
	class RequestListener : public BaseRequestListener,
							public IRequestListener<VALUE_TYPE> {
	public:
		RequestListener(){}
		virtual ~RequestListener(){}
		virtual bool onAttach(ITelephonyManager *telephonyManager, int requestId);
		virtual void onDetach();
	};

	class SimpleRequestListener : public BaseRequestListener,
								public ISimpleRequestListener {
	public:
		SimpleRequestListener(){}
		virtual ~SimpleRequestListener(){}
		virtual bool onAttach(ITelephonyManager *telephonyManager, int requestId);
		virtual void onDetach();
	};

	template class RequestListener<CallWaitingReqData>;
	template class RequestListener<CallFwdReqData>;
} }

#endif /* REQUEST_LISTENER_H_ */
