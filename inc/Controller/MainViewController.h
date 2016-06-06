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

#ifndef _CONTROLLER_MAIN_VIEW_CONTROLLER_H_
#define _CONTROLLER_MAIN_VIEW_CONTROLLER_H_

#include "gui/ViewController.h"

#include "App/Application.h"
#include "View/MainView.h"

#include "RejectMsgListController.h"
#include "AnswerViewController.h"
#include "MoreViewController.h"

namespace CallSettings { namespace Controller {

	class MainViewController : public gui::ViewController {
	protected:
		virtual ~MainViewController();

	private:
		friend class ViewController;

		MainViewController(Application &app, NotiHandler handler);
		virtual void updateView(int updateFlag) override;
		bool initialize();
		void onRejectMsgClick();
		void onCallAnsweringClick();
		void onMoreClick();
		void onRejectMsgControllerDestroy();
		void onAnswerControllerDestroy();
		void onMoreControllerDestroy();

		bool checkSimReadinessWithNoti();
		bool isSimReady();
		void showSimErrorNoti();

	private:
		Application &m_app;
		RejectMsgListController *m_pRejectListViewController;
		AnswerViewController *m_pAnswerViewController;
		MoreViewController *m_pMoreViewController;
		View::MainView *m_pMainView;
	};
} }

#endif /* _CONTROLLER_MAIN_VIEW_CONTROLLER_H_ */
