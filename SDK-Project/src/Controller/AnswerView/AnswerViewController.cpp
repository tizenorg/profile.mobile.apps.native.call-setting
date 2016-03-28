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
#include "Model/Settings/SettingsManager.h"

namespace AnsweringController {

	using namespace Model::Settings;

	AnswerViewController::AnswerViewController (App::AppCore &core, DestroyRequestHandler handler) :
		ViewController(core, handler),
		m_pAnswerView(nullptr),
		m_isAnswerByHomeSet(false),
		m_isRejectByPowerSet(false)
	{
	}

	AnswerViewController::~AnswerViewController()
	{
		SettingsManager &settingsManager = m_Core.getSettingsManager();

		settingsManager.removePropertyHandler(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
								BoolPropertyHandler::wrap<AnswerViewController, &AnswerViewController::answerOptionHandler>(this));

		settingsManager.removePropertyHandler(BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
								BoolPropertyHandler::wrap<AnswerViewController, &AnswerViewController::rejectOptionHandler>(this));
	}

	bool AnswerViewController::onInitizlize()
	{
		SettingsManager &settingsManager = m_Core.getSettingsManager();

		ResultCode res = settingsManager.getBoolProperty(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY, m_isAnswerByHomeSet);
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to get answering option state");

		res = settingsManager.getBoolProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, m_isRejectByPowerSet);
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to get rejecting option state");

		res = settingsManager.addPropertyHandler(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY,
				BoolPropertyHandler::wrap<AnswerViewController, &AnswerViewController::answerOptionHandler>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup answer option listener");

		res = settingsManager.addPropertyHandler(BOOL_KEY_CALL_ENDING_BY_POWER_KEY,
						BoolPropertyHandler::wrap<AnswerViewController, &AnswerViewController::rejectOptionHandler>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup rejecting option listener");

		return true;
	}

	BaseView *AnswerViewController::createView()
	{
		m_pAnswerView = ViewManager::pushView<AnswerView::AnswerView>(m_Core.getViewManager(), true);
		RETVM_IF(!m_pAnswerView, nullptr, "Failed to create view instance!");

		m_pAnswerView->setAnswerOptionHandler(
				NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onAnswerOptionClick>(this));

		m_pAnswerView->setRejectOptionHandler(
						NotifyHandler::wrap<AnswerViewController, &AnswerViewController::onRejectOptionClick>(this));

		return m_pAnswerView;
	}

	void AnswerViewController::onAnswerOptionClick()
	{
		SettingsManager &settingsManager = m_Core.getSettingsManager();
		ResultCode res = settingsManager.setBoolProperty(BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY, !m_isAnswerByHomeSet);
		if (res != SETTINGS_RES_SUCCESS) {
			m_pAnswerView->checkAnswerOption(m_isAnswerByHomeSet);
			ERR("Failed to change Answer option");
		}

	}

	void AnswerViewController::onRejectOptionClick()
	{
		SettingsManager &settingsManager = m_Core.getSettingsManager();
		ResultCode res = settingsManager.setBoolProperty(BOOL_KEY_CALL_ENDING_BY_POWER_KEY, !m_isRejectByPowerSet);
		if (res != SETTINGS_RES_SUCCESS) {
			m_pAnswerView->checkRejectOption(m_isAnswerByHomeSet);
			ERR("Failed to change Reject option");
		}
	}

	void AnswerViewController::answerOptionHandler(bool isEnabled)
	{
		m_isAnswerByHomeSet = isEnabled;
	}

	void AnswerViewController::rejectOptionHandler(bool isEnabled)
	{
		m_isRejectByPowerSet = isEnabled;
	}

	void AnswerViewController::onBackKeyPressed()
	{
		makeDestroyReqeuest();
	}

	void AnswerViewController::onUpdate(int updateFlag)
	{
		if ((updateFlag & UPDATE_INITIAL) || (updateFlag & UPDATE_WAS_PAUSED)) {
			m_pAnswerView->checkAnswerOption(m_isAnswerByHomeSet);
			m_pAnswerView->checkRejectOption(m_isRejectByPowerSet);
		}
	}
}
