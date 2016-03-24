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
#include "View/MainView/MainView.h"
#include "View/ViewManager/ViewManager.h"

namespace Controller {

	MainViewController::MainViewController (App::AppCore &core, DestroyRequestHandler handler) :
			ViewController(core, handler)
	{
	}

	bool MainViewController::createView()
	{
		m_pView = ViewManager::pushView<MainView::MainView>(m_Core.getViewManager(), true);
		RETVM_IF(!m_pView, false, "Failed to create MainView");

		MainView::MainView *view = dynamic_cast<MainView::MainView *>(m_pView);
		view->setRejectMsgHandler(makeNotifyHandler(MainViewController, onRejectMsgClick, this));
		view->setCallAceptHandler(makeNotifyHandler(MainViewController, onCallAceptClick, this));
		view->setMoreHandler(makeNotifyHandler(MainViewController, onMoreClick, this));

		return true;
	}

	void MainViewController::onBackKeyPressed()
	{
		if (m_destroyRequestHandler.assigned()) {
			m_destroyRequestHandler(this);
		}
	}

	/*TODO Child view creation will be added here */
	void MainViewController::onRejectMsgClick()
	{
		DBG("Reject message option selected");
	}

	void MainViewController::onCallAceptClick()
	{
		DBG("Answer/End call option selected");
	}

	void MainViewController::onMoreClick()
	{
		DBG("More option selected");
	}
}
