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

#include "Controller/MainView/MainViewController.h"

namespace MainController {

	using namespace App;
	using namespace View;

	MainViewController::MainViewController (AppCore &core, NotiHandler handler) :
			ViewController(core, handler),
			m_appCore(core),
			m_pMainView(nullptr)
	{
	}

	MainViewController::~MainViewController()
	{
		m_pMainView->setRejectMsgHandler(nullptr);
		m_pMainView->setCallAceptHandler(nullptr);
		m_pMainView->setMoreHandler(nullptr);
	}

	bool MainViewController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		m_pMainView = ViewManager::pushView<MainView::MainView>(m_appCore.getViewManager(), true);
		RETVM_IF(!m_pMainView, false, "Failed to create view");

		setBaseView(m_pMainView);
		return true;
	}

	void MainViewController::updateView(int updateFlag)
	{
		if (updateFlag & UF_INITIAL) {
			m_pMainView->setRejectMsgHandler(
					NotiHandler::wrap<MainViewController, &MainViewController::onRejectMsgClick>(this));
			m_pMainView->setCallAceptHandler(
					NotiHandler::wrap<MainViewController, &MainViewController::onCallAnsweringClick>(this));
			m_pMainView->setMoreHandler(
					NotiHandler::wrap<MainViewController, &MainViewController::onMoreClick>(this));
		}
	}

	void MainViewController::onBackKeyPressed()
	{
		makeDestroyReqeuest();
	}

	void MainViewController::onRejectMsgClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		DBG("Reject message option selected");

	}

	void MainViewController::onCallAnsweringClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		DBG("Answer/End call option selected");
	}

	void MainViewController::onMoreClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		DBG("More option selected");
	}

	void MainViewController::onAnswerControllerDestroy()
	{
	}

}
