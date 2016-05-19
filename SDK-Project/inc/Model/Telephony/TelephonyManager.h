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

		virtual TelResultCode addCallWaitChangeHandler(NotiHandler handler);
		virtual void removeCallWaitChangeHandler(NotiHandler handler);

		virtual TelResultCode addCallFwdChangeHandler(NotiHandler handler);
		virtual void removeCallFwdChangeHandler(NotiHandler handler);

		virtual TelResultCode requestCallWaitState(CallWaitingReqData *reqData, TelRequestListener<CallWaitingReqData> *listener);
		virtual TelResultCode requestCallWaitSetup(CallWaitingReqData *reqData, SimpleTelRequestListener *listener);

		virtual TelResultCode requestCallFwdState(CallFwdReqData *reqData, TelRequestListener<CallFwdReqData> *listener);
		virtual TelResultCode requestCallFwdSetup(CallFwdReqData *reqData, SimpleTelRequestListener *listener);

		virtual void cancelRequest(int requestId);

		virtual SimCardState getSimState();
		virtual int getSimSlotCount();

		virtual TelResultCode setActiveSlot(SimSlot slotId);
		virtual SimSlot getActiveSlot();

	private:
		void initTelephonyManager();
		void initSimSlotInfo();
		void deinitTelephonyManager();

		SimCardState getSimStateBySlotHandler(TapiHandle *handle);

		void putRequestToQueue(TelephonyRequest *request);
		void clearRequestQueue();
		void processNextTelephonyRequest();

		void processCallForwardRequest(TelephonyRequest *request);
		void processCallWaitingRequest(TelephonyRequest *request);

		static TelResultCode convertFromTapiResultCode(TelSsCause_t tapiError);
		static TelSsForwardWhen_t convertToTapiCallFwdType(CallFwdCondition condition);

		static CallFwdCondition convertFromTapiCallFwdType(TelSsForwardWhen_t fwdTapiCondition);
		static TelSsCallWaitingMode_t convertToTapiCallWaitingMode(CallWaitingMode mode);

		static CallWaitingMode convertTapiStatusToCallWaitingMode(TelSsStatus_t tapiWaitingMode);
		static TelSsNoReplyTime_t convertToTapiNoReplyTime(CallFwdNoReplyTime waitTime);

		static CallFwdNoReplyTime convertFromTapiNoReplyTime(TelSsNoReplyTime_t tapiWaitTime);
		static TelSsForwardMode_t convertToTapiCallFwdMode(CallForwardMode state);

		static CallForwardMode convertTapiStatusToCallFwdMode(TelSsStatus_t cfStatus);
		static TelSsClass_t convertToTapiCallClassType(CallType callClassType);
		static CallType convertFromTapiCallClassType(TelSsClass_t tapiClass);

		static void responseOnSetupRequest(TapiHandle *handle, int result, void *data, void *userData);
		static void responseOnCWStatusRequest(TapiHandle *handle, int result, void *data, void *userData);
		static void responseOnCFStatusRequest(TapiHandle *handle, int result, void *data, void *userData);

		static TelResultCode parseCWStatusRequestResponse(TelSsWaitingResp_t *cwInfo, TelephonyRequest *request);
		static TelResultCode parseCFStatusRequestResponse(TelSsForwardResp_t *cfInfo, TelephonyRequest *request);

		TelResultCode addEventHandlerImpl(int key, NotiHandler handler);
		void removeEventHandlerImpl(int key, NotiHandler handler);

		TelResultCode registerTelephonyEventCb(int key);
		void unregisterTelephonyEventCb(int key);

		static void onTelephonyEventCb(TapiHandle *handle, const char *notiId, void *data, void *userData);

	private:
		typedef util::Delegation<void()> HandlersCollection;
		typedef std::map<int, HandlersCollection *> HandlersMap;

		std::deque <TelephonyRequest *> m_requestQueue;
		int m_requestCounter;
		int m_slotCount;
		TapiHandle *m_pSim1Handler;
		TapiHandle *m_pSim2Handler;
		TapiHandle *m_pActiveSimHandler;
		SimSlot m_activeSlot;
		HandlersMap m_eventHandlersMap;
	};
} }

#endif /* _MODEL_TELEPHONY_MANAGER_H_ */
