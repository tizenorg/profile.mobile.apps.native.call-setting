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

#ifndef TELEPHONYMANAGER_H_
#define TELEPHONYMANAGER_H_

#include <deque>
#include <tapi_common.h>
#include <TelSs.h>
#include "Model/Telephony/ITelephonyManager.h"

namespace Model { namespace Telephony {

	class TelephonyManager : public ITelephonyManager {
	public:
		TelephonyManager();
		virtual ~TelephonyManager();
		virtual ResultCode requestCallWaitState(CallWaitingReqData &reqData, RequestListener<CallWaitingReqData> &listener);
		virtual ResultCode requestCallWaitSetup(CallWaitingReqData &reqData, SimpleRequestListener &listener);
		virtual ResultCode requestCallFwdState(CallFwdReqData &reqData, RequestListener<CallFwdReqData> &listener);
		virtual ResultCode requestCallFwdSetup(CallFwdReqData &reqData, SimpleRequestListener &listener);
		virtual void cancelRequest(int requestId);
		virtual SimCardState getSimState();
		virtual int getSimSlotCount();
		virtual ResultCode setActiveSlot(SimSlot slotId);
		virtual SimSlot getActiveSlot();

	private:
		typedef enum {
			SETUP_REQUEST,
			STATUS_REQUEST,
			UNDEFINED_TYPE,
		} requestType;

		typedef enum {
			CALL_WAITING,
			CALL_FORWARD,
			UNDEFINED_OPERATION,
		} requestOperation;

		typedef enum {
			requestPending,
			requestProcessing,
			requestCanceled,
		} requestState;

		struct TelephonyRequest {
			requestType m_reqType;
			requestOperation m_reqOperation;
			requestState m_reqState;
			int m_requestId;
			IBaseRequestListener *m_pListener;
			TelephonyManager *m_pTelephonyManager;
			TapiHandle *m_pSimHandler;
			void *m_pReqData;

			TelephonyRequest();
			~TelephonyRequest();
			TelephonyRequest(const TelephonyRequest&) = delete;
			TelephonyRequest &operator = (const TelephonyRequest&) = delete;
		};

		std::deque <TelephonyRequest *> m_requestQueue;
		int m_requestCounter;
		int slotCount;
		TapiHandle *m_pSim1Handler;
		TapiHandle *m_pSim2Handler;
		TapiHandle *m_pActiveSimHandler;
		SimSlot m_activeSlot;

		void initTelephonyManager();
		void deinitTelephonyManager();
		ResultCode putRequestToQueue(requestType reqType, requestOperation reqOperation, void *reqData, IBaseRequestListener *listener);
		void clearRequestQueue();
		void processNextTelephonyRequest();
		void processCallForwardRequest(TelephonyRequest *request);
		void processCallWaitingRequest(TelephonyRequest *request);
		ResultCode convertFromTapiResultCode(TelSsCause_t tapiError);
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
		static ResultCode parseCWStatusRequestResponse(TelSsWaitingResp_t *cwInfo, TelephonyRequest *request);
		static ResultCode parseCFStatusRequestResponse(TelSsForwardResp_t *cfInfo, TelephonyRequest *request);
	};
} }

#endif /* TELEPHONYMANAGER_H_ */
