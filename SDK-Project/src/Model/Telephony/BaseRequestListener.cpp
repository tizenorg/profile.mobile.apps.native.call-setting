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

#include "Model/Telephony/BaseRequestListener.h"
#include "Model/Telephony/ITelephonyManager.h"

namespace Model { namespace Telephony {

	BaseRequestListener::BaseRequestListener() :
		m_requestId(-1),
		m_pTelephonyManager(nullptr)
	{

	}

	BaseRequestListener::~BaseRequestListener()
	{
		if (isAttached()) {
			cancelRequest();
		}
	}

	void BaseRequestListener::cancelRequest()
	{
		if (isAttached()) {
			m_pTelephonyManager->cancelRequest(m_requestId);
		}
	}

	bool BaseRequestListener::isAttached()
	{
		if (m_pTelephonyManager && m_requestId != -1) {
			return true;
		} else {
			return false;
		}
	}

	bool BaseRequestListener::onAttachImpl(ITelephonyManager *telephonyManager, int requestId)
	{
		if (!isAttached()) {
			m_pTelephonyManager = telephonyManager;
			m_requestId = requestId;
			return true;
		} else {
			return false;
		}
	}

	void BaseRequestListener::onDetachImpl()
	{
		if (isAttached()) {
			m_pTelephonyManager = nullptr;
			m_requestId = -1;
		}
	}
} }
