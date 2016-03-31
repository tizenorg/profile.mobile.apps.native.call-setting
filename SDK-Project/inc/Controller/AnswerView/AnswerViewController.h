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

#ifndef ANSWER_VIEW_CONTROLLER_H_
#define ANSWER_VIEW_CONTROLLER_H_

#include "Controller/ViewController.h"
#include "View/AnswerView/AnswerView.h"

namespace AnsweringController {

	class AnswerViewController : public Controller::ViewController {
	public:
		virtual ~AnswerViewController();

	private:
		friend class ViewController;

		AnswerViewController(App::AppCore &core, NotifyHandler handler);
		virtual void onBackKeyPressed() override;
		virtual void updateView(int updateFlag) override;
		bool initialize();

		void onAnswerOptionCheck();
		void onRejectOptionCheck();
		void onAnswerItemClick();
		void onRejectItemClick();
		void onAnswerOptionChanged();
		void onRejectOptionChanged();
		void updateAnswerOption();
		void updateRejectOption();

	private:
		AnswerView::AnswerView *m_pAnswerView;
		bool m_needUpdateAnswering;
		bool m_needUpdateRejection;
	};
}

#endif /* ANSWER_VIEW_CONTROLLER_H_ */
