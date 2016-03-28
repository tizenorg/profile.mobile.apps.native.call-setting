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
#include "View/ViewManager/ViewManager.h"

namespace MainController {

	MainViewController::MainViewController (App::AppCore &core, DestroyRequestHandler handler) :
			ViewController(core, handler),
			m_pAnswerCallController(nullptr),
			m_pMainView(nullptr)
	{
	}

	BaseView *MainViewController::createView()
	{
		m_pMainView = ViewManager::pushView<MainView::MainView>(m_Core.getViewManager(), true);
		return m_pMainView;
	}

	void MainViewController::onUpdate(int updateFlag)
	{
		if (updateFlag & UPDATE_INITIAL) {
			m_pMainView->setRejectMsgHandler(NotifyHandler::wrap<MainViewController, &MainViewController::onRejectMsgClick>(this));
			m_pMainView->setCallAceptHandler(NotifyHandler::wrap<MainViewController, &MainViewController::onCallAnsweringClick>(this));
			m_pMainView->setMoreHandler(NotifyHandler::wrap<MainViewController, &MainViewController::onMoreClick>(this));
		}
	}

	void MainViewController::onBackKeyPressed()
	{
		makeDestroyReqeuest();
	}

	/*TODO Child view creation will be added here */
	void MainViewController::onRejectMsgClick()
	{
		DBG("Reject message option selected");
	}

	void MainViewController::onCallAnsweringClick()
	{
		DBG("Answer/End call option selected");
		m_pAnswerCallController = ViewController::create<AnsweringController::AnswerViewController>(m_Core,
				makeHandler(DestroyRequestHandler, MainViewController, destroyRequestHandler, this));
	}

	void MainViewController::onMoreClick()
	{
		DBG("More option selected");
	}

	void MainViewController::destroyRequestHandler(ViewController *controller)
	{
		if (controller == m_pAnswerCallController) {
			m_pAnswerCallController = nullptr;
		}

		delete controller;
	}

}
