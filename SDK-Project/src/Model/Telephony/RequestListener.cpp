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

#include "Model/Telephony/RequestListener.h"

namespace Model { namespace Telephony {

	template <typename VALUE_TYPE>
	bool RequestListener<VALUE_TYPE>::onAttach(ITelephonyManager *telephonyManager, int requestId)
	{
		if (telephonyManager) {
			return BaseRequestListener::onAttachImpl(telephonyManager, requestId);
		} else {
			return false;
		}
	}

	template <typename VALUE_TYPE>
	void RequestListener<VALUE_TYPE>::onDetach()
	{
		BaseRequestListener::onDetachImpl();
	}

	bool SimpleRequestListener::onAttach(ITelephonyManager *telephonyManager, int requestId)
	{
		if (telephonyManager) {
			return BaseRequestListener::onAttachImpl(telephonyManager, requestId);
		} else {
			return false;
		}
	}

	void SimpleRequestListener::onDetach()
	{
		BaseRequestListener::onDetachImpl();
	}

} }
