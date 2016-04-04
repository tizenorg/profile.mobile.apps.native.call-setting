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

#ifndef _MODEL_TELEPHONY_MANAGER_H_
#define _MODEL_TELEPHONY_MANAGER_H_

#include <tapi_common.h>
#include <TelSs.h>
#include "ITelephonyManager.h"

namespace CallSettings { namespace Model {

	struct TelephonyRequest;

	class TelephonyManager : public ITelephonyManager {
	public:
		TelephonyManager();
		virtual ~TelephonyManager();
		virtual TelResultCode requestCallWaitState(CallWaitingReqData *reqData, RequestListener<CallWaitingReqData> *listener);
		virtual TelResultCode requestCallWaitSetup(CallWaitingReqData *reqData, SimpleRequestListener *listener);
		virtual TelResultCode requestCallFwdState(CallFwdReqData *reqData, RequestListener<CallFwdReqData> *listener);
		virtual TelResultCode requestCallFwdSetup(CallFwdReqData *reqData, SimpleRequestListener *listener);
		virtual void cancelRequest(int requestId);
		virtual SimCardState getSimState();
		virtual int getSimSlotCount();
		virtual TelResultCode setActiveSlot(SimSlot slotId);
		virtual SimSlot getActiveSlot();

	private:
		void initTelephonyManager();
		void deinitTelephonyManager();
		void putRequestToQueue(TelephonyRequest *request);
		void clearRequestQueue();
		void processNextTelephonyRequest();
		void processCallForwardRequest(TelephonyRequest *request);
		void processCallWaitingRequest(TelephonyRequest *request);
		TelResultCode convertFromTapiResultCode(TelSsCause_t tapiError);
		TelSsForwardWhen_t convertToTapiCallFwdType(CallFwdCondition condition);
		CallFwdCondition convertFromTapiCallFwdType(TelSsForwardWhen_t fwdTapiCondition);
		TelSsCallWaitingMode_t convertToTapiCallWaitingMode(CallWaitingMode mode);
		CallWaitingMode convertTapiStatusToCallWaitingMode(TelSsStatus_t tapiWaitingMode);
		TelSsNoReplyTime_t convertToTapiNoReplyTime(CallFwdNoReplyTime waitTime);
		CallFwdNoReplyTime convertFromTapiNoReplyTime(TelSsNoReplyTime_t tapiWaitTime);
		TelSsForwardMode_t convertToTapiCallFwdMode(CallForwardMode state);
		CallForwardMode convertTapiStatusToCallFwdMode(TelSsStatus_t cfStatus);
		TelSsClass_t convertToTapiCallClassType(CallType callClassType);
		CallType convertFromTapiCallClassType(TelSsClass_t tapiClass);
		static void responseOnSetupRequest(TapiHandle *handle, int result, void *data, void *userData);
		static void responseOnCWStatusRequest(TapiHandle *handle, int result, void *data, void *userData);
		static void responseOnCFStatusRequest(TapiHandle *handle, int result, void *data, void *userData);
		static TelResultCode parseCWStatusRequestResponse(TelSsWaitingResp_t *cwInfo, TelephonyRequest *request);
		static TelResultCode parseCFStatusRequestResponse(TelSsForwardResp_t *cfInfo, TelephonyRequest *request);

	private:
		std::deque <TelephonyRequest *> m_requestQueue;
		int m_requestCounter;
		int slotCount;
		TapiHandle *m_pSim1Handler;
		TapiHandle *m_pSim2Handler;
		TapiHandle *m_pActiveSimHandler;
		SimSlot m_activeSlot;
	};
} }

#endif /* _MODEL_TELEPHONY_MANAGER_H_ */
