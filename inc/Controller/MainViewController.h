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

	/*
	 * @brief Represents Controller of Main view
	 */
	class MainViewController : public gui::ViewController {
	protected:
		virtual ~MainViewController();

	private:
		friend class ViewController;

		MainViewController(Application &app, NotiHandler handler);

		/*
		 * @brief Called when view needs update for some reasons
		 * @param[in] Update flags which describes update reason
		 */
		virtual void updateView(int updateFlag) override;

		/*
		 * @brief initialization of MainViewController
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief Called when Reject messages list item clicked
		 */
		void onRejectMsgClick();

		/*
		 * @brief Called when Answering/Ending call list item clicked
		 */
		void onCallAnsweringClick();

		/*
		 * @brief Called when More list item clicked
		 */
		void onMoreClick();

		/*
		 * @brief Called when Reject message view controller make destroy request
		 */
		void onRejectMsgControllerDestroy();

		/*
		 * @brief Called when Answering view controller make destroy request
		 */
		void onAnswerControllerDestroy();

		/*
		 * @brief Called when More view controller make destroy request
		 */
		void onMoreControllerDestroy();

		/*
		 * @brief Check whether SIM card is inserted and ready for work. In case of any issues toast message will be shown
		 * @return true if SIM inserted and ready, otherwise false
		 */
		bool checkSimReadinessWithNoti();

		/*
		 * @brief Check whether SIM card is inserted and ready for work
		 * @return true if SIM inserted and ready, otherwise false
		 */
		bool isSimReady();

		/*
		 * @brief Show SIM error toast message with needed error text based on SIM status
		 */
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
