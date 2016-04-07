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

#include "Controller/MoreView/MoreViewController.h"

namespace CallSettings { namespace Controller {

	using namespace Model;


	MoreViewController::MoreViewController (Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pMoreView(nullptr),
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

		m_pMoreView = gui::ViewManager::pushView<View::MoreView>(m_app.getViewManager(), true);
		RETVM_IF(!m_pMoreView, false, "Failed to create view instance!");
		setBaseView(m_pMoreView);

		m_pMoreView->setCallerIdClickHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onCallerIdItemClick>(this));

		m_pMoreView->setCallForwardClickHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onForwardingItemClick>(this));

		m_pMoreView->setCallWaitingClickHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onWaitingItemClick>(this));

		m_pMoreView->setCallWaitingCheckHandler(
				NotiHandler::wrap<MoreViewController, &MoreViewController::onWaitingOptionCheck>(this));

		return true;
	}

	void MoreViewController::onCallerIdItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		//TODO Caller Id popup will be added here
		DBG("Caller Id option selected");
	}

	void MoreViewController::onForwardingItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		//TODO Caller Id popup will be added here
		DBG("Call forwarding option selected");
	}

	void MoreViewController::onWaitingItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		DBG("Call Waiting option selected");
		if (!m_waitRequestPending) {
			bool state = m_pMoreView->getWaitingOptionState();
			setupWaitingOption(!state);
		}
	}

	void MoreViewController::onWaitingOptionCheck()
	{
		DBG("Call forwarding option checked");
		if (!m_waitRequestPending) {
			bool state = m_pMoreView->getWaitingOptionState();
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
			m_pMoreView->setCallerIdValue(-1);
		} else {
			m_pMoreView->setCallerIdValue(callerId);
		}
	}

	void MoreViewController::updateWaitingOption()
	{
		m_needUpdateWaiting = false;
		m_callWaitData.callType = TELEPHONY_CALLTYPE_VOICE;
		m_callWaitData.mode = TELEPHONY_CW_MODE_UNDEFINED;

		m_pMoreView->setWaitingOptionPending(true);
		m_waitRequestPending = true;
		m_app.getTelephonyManager().requestCallWaitState(&m_callWaitData, this);
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
		m_app.getTelephonyManager().requestCallWaitSetup(&m_callWaitData, this);
	}

	void MoreViewController::onBackKeyPressed()
	{
		makeDestroyReqeuest();
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
			m_pMoreView->checkWaitingOption(false);
			ERR("Failed to get Call Waiting state!");
		} else {
			if (data->mode == TELEPHONY_CW_MODE_ACTIVATE) {
				m_pMoreView->checkWaitingOption(true);
			} else {
				m_pMoreView->checkWaitingOption(false);
			}
		}
	}

	void MoreViewController::onRequestComplete(TelResultCode result)
	{
		m_waitRequestPending = false;
		if (result != TELEPHONY_RES_SUCCESS) {
			m_pMoreView->setWaitingOptionPending(false);
			m_pMoreView->checkWaitingOption(false);
			ERR("Failed to setup Call Waiting state!");
		}
	}
} }
