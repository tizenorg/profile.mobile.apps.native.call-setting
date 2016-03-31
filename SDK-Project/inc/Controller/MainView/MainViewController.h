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

#ifndef MAIN_VIEW_CONTROLLER_H_
#define MAIN_VIEW_CONTROLLER_H_

#include "App/AppCore.h"
#include "Controller/ViewController.h"
#include "Controller/AnswerView/AnswerViewController.h"
#include "View/MainView/MainView.h"

namespace MainController {

	class MainViewController : public Controller::ViewController {
	protected:
		virtual ~MainViewController();

	private:
		friend class ViewController;

		MainViewController(App::AppCore &core, NotifyHandler handler);
		virtual void onBackKeyPressed();
		virtual void updateView(int updateFlag);

		bool initialize();
		void onRejectMsgClick();
		void onCallAnsweringClick();
		void onMoreClick();
		void onAnswerControllerDestroy();


	private:
		App::AppCore &m_appCore;
		MainView::MainView *m_pMainView;
		AnsweringController::AnswerViewController *m_pAnswerCallController;
	};
}

#endif /* MAIN_VIEW_CONTROLLER_H_ */
