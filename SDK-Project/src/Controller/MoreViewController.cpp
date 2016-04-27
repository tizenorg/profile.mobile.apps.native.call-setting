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

#include "Controller/MoreViewController.h"

namespace CallSettings { namespace Controller {

	using namespace Model;
	using namespace View;

	MoreViewController::MoreViewController (Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pMoreView(nullptr),
		m_pCallFwController(nullptr),
		m_needUpdateCallerId(true),
		m_needUpdateWaiting(true),
		m_waitRequestPending(false)
	{
	}

	MoreViewController::~MoreViewController()
	{
		m_app.getSettingsManager().removePropertyHandler(INT_KEY_SHOW_CALLER_ID,
				NotiHandler::wrap<MoreViewController, &MoreViewController::onCallerIdOptionChanged>(this));

		m_app.getTelephonyManager().removeCallWaitChangeHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onWaitingOptionChanged>(this));

		m_pMoreView->setCallerIdClickHandler(nullptr);
		m_pMoreView->setCallForwardClickHandler(nullptr);
		m_pMoreView->setCallWaitingCheckHandler(nullptr);

		delete m_pCallFwController;
	}

	bool MoreViewController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		SettingResultCode settingsRes = m_app.getSettingsManager().addPropertyHandler(INT_KEY_SHOW_CALLER_ID,
				NotiHandler::wrap<MoreViewController, &MoreViewController::onCallerIdOptionChanged>(this));
		RETVM_IF(settingsRes != SETTINGS_RES_SUCCESS, false, "Failed to setup answer option listener");

		TelResultCode telRes = m_app.getTelephonyManager().addCallWaitChangeHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onWaitingOptionChanged>(this));
		RETVM_IF(telRes != TELEPHONY_RES_SUCCESS, false, "Failed to setup call waiting option listener");

		m_pMoreView = m_app.getViewManager().pushView<View::MoreView>();
		RETVM_IF(!m_pMoreView, false, "Failed to create view instance!");
		setBaseView(m_pMoreView);

		m_pMoreView->setCallerIdClickHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onCallerIdItemClick>(this));

		m_pMoreView->setCallForwardClickHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onForwardingItemClick>(this));

		m_pMoreView->setCallWaitingCheckHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onWaitingOptionCheck>(this));

		return true;
	}

	void MoreViewController::onCallerIdItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		int callerId = -1;
		SettingResultCode res = m_app.getSettingsManager().getProperty(INT_KEY_SHOW_CALLER_ID, callerId);
		RETM_IF(res != SETTINGS_RES_SUCCESS, "Caller Id state is undefined!");

		m_isActivated = false;
		m_pMoreView->showCallerIdPopup((MoreView::CallerIdStatus)callerId,
				MoreView::CallerIdStatusChangeHandler::wrap<MoreViewController, &MoreViewController::onCallerIdStatusSelected>(this),
				NotiHandler::wrap<MoreViewController, &MoreViewController::onCallerIdPopupHide>(this));
	}

	void MoreViewController::onCallerIdPopupHide()
	{
		m_isActivated = true;
	}

	void MoreViewController::onCallerIdStatusSelected(MoreView::CallerIdStatus value)
	{
		int newCallerIdState = 0;

		if (value == MoreView::CALLER_ID_STATUS_SHOW) {
			newCallerIdState = 1;
		} else if (value == MoreView::CALLER_ID_STATUS_HIDE) {
			newCallerIdState = 2;
		}

		m_app.getSettingsManager().setProperty(INT_KEY_SHOW_CALLER_ID, newCallerIdState);
		m_pMoreView->hideCallerIdPopup();
	}

	void MoreViewController::onForwardingItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		DBG("Call forwarding option selected");
		m_pCallFwController = ViewController::create<CallFwController>(m_app,
				NotiHandler::wrap<MoreViewController, &MoreViewController::onCallFwControllerDestroy>(this));
	}

	void MoreViewController::onCallFwControllerDestroy()
	{
		delete m_pCallFwController;
		m_pCallFwController = nullptr;
	}

	void MoreViewController::onWaitingOptionCheck()
	{
		DBG("Call forwarding option checked");
		if (!m_waitRequestPending) {
			bool state = m_pMoreView->isWaitingOptionChecked();
			setupWaitingOption(state);
		}
	}

	void MoreViewController::onCallerIdOptionChanged()
	{
		if (m_isVisible) {
			updateCallerIdOption();
		} else {
			m_needUpdateCallerId = true;
		}
	}

	void MoreViewController::onWaitingOptionChanged()
	{
		if (m_isVisible) {
			updateWaitingOption();
		} else {
			m_needUpdateWaiting = true;
		}
	}

	void MoreViewController::updateCallerIdOption()
	{
		m_needUpdateCallerId = false;
		int callerId = -1;
		ISettingsManager &settingsManager = m_app.getSettingsManager();
		SettingResultCode res = settingsManager.getProperty(INT_KEY_SHOW_CALLER_ID, callerId);
		if (res != SETTINGS_RES_SUCCESS) {
			m_pMoreView->setCallerIdStatus(MoreView::CALLER_ID_STATUS_DEFAULT);
			return;
		}

		switch(callerId) {
		case 1:
			m_pMoreView->setCallerIdStatus(MoreView::CALLER_ID_STATUS_SHOW);
			break;
		case 2:
			m_pMoreView->setCallerIdStatus(MoreView::CALLER_ID_STATUS_HIDE);
			break;
		default:
			m_pMoreView->setCallerIdStatus(MoreView::CALLER_ID_STATUS_DEFAULT);
			break;
		}
	}

	void MoreViewController::updateWaitingOption()
	{
		m_needUpdateWaiting = false;
		m_callWaitData.callType = TELEPHONY_CALLTYPE_VOICE;
		m_callWaitData.mode = TELEPHONY_CW_MODE_UNDEFINED;

		m_pMoreView->setWaitingOptionPending(true);
		m_waitRequestPending = true;
		TelResultCode res = m_app.getTelephonyManager().requestCallWaitState(&m_callWaitData, this);
		RETM_IF(res != TELEPHONY_RES_SUCCESS, "Failed to send state request, error code: %d", res);
	}

	void MoreViewController::setupWaitingOption(bool state)
	{
		m_callWaitData.callType = TELEPHONY_CALLTYPE_VOICE;
		if (state) {
			m_callWaitData.mode = TELEPHONY_CW_MODE_ACTIVATE;
		} else {
			m_callWaitData.mode = TELEPHONY_CW_MODE_DEACTIVATE;
		}

		m_pMoreView->setWaitingOptionPending(true);
		m_waitRequestPending = true;
		TelResultCode res = m_app.getTelephonyManager().requestCallWaitSetup(&m_callWaitData, this);
		RETM_IF(res != TELEPHONY_RES_SUCCESS, "Failed to send setup request, error code: %d", res);
	}

	void MoreViewController::updateView(int updateFlag)
	{
		if (m_needUpdateCallerId) {
			updateCallerIdOption();
		}

		if (m_needUpdateWaiting) {
			updateWaitingOption();
		}
	}

	void MoreViewController::onRequestComplete(TelResultCode result, const CallWaitingReqData *data)
	{
		m_pMoreView->setWaitingOptionPending(false);
		m_waitRequestPending = false;
		if (result != TELEPHONY_RES_SUCCESS) {
			m_pMoreView->setWaitingOptionChecked(false);
			ERR("Failed to get Call Waiting state!");
		} else {
			if (data->mode == TELEPHONY_CW_MODE_ACTIVATE) {
				m_pMoreView->setWaitingOptionChecked(true);
			} else {
				m_pMoreView->setWaitingOptionChecked(false);
			}
		}
	}

	void MoreViewController::onRequestComplete(TelResultCode result)
	{
		m_waitRequestPending = false;
		if (result != TELEPHONY_RES_SUCCESS) {
			m_pMoreView->setWaitingOptionChecked(false);
			m_pMoreView->setWaitingOptionPending(false);
			ERR("Failed to setup Call Waiting state!");
		}
	}
} }
