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

namespace AnsweringController {

	using namespace Model::Settings;

	AnswerViewController::AnswerViewController (App::Application &core, NotifyHandler handler) :
		ViewController(core, handler),
		m_appCore(core),
		m_pAnswerView(nullptr),
		m_needUpdateAnswering(true),
		m_needUpdateRejection(true)
	{
	}

	AnswerViewController::~AnswerViewController()
	{
		SettingsManager &settingsManager = m_appCore.getSettingsManager();

		settingsManager.removePropertyHandler(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionChanged>(this));

		settingsManager.removePropertyHandler(BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionChanged>(this));
	}

	bool AnswerViewController::initialize()
	{

		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		SettingsManager &settingsManager = m_appCore.getSettingsManager();

		ResultCode res = settingsManager.addPropertyHandler(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionChanged>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup answer option listener");

		res = settingsManager.addPropertyHandler(BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionChanged>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup rejecting option listener");

		m_pAnswerView = View::ViewManager::pushView<AnswerView::AnswerView>(m_appCore.getViewManager(), true);
		RETVM_IF(!m_pAnswerView, false, "Failed to create view instance!");
		setBaseView(m_pAnswerView);

		m_pAnswerView->setAnswerCheckHandler(
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionCheck>(this));

		m_pAnswerView->setRejectCheckHandler(
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionCheck>(this));

		m_pAnswerView->setAnswerItemClickHandler(
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerItemClick>(this));

		m_pAnswerView->setRejectItemClickHandler(
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onRejectItemClick>(this));

		return true;
	}

	void AnswerViewController::onAnswerOptionCheck()
	{
		bool state = m_pAnswerView->getAnswerOptionState();
		SettingsManager &settingsManager = m_appCore.getSettingsManager();
		ResultCode res = settingsManager.setProperty(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY, state);
		if (res != SETTINGS_RES_SUCCESS) {
			ERR("Failed to change Answer option");
			updateRejectOption();
		}
	}

	void AnswerViewController::onRejectOptionCheck()
	{
		bool state = m_pAnswerView->getRejectOptionState();
		SettingsManager &settingsManager = m_appCore.getSettingsManager();
		ResultCode res = settingsManager.setProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, state);
		if (res != SETTINGS_RES_SUCCESS) {
			ERR("Failed to change Reject option");
			updateAnswerOption();
		}
	}

	void AnswerViewController::onAnswerItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		bool state = m_pAnswerView->getAnswerOptionState();

		SettingsManager &settingsManager = m_appCore.getSettingsManager();
		settingsManager.setProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, !state);
	}

	void AnswerViewController::onRejectItemClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		bool state = m_pAnswerView->getRejectOptionState();

		SettingsManager &settingsManager = m_appCore.getSettingsManager();
		settingsManager.setProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, state);
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
		SettingsManager &settingsManager = m_appCore.getSettingsManager();
		ResultCode res = settingsManager.getProperty(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY, isEnabled);
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
		SettingsManager &settingsManager = m_appCore.getSettingsManager();
		ResultCode res = settingsManager.getProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, isEnabled);
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
}
