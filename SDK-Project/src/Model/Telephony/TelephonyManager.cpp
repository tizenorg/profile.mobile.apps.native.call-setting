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

#include <ITapiSim.h>
#include <TapiUtility.h>
#include <TelSim.h>
#include <ITapiSs.h>

#include "Model/Telephony/TelephonyManager.h"
#include "Model/Telephony/RequestListener.h"

const int REQUEST_MAX_ID = 1000;

namespace CallSettings { namespace Model {

	typedef enum {
		REQUEST_TYPE_SETUP,
		REQUEST_TYPE_GET_STATE,
	} requestType;

	typedef enum {
		REQUEST_OPER_TYPE_CALL_WAITING,
		REQUEST_OPER_TYPE_CALL_FORWARD,
	} requestOperation;

	typedef enum {
		REQUEST_STATE_PENDING,
		REQUEST_STATE_PROCESSING,
		REQUEST_STATE_CANCELED,
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

		TelephonyRequest(requestType reqType, requestOperation reqOperation);
		~TelephonyRequest();
		TelephonyRequest(const TelephonyRequest&) = delete;
		TelephonyRequest &operator = (const TelephonyRequest&) = delete;
	};

	TelephonyManager::TelephonyManager() :
		m_requestCounter(0),
		slotCount(0),
		m_pSim1Handler(nullptr),
		m_pSim2Handler(nullptr),
		m_pActiveSimHandler(nullptr),
		m_activeSlot(TELEPHONY_SIM_SLOT_NONE)
	{
		initTelephonyManager();
	}

	TelephonyManager::~TelephonyManager()
	{
		deinitTelephonyManager();
		clearRequestQueue();
	}

	TelephonyRequest::TelephonyRequest(requestType reqType, requestOperation reqOperation) :
		m_reqType(reqType),
		m_reqOperation(reqOperation),
		m_reqState(REQUEST_STATE_PENDING),
		m_requestId(-1),
		m_pListener(nullptr),
		m_pTelephonyManager(nullptr),
		m_pSimHandler(nullptr),
		m_pReqData(nullptr)
	{
	}

	TelephonyRequest::~TelephonyRequest() {
		m_pListener->onDetach();
	}

	TelResultCode TelephonyManager::requestCallWaitState(CallWaitingReqData *reqData, RequestListener<CallWaitingReqData> *listener)
	{
		RETVM_IF(!reqData || !listener, TELEPHONY_RES_FAIL_DATA_MISSING, "Reject request due to invalid data");

		if (listener->onAttach(this, m_requestCounter)) {
			TelephonyRequest *request = new TelephonyRequest(REQUEST_TYPE_GET_STATE, REQUEST_OPER_TYPE_CALL_WAITING);
			RETVM_IF(!request,TELEPHONY_RES_FAIL_MEMORY_BAD_ALLOC, "Failed to create request");
			request->m_pSimHandler = m_pActiveSimHandler;
			request->m_pReqData = reqData;
			request->m_pListener = listener;
			putRequestToQueue(request);
			return TELEPHONY_RES_SUCCESS;
		} else {
			ERR("Request is already attached!");
			return TELEPHONY_RES_FAIL_UNEXPECTED_DATA;
		}

	}

	TelResultCode TelephonyManager::requestCallWaitSetup(CallWaitingReqData *reqData, SimpleRequestListener *listener)
	{
		RETVM_IF(!reqData || !listener, TELEPHONY_RES_FAIL_DATA_MISSING, "Reject request due to invalid data");

		if (listener->onAttach(this, m_requestCounter)) {
			TelephonyRequest *request = new TelephonyRequest(REQUEST_TYPE_SETUP, REQUEST_OPER_TYPE_CALL_WAITING);
			RETVM_IF(!request,TELEPHONY_RES_FAIL_MEMORY_BAD_ALLOC, "Failed to create request");
			request->m_pSimHandler = m_pActiveSimHandler;
			request->m_pReqData = reqData;
			request->m_pListener = listener;
			putRequestToQueue(request);
			return TELEPHONY_RES_SUCCESS;
		} else {
			ERR("Request is already attached!");
			return TELEPHONY_RES_FAIL_UNEXPECTED_DATA;
		}
	}

	TelResultCode TelephonyManager::requestCallFwdState(CallFwdReqData *reqData, RequestListener<CallFwdReqData> *listener)
	{
		RETVM_IF(!reqData || !listener, TELEPHONY_RES_FAIL_DATA_MISSING, "Reject request due to invalid data");

		if (listener->onAttach(this, m_requestCounter)) {
			TelephonyRequest *request = new TelephonyRequest(REQUEST_TYPE_GET_STATE, REQUEST_OPER_TYPE_CALL_FORWARD);
			RETVM_IF(!request,TELEPHONY_RES_FAIL_MEMORY_BAD_ALLOC, "Failed to create request");
			request->m_pSimHandler = m_pActiveSimHandler;
			request->m_pReqData = reqData;
			request->m_pListener = listener;
			putRequestToQueue(request);
			return TELEPHONY_RES_SUCCESS;
		} else {
			ERR("Request is already attached!");
			return TELEPHONY_RES_FAIL_UNEXPECTED_DATA;
		}
	}

	TelResultCode TelephonyManager::requestCallFwdSetup(CallFwdReqData *reqData, SimpleRequestListener *listener)
	{
		RETVM_IF(!reqData || !listener, TELEPHONY_RES_FAIL_DATA_MISSING, "Reject request due to invalid data");

		if (listener->onAttach(this, m_requestCounter)) {
			TelephonyRequest *request = new TelephonyRequest(REQUEST_TYPE_GET_STATE, REQUEST_OPER_TYPE_CALL_FORWARD);
			RETVM_IF(!request,TELEPHONY_RES_FAIL_MEMORY_BAD_ALLOC, "Failed to create request");
			request->m_pSimHandler = m_pActiveSimHandler;
			request->m_pReqData = reqData;
			request->m_pListener = listener;
			putRequestToQueue(request);
			return TELEPHONY_RES_SUCCESS;
		} else {
			ERR("Request is already attached!");
			return TELEPHONY_RES_FAIL_UNEXPECTED_DATA;
		}
	}

	void TelephonyManager::cancelRequest(int requestId)
	{
		auto it = std::find_if(m_requestQueue.begin(), m_requestQueue.end(), [&requestId](const TelephonyRequest *item) {
			return item->m_requestId == requestId;
		});

		if (it != m_requestQueue.end()) {
			TelephonyRequest *item = *it;
			if (item->m_reqState == REQUEST_STATE_PROCESSING) {
				item->m_reqState = REQUEST_STATE_CANCELED;
			} else {
				m_requestQueue.erase(it);
			}
		}
	}

	SimCardState TelephonyManager::getSimState()
	{
		RETVM_IF(!m_pActiveSimHandler, TELEPHONY_SIMCARD_STATE_UNKNOWN, "Sim slots are not available");

		int tapiRes = TAPI_API_SUCCESS;
		TelSimCardStatus_t simStatus = TAPI_SIM_STATUS_UNKNOWN;
		tapiRes = tel_get_sim_init_info(m_pActiveSimHandler, &simStatus, NULL);

		if (tapiRes != TAPI_API_SUCCESS) {
			ERR("Failed to check sim state");
			return TELEPHONY_SIMCARD_STATE_UNKNOWN;
		}

		switch (simStatus) {
		case TAPI_SIM_STATUS_CARD_NOT_PRESENT:
		case TAPI_SIM_STATUS_CARD_REMOVED:
		case TAPI_SIM_STATUS_CARD_POWEROFF:
			return TELEPHONY_SIMCARD_STATE_REMOVED;
		case TAPI_SIM_STATUS_CARD_ERROR:
		case TAPI_SIM_STATUS_CARD_CRASHED:
			return TELEPHONY_SIMCARD_STATE_CRASHED;
		case TAPI_SIM_STATUS_CARD_BLOCKED:
			return TELEPHONY_SIMCARD_STATE_BLOCKED;
		case TAPI_SIM_STATUS_SIM_INITIALIZING:
			return TELEPHONY_SIMCARD_STATE_PROCESSING;
		default:
			return TELEPHONY_SIMCARD_STATE_READY;
		}
	}

	int TelephonyManager::getSimSlotCount()
	{
		return slotCount;
	}

	TelResultCode TelephonyManager::setActiveSlot(SimSlot slotId)
	{
		RETVM_IF (m_activeSlot == slotId, TELEPHONY_RES_SUCCESS, "Slot is already activated!")

		if (slotId == TELEPHONY_SIM_SLOT_1 && m_pSim1Handler) {
			m_pActiveSimHandler = m_pSim1Handler;
			m_activeSlot = slotId;
			return TELEPHONY_RES_SUCCESS;
		} else if (slotId == TELEPHONY_SIM_SLOT_2 && m_pSim2Handler) {
			m_pActiveSimHandler = m_pSim2Handler;
			m_activeSlot = slotId;
			return TELEPHONY_RES_SUCCESS;
		} else {
			ERR("Failed to change Sim Slot");
			return TELEPHONY_RES_FAIL_OPTION_NOT_SUPPORTED;
		}
	}

	SimSlot TelephonyManager::getActiveSlot()
	{
		return m_activeSlot;
	}

	void TelephonyManager::initTelephonyManager()
	{
		char **cpList = nullptr;
		cpList = tel_get_cp_name_list();
		RETM_IF(!cpList, "Failed to get CP list");

		m_pSim1Handler = tel_init(cpList[0]);
		free(cpList[0]);

		m_pSim2Handler = tel_init(cpList[1]);
		free(cpList[1]);

		free(cpList);
		RETM_IF(!m_pSim1Handler && !m_pSim2Handler, "Failed to init any Sim Slot");

		if (m_pSim1Handler && m_pSim2Handler && m_pSim1Handler != m_pSim2Handler) {
			INF("Dual Sim Phone detected, use Slot 1 by default");
			m_pActiveSimHandler = m_pSim1Handler;
			slotCount = 2;
		} else {
			if (m_pSim1Handler) {
				m_pActiveSimHandler = m_pSim1Handler;
				m_activeSlot = TELEPHONY_SIM_SLOT_1;
			} else {
				m_pActiveSimHandler = m_pSim2Handler;
				m_activeSlot = TELEPHONY_SIM_SLOT_2;
			}
			slotCount = 1;
		}
	}

	void TelephonyManager::deinitTelephonyManager()
	{
		if (m_pSim1Handler) {
			tel_deinit(m_pSim1Handler);
			m_pSim1Handler = nullptr;
		}

		if (m_pSim2Handler) {
			tel_deinit(m_pSim2Handler);
			m_pSim2Handler = nullptr;
		}

		m_pActiveSimHandler = nullptr;
	}

	void TelephonyManager::putRequestToQueue(TelephonyRequest *request)
	{
		request->m_pTelephonyManager = this;
		request->m_reqState = REQUEST_STATE_PENDING;
		request->m_requestId = m_requestCounter;

		m_requestQueue.push_back(request);

		if (m_requestQueue.size() == 1) {
			processNextTelephonyRequest();
		}

		m_requestCounter++;
		if (m_requestCounter == REQUEST_MAX_ID) {
			m_requestCounter = 0;
		}
	}

	void TelephonyManager::clearRequestQueue()
	{
		while (m_requestQueue.empty()) {
			TelephonyRequest *request = m_requestQueue.front();
			delete request;
			m_requestQueue.pop_front();
		}
	}

	void TelephonyManager::processNextTelephonyRequest()
	{
		if (m_requestQueue.empty()) {
			return;
		}

		TelephonyRequest *request = m_requestQueue.front();
		request->m_reqState = REQUEST_STATE_PROCESSING;
		if (request->m_reqOperation == REQUEST_OPER_TYPE_CALL_WAITING) {
			processCallWaitingRequest(request);
		} else if (request->m_reqOperation == REQUEST_OPER_TYPE_CALL_FORWARD) {
			processCallForwardRequest(request);
		}
	}

	TelResultCode TelephonyManager::convertFromTapiResultCode(TelSsCause_t tapiError)
	{
		switch (tapiError) {
		case TAPI_SS_SUCCESS:
			return TELEPHONY_RES_SUCCESS;
		case TAPI_SS_UNKNOWNSUBSCRIBER:
		case TAPI_SS_BEARERSERVICENOTPROVISIONED:
		case TAPI_SS_TELESERVICENOTPROVISIONED:
		case TAPI_SS_CALLBARRED:
		case TAPI_SS_ILLEGALSSOPERATION:
		case TAPI_SS_ERRORSTATUS:
		case TAPI_SS_FACILITYNOTSUPPORTED:
		case TAPI_SS_MAXNOMPTYEXCEEDED:
		case TAPI_SS_RESOURCESNOTAVAILABLE:
		case TAPI_SS_PWREGISTRATIONFAILURE:
		case TAPI_SS_SUBSCRIPTIONVIOLATION:
		case TAPI_SS_NOTAVAILABLE:
		case TAPI_SS_SYSTEMFAILURE:
		case TAPI_SS_REJECTEDBYNETWORK:
			return TELEPHONY_RES_FAIL_SERVICE_NOT_AVAILABLE;
		case TAPI_SS_INCOMPATIBILITY:
		case TAPI_SS_DATAMISSING:
		case TAPI_SS_UNEXPECTEDDATAVALUE:
			return TELEPHONY_RES_FAIL_DATA_MISSING;
		case TAPI_SS_REJECTEDBYUSER:
			return TELEPHONY_RES_FAIL_REJECT_BY_NETWORK;
		default:
			return TELEPHONY_RES_FAIL_UNKNOWN_ERROR;
		}
	}

	TelSsForwardWhen_t TelephonyManager::convertToTapiCallFwdType(CallFwdCondition condition)
	{
		switch (condition) {
		case TELEPHONY_CF_IF_BUSY:
			return TAPI_SS_CF_WHEN_CFB;
		case TELEPHONY_CF_IF_NO_REPLY:
			return TAPI_SS_CF_WHEN_CFNRy;
		case TELEPHONY_CF_IF_NO_REACHABLE:
			return TAPI_SS_CF_WHEN_CFNRc;
		case TELEPHONY_CF_UNCONDITIONAL:
		default:
			return TAPI_SS_CF_WHEN_CFU;
		}
	}

	CallFwdCondition TelephonyManager::convertFromTapiCallFwdType(TelSsForwardWhen_t fwdTapiCondition)
	{
		switch (fwdTapiCondition) {
		case TAPI_SS_CF_WHEN_CFB:
			return TELEPHONY_CF_IF_BUSY;
		case TAPI_SS_CF_WHEN_CFNRy:
			return TELEPHONY_CF_IF_NO_REPLY;
		case TAPI_SS_CF_WHEN_CFNRc:
			return TELEPHONY_CF_IF_NO_REACHABLE;
		case TAPI_SS_CF_WHEN_CFU:
		default:
			return TELEPHONY_CF_UNCONDITIONAL;
		}
	}

	TelSsCallWaitingMode_t TelephonyManager::convertToTapiCallWaitingMode(CallWaitingMode mode)
	{
		switch(mode) {
		case TELEPHONY_CW_MODE_ACTIVATE:
			return TAPI_SS_CW_MODE_ACTIVATE;
		case TELEPHONY_CW_MODE_DEACTIVATE:
		default:
			return TAPI_SS_CW_MODE_DEACTIVATE;
		}
	}

	CallWaitingMode TelephonyManager::convertTapiStatusToCallWaitingMode(TelSsStatus_t tapiState)
	{
		switch(tapiState) {
		case TAPI_SS_STATUS_ACTIVE:
		case TAPI_SS_STATUS_REGISTERED:
			return TELEPHONY_CW_MODE_ACTIVATE;
		case TAPI_SS_STATUS_PROVISIONED:
		case TAPI_SS_STATUS_QUIESCENT:
			return TELEPHONY_CW_MODE_DEACTIVATE;
		default:
			return TELEPHONY_CW_MODE_UNDEFINED;
		}
	}

	TelSsNoReplyTime_t TelephonyManager::convertToTapiNoReplyTime(CallFwdNoReplyTime waitTime)
	{
		switch (waitTime) {
		case TELEPHONY_CF_NO_REPLY_5_SEC:
			return TAPI_SS_CF_NO_REPLY_TIME_5_SECS;
		case TELEPHONY_CF_NO_REPLY_10_SEC:
			return TAPI_SS_CF_NO_REPLY_TIME_10_SECS;
		case TELEPHONY_CF_NO_REPLY_15_SEC:
			return TAPI_SS_CF_NO_REPLY_TIME_15_SECS;
		case TELEPHONY_CF_NO_REPLY_20_SEC:
			return TAPI_SS_CF_NO_REPLY_TIME_20_SECS;
		case TELEPHONY_CF_NO_REPLY_25_SEC:
			return TAPI_SS_CF_NO_REPLY_TIME_25_SECS;
		case TELEPHONY_CF_NO_REPLY_30_SEC:
			return TAPI_SS_CF_NO_REPLY_TIME_30_SECS;
		default:
			return TAPI_SS_CF_NO_REPLY_TIME_30_SECS;
		}
	}

	CallFwdNoReplyTime TelephonyManager::convertFromTapiNoReplyTime(TelSsNoReplyTime_t tapiWaitTime)
	{
		switch (tapiWaitTime) {
		case TAPI_SS_CF_NO_REPLY_TIME_5_SECS:
			return TELEPHONY_CF_NO_REPLY_5_SEC;
		case TAPI_SS_CF_NO_REPLY_TIME_10_SECS:
			return TELEPHONY_CF_NO_REPLY_10_SEC;
		case TAPI_SS_CF_NO_REPLY_TIME_15_SECS:
			return TELEPHONY_CF_NO_REPLY_15_SEC;
		case TAPI_SS_CF_NO_REPLY_TIME_20_SECS:
			return TELEPHONY_CF_NO_REPLY_20_SEC;
		case TAPI_SS_CF_NO_REPLY_TIME_25_SECS:
			return TELEPHONY_CF_NO_REPLY_25_SEC;
		case TAPI_SS_CF_NO_REPLY_TIME_30_SECS:
			return TELEPHONY_CF_NO_REPLY_30_SEC;
		default:
			return TELEPHONY_CF_NO_REPLY_UNDEFINED;
		}
	}

	TelSsForwardMode_t TelephonyManager::convertToTapiCallFwdMode(CallForwardMode mode)
	{
		switch (mode) {
		case TELEPHONY_CF_MODE_ACTIVATE:
			return TAPI_SS_CF_MODE_ENABLE_EV;
		case TAPI_SS_STATUS_REGISTERED:
			return TAPI_SS_CF_MODE_REGISTRATION_EV;
		case TELEPHONY_CF_MODE_DEACTIVATE:
			return TAPI_SS_CF_MODE_DISABLE_EV;
		case TELEPHONY_CF_MODE_UNDEFINED:
			return TAPI_SS_CF_MODE_ERASURE_EV;
		}
	}

	CallForwardMode TelephonyManager::convertTapiStatusToCallFwdMode(TelSsStatus_t cfStatus)
	{
		switch (cfStatus) {
		case TAPI_SS_STATUS_ACTIVE:
			return TELEPHONY_CF_MODE_ACTIVATE;
		case TAPI_SS_STATUS_REGISTERED:
			return TELEPHONY_CF_MODE_REGISTER;
		case TAPI_SS_STATUS_PROVISIONED:
		case TAPI_SS_STATUS_QUIESCENT:
			return TELEPHONY_CF_MODE_DEACTIVATE;
		default :
			return TELEPHONY_CF_MODE_UNDEFINED;
		}
	}

	TelSsClass_t TelephonyManager::convertToTapiCallClassType(CallType callClassType)
	{
		switch (callClassType) {
		case TELEPHONY_CALLTYPE_VOICE:
			return TAPI_SS_CLASS_VOICE;
		case TELEPHONY_CALLTYPE_VIDEO:
			return TAPI_SS_CLASS_ALL_CS_SYNC;
		case TELEPHONY_CALLTYPE_ALL:
			return TAPI_SS_CLASS_ALL_TELE_BEARER;
		default:
			return TAPI_SS_CLASS_VOICE;
		}
	}

	CallType TelephonyManager::convertFromTapiCallClassType(TelSsClass_t tapiClass)
	{
		switch (tapiClass) {
		case TAPI_SS_CLASS_VOICE:
			return TELEPHONY_CALLTYPE_VOICE;
		case TAPI_SS_CLASS_ALL_CS_SYNC:
			return TELEPHONY_CALLTYPE_VIDEO;
		case TAPI_SS_CLASS_ALL_TELE_BEARER:
			return TELEPHONY_CALLTYPE_ALL;
		default:
			return TELEPHONY_CALLTYPE_UNDEFINED;
		}
	}

	void TelephonyManager::responseOnSetupRequest(TapiHandle *handle, int result, void *data, void *userData)
	{
		RETM_IF(!userData, "Callback Data is NULL");

		TelephonyRequest *request = static_cast<TelephonyRequest *>(userData);
		TelephonyManager *telManager = request->m_pTelephonyManager;
		SimpleRequestListener *listener = dynamic_cast<SimpleRequestListener *>(request->m_pListener);
		if (listener->isAttached() && request->m_reqState != REQUEST_STATE_CANCELED) {
			if (result == TAPI_SS_SUCCESS) {
				listener->onRequestComplete(TELEPHONY_RES_SUCCESS);
			} else {
				listener->onRequestComplete(telManager->convertFromTapiResultCode((TelSsCause_t)result));
			}
		}

		delete request;
		telManager->m_requestQueue.pop_front();
		telManager->processNextTelephonyRequest();
	}

	void TelephonyManager::responseOnCWStatusRequest(TapiHandle *handle, int result, void *data, void *userData)
	{
		RETM_IF(!userData, "Callback Data is NULL");

		TelephonyRequest *request = static_cast<TelephonyRequest *>(userData);
		TelephonyManager *telManager = request->m_pTelephonyManager;
		CallWaitingReqData *reqCallWaitData = static_cast<CallWaitingReqData *>(request->m_pReqData);
		TelSsWaitingResp_t *cwInfo = static_cast<TelSsWaitingResp_t *>(data);
		RequestListener<CallWaitingReqData> *listener = dynamic_cast<RequestListener<CallWaitingReqData> *>(request->m_pListener);
		TelResultCode reqResult = telManager->convertFromTapiResultCode((TelSsCause_t)result);

		if (!cwInfo || !request) {
			reqResult = TELEPHONY_RES_FAIL_DATA_MISSING;
		}

		if (listener->isAttached() && request->m_reqState != REQUEST_STATE_CANCELED) {

			if (reqResult == TELEPHONY_RES_SUCCESS) {
				reqResult = parseCWStatusRequestResponse(cwInfo, request);
			}

			listener->onRequestComplete(reqResult, reqCallWaitData);
		}

		delete request;
		telManager->m_requestQueue.pop_front();
		telManager->processNextTelephonyRequest();
	}

	TelResultCode TelephonyManager::parseCWStatusRequestResponse(TelSsWaitingResp_t *cwInfo, TelephonyRequest *request)
	{
		CallWaitingReqData *reqCallWaitData = static_cast<CallWaitingReqData *>(request->m_pReqData);
		TelephonyManager *telManager =  request->m_pTelephonyManager;
		for (auto i = 0; i < cwInfo->record_num; i++) {
			if (reqCallWaitData->callType != telManager->convertFromTapiCallClassType(cwInfo->record[i].Class)) {
				continue;
			}
			reqCallWaitData->mode = telManager->convertTapiStatusToCallWaitingMode(cwInfo->record[i].Status);
			return TELEPHONY_RES_SUCCESS;
		}
		reqCallWaitData->mode = TELEPHONY_CW_MODE_UNDEFINED;
		return TELEPHONY_RES_FAIL_DATA_MISSING;
	}

	void TelephonyManager::responseOnCFStatusRequest(TapiHandle *handle, int result, void *data, void *userData)
	{
		RETM_IF(!userData, "Callback Data is NULL");

		TelephonyRequest *request = static_cast<TelephonyRequest *>(userData);
		TelephonyManager *telManager = request->m_pTelephonyManager;
		CallFwdReqData *reqCallFwdData = static_cast<CallFwdReqData *>(request->m_pReqData);
		TelSsForwardResp_t *cfInfo = static_cast<TelSsForwardResp_t *>(data);
		RequestListener<CallFwdReqData> *listener = dynamic_cast<RequestListener<CallFwdReqData> *>(request->m_pListener);
		TelResultCode reqResult = telManager->convertFromTapiResultCode((TelSsCause_t)result);

		if (!cfInfo || !request) {
			reqResult = TELEPHONY_RES_FAIL_DATA_MISSING;
		}

		if (listener->isAttached() && request->m_reqState != REQUEST_STATE_CANCELED) {

			if (reqResult == TELEPHONY_RES_SUCCESS) {
				reqResult = parseCFStatusRequestResponse(cfInfo, request);
			}

			listener->onRequestComplete(reqResult, reqCallFwdData);
		}

		delete request;
		telManager->m_requestQueue.pop_front();
		telManager->processNextTelephonyRequest();
	}

	TelResultCode TelephonyManager::parseCFStatusRequestResponse(TelSsForwardResp_t *cfInfo, TelephonyRequest *request)
	{
		CallFwdReqData *reqCallFwdData = static_cast<CallFwdReqData *>(request->m_pReqData);
		TelephonyManager *telManager =  request->m_pTelephonyManager;

		for (auto i = 0; i < cfInfo->record_num; i++) {
			if (reqCallFwdData->callType != telManager->convertFromTapiCallClassType(cfInfo->record[i].Class)) {
				continue;
			}

			if(reqCallFwdData->condition != telManager->convertFromTapiCallFwdType(cfInfo->record[i].ForwardCondition)) {
				continue;
			}

			if (reqCallFwdData->condition == TELEPHONY_CF_IF_NO_REPLY) {
				reqCallFwdData->waitTime = request->m_pTelephonyManager->convertFromTapiNoReplyTime(cfInfo->record[i].NoReplyWaitTime);
			} else {
				reqCallFwdData->waitTime = TELEPHONY_CF_NO_REPLY_UNDEFINED;
			}

			reqCallFwdData->mode = telManager->convertTapiStatusToCallFwdMode(cfInfo->record[i].Status);
			if (cfInfo->record[i].bCallForwardingNumberPresent == 1) {
				reqCallFwdData->telNumber.assign((char *)cfInfo->record[i].szCallForwardingNumber);
			} else {
				reqCallFwdData->telNumber.assign("undefined");
			}

			return TELEPHONY_RES_SUCCESS;
		}

		reqCallFwdData->mode = TELEPHONY_CF_MODE_UNDEFINED;
		reqCallFwdData->telNumber.assign("undefined");
		reqCallFwdData->waitTime = TELEPHONY_CF_NO_REPLY_UNDEFINED;

		return TELEPHONY_RES_FAIL_DATA_MISSING;
	}

	void TelephonyManager::processCallForwardRequest(TelephonyRequest *request)
	{
		int res = TAPI_API_SUCCESS;
		CallFwdReqData *reqCallFwdData = static_cast<CallFwdReqData *>(request->m_pReqData);
		TelSsClass_t callClass = convertToTapiCallClassType(reqCallFwdData->callType);
		TelSsForwardWhen_t forwardCondition = convertToTapiCallFwdType(reqCallFwdData->condition);

		if (request->m_reqType == REQUEST_TYPE_GET_STATE) {
			res = tel_get_ss_forward_status(request->m_pSimHandler, callClass, forwardCondition, responseOnCFStatusRequest, request);
			if (res != TAPI_API_SUCCESS) {
				dynamic_cast<RequestListener<CallFwdReqData> *>(request->m_pListener)->onRequestComplete(TELEPHONY_RES_FAIL_NETWORK_API_FAILED, reqCallFwdData);
			}
		} else if (request->m_reqType == REQUEST_TYPE_SETUP) {
			TelSsForwardInfo_t cfInfo;
			memset(&cfInfo, 0x0, sizeof(TelSsForwardInfo_t));
			cfInfo.Class = callClass;
			cfInfo.Condition = forwardCondition;
			cfInfo.Mode = convertToTapiCallFwdMode(reqCallFwdData->mode);
			if (reqCallFwdData->condition == TELEPHONY_CF_IF_NO_REPLY && reqCallFwdData->waitTime != TELEPHONY_CF_NO_REPLY_UNDEFINED) {
				cfInfo.NoReplyConditionTimer = convertToTapiNoReplyTime(reqCallFwdData->waitTime);
			}

			std::snprintf((char *)cfInfo.szPhoneNumber, TAPI_CALL_DIALDIGIT_LEN_MAX, "%s", reqCallFwdData->telNumber.c_str());
			if (cfInfo.szPhoneNumber[0] == '+') {
				cfInfo.Ton = TAPI_SS_CF_TON_INTERNATIONAL;
			} else {
				cfInfo.Ton = TAPI_SS_CF_TON_NATIONAL;
			}

			res = tel_set_ss_forward(request->m_pSimHandler, &cfInfo, responseOnSetupRequest, request);
			if (res != TAPI_API_SUCCESS) {
				dynamic_cast<SimpleRequestListener *>(request->m_pListener)->onRequestComplete(TELEPHONY_RES_FAIL_NETWORK_API_FAILED);
			}
		}

		if (res != TAPI_API_SUCCESS) {
			delete request;
			m_requestQueue.pop_front();
			processNextTelephonyRequest();
		}
	}

	void TelephonyManager::processCallWaitingRequest(TelephonyRequest *request)
	{
		int res = TAPI_API_SUCCESS;
		CallWaitingReqData *reqCallWaitingData = static_cast<CallWaitingReqData *>(request->m_pReqData);
		TelSsClass_t callClass = convertToTapiCallClassType(reqCallWaitingData->callType);

		if (request->m_reqType == REQUEST_TYPE_GET_STATE) {
			res = tel_get_ss_waiting_status(request->m_pSimHandler, callClass, responseOnCWStatusRequest, (void *)request);
		} else if (request->m_reqType == REQUEST_TYPE_SETUP) {
			TelSsWaitingInfo_t cwInfo;
			memset(&cwInfo, 0x0, sizeof(TelSsForwardInfo_t));
			cwInfo.Class = callClass;
			cwInfo.Mode = convertToTapiCallWaitingMode(reqCallWaitingData->mode);
			res = tel_set_ss_waiting(request->m_pSimHandler, &cwInfo, responseOnSetupRequest, (void *)request);
		}

		if (res != TAPI_API_SUCCESS) {
			delete request;
			m_requestQueue.pop_front();
			processNextTelephonyRequest();
		}
	}

} }
