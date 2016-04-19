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

#include "Controller/AnswerView/AnswerViewController.h"

namespace CallSettings { namespace Controller {

	using namespace Model;

	AnswerViewController::AnswerViewController (Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pAnswerView(nullptr),
		m_needUpdateAnswering(true),
		m_needUpdateRejection(true)
	{
	}

	AnswerViewController::~AnswerViewController()
	{
		ISettingsManager &settingsManager = m_app.getSettingsManager();

		settingsManager.removePropertyHandler(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
				NotiHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionChanged>(this));

		settingsManager.removePropertyHandler(BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
				NotiHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionChanged>(this));
	}

	bool AnswerViewController::initialize()
	{

		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		ISettingsManager &settingsManager = m_app.getSettingsManager();


		SettingResultCode res = settingsManager.addPropertyHandler(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
				NotiHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionChanged>(this));

		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup answer option listener");

		res = settingsManager.addPropertyHandler(BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
				NotiHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionChanged>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup rejecting option listener");

		m_pAnswerView =  gui::ViewManager::pushView<View::AnswerView>(m_app.getViewManager(), true);
		RETVM_IF(!m_pAnswerView, false, "Failed to create view instance!");
		setBaseView(m_pAnswerView);

		m_pAnswerView->setAnswerCheckHandler(
				NotiHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionCheck>(this));

		m_pAnswerView->setRejectCheckHandler(
				NotiHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionCheck>(this));

		return true;
	}

	void AnswerViewController::onAnswerOptionCheck()
	{
		DBG("onAnswerOptionCheck");
		bool state = m_pAnswerView->getAnswerOptionState();
		ISettingsManager &settingsManager = m_app.getSettingsManager();
		SettingResultCode res = settingsManager.setProperty(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY, state);
		if (res != SETTINGS_RES_SUCCESS) {
			ERR("Failed to change Answer option");
			updateAnswerOption();
		}
	}

	void AnswerViewController::onRejectOptionCheck()
	{
		DBG("onRejectOptionCheck");
		bool state = m_pAnswerView->getRejectOptionState();
		ISettingsManager &settingsManager = m_app.getSettingsManager();
		SettingResultCode res = settingsManager.setProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, state);
		if (res != SETTINGS_RES_SUCCESS) {
			ERR("Failed to change Reject option");
			updateRejectOption();
		}
	}

	void AnswerViewController::onAnswerOptionChanged()
	{
		if (m_isVisible) {
			updateAnswerOption();
		} else {
			m_needUpdateAnswering = true;
		}
	}

	void AnswerViewController::onRejectOptionChanged()
	{
		if (m_isVisible) {
			updateRejectOption();
		} else {
			m_needUpdateRejection = true;
		}
	}

	void AnswerViewController::updateAnswerOption()
	{
		m_needUpdateAnswering = false;
		bool isEnabled = false;
		ISettingsManager &settingsManager = m_app.getSettingsManager();
		SettingResultCode res = settingsManager.getProperty(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY, isEnabled);
		if (res != SETTINGS_RES_SUCCESS) {
			m_pAnswerView->checkAnswerOption(false);
		} else {
			m_pAnswerView->checkAnswerOption(isEnabled);
		}

	}

	void AnswerViewController::updateRejectOption()
	{
		m_needUpdateRejection = false;
		bool isEnabled = false;
		ISettingsManager &settingsManager = m_app.getSettingsManager();
		SettingResultCode res = settingsManager.getProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, isEnabled);
		if (res != SETTINGS_RES_SUCCESS) {
			m_pAnswerView->checkRejectOption(false);
		} else {
			m_pAnswerView->checkRejectOption(isEnabled);
		}
	}

	void AnswerViewController::onBackKeyPressed()
	{
		makeDestroyReqeuest();
	}

	void AnswerViewController::updateView(int updateFlag)
	{
			if (m_needUpdateAnswering) {
				updateAnswerOption();
			}

			if (m_needUpdateRejection) {
				updateRejectOption();
			}
	}
} }
