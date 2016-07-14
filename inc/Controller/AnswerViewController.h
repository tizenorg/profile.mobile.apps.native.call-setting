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

#ifndef _CONTROLLER_ANSWER_VIEW_CONTROLLER_H_
#define _CONTROLLER_ANSWER_VIEW_CONTROLLER_H_

#include "gui/ViewController.h"

#include "App/Application.h"
#include "View/AnswerView.h"

namespace CallSettings { namespace Controller {

	/*
	 * @brief Represents Controller of Answering/Ending calls view
	 */
	class AnswerViewController : public gui::ViewController {
	public:
		virtual ~AnswerViewController();

	private:
		friend class ViewController;

		AnswerViewController(Application &app, NotiHandler handler);

		/*
		 * @brief Called when view needs update for some reasons
		 * @param[in] Update flags which describes update reason
		 */
		virtual void updateView(int updateFlag) override;

		/*
		 * @brief initialization of AnswerViewController
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief Called when checkbox of answer option is checked
		 */
		void onAnswerOptionCheck();

		/*
		 * @brief Called when checkbox of reject option is checked
		 */
		void onRejectOptionCheck();

		/*
		 * @brief Called when answer option state changed. This callback is invoked by model
		 */
		void onAnswerOptionChanged();

		/*
		 * @brief Called when reject option state changed. This callback is invoked by model
		 */
		void onRejectOptionChanged();

		/*
		 * @brief Update answer option list item state
		 */
		void updateAnswerOption();

		/*
		 * @brief Update reject option list item state
		 */
		void updateRejectOption();

	private:
		Application &m_app;
		View::AnswerView *m_pAnswerView;

		bool m_needUpdateAnswering;
		bool m_needUpdateRejection;
	};
} }

#endif /* _CONTROLLER_ANSWER_VIEW_CONTROLLER_H_ */
