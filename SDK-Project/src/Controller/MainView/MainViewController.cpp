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

namespace CallSettings { namespace Controller {

	using namespace gui;

	MainViewController::MainViewController (Application &app, NotiHandler handler) :
			ViewController(app, handler),
			m_app(app),
			m_pRejectListViewController(nullptr),
			m_pAnswerViewController(nullptr),
			m_pMoreViewController(nullptr),
			m_pMainView(nullptr)
	{
	}

	MainViewController::~MainViewController()
	{
		m_pMainView->setRejectMsgHandler(nullptr);
		m_pMainView->setCallAceptHandler(nullptr);
		m_pMainView->setMoreHandler(nullptr);
		delete m_pRejectListViewController;
		delete m_pAnswerViewController;
		delete m_pMoreViewController;

	}

	bool MainViewController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		m_pMainView = ViewManager::pushView<View::MainView>(m_app.getViewManager(), true);
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

		m_pRejectListViewController = ViewController::create<Controller::RejectMsgListController>(m_app,
				NotiHandler::wrap<MainViewController, &MainViewController::onRejectMsgControllerDestroy>(this));
	}

	void MainViewController::onCallAnsweringClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		DBG("Answer/End call option selected");

		m_pAnswerViewController = ViewController::create<Controller::AnswerViewController>(m_app,
				NotiHandler::wrap<MainViewController, &MainViewController::onAnswerControllerDestroy>(this));
	}

	void MainViewController::onMoreClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		DBG("More option selected");
		m_pMoreViewController = ViewController::create<MoreViewController>(m_app,
				NotiHandler::wrap<MainViewController, &MainViewController::onMoreControllerDestroy>(this));
	}

	void MainViewController::onAnswerControllerDestroy()
	{
		delete m_pAnswerViewController;
		m_pAnswerViewController = nullptr;
	}


	void MainViewController::onMoreControllerDestroy()
	{
		delete m_pMoreViewController;
		m_pMoreViewController = nullptr;
	}

	void MainViewController::onRejectMsgControllerDestroy()
	{
		delete m_pRejectListViewController;
		m_pRejectListViewController = nullptr;
	}

} }
