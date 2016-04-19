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

#ifndef _VIEW_ANSWER_VIEW_
#define _VIEW_ANSWER_VIEW_

#include "gui/Base/BaseView.h"
#include "gui/Widgets/Genlist.h"
#include "gui/Widgets/CheckboxListItem.h"

namespace CallSettings { namespace View {

	class AnswerView : public gui::BaseView {
	public:
		void setAnswerCheckHandler(NotiHandler handler) {m_answerCheckHandler = handler;}
		void setRejectCheckHandler(NotiHandler handler) {m_rejectCheckHandler = handler;}

		bool getAnswerOptionState() { return m_pAnswerOption->getCheckState();}
		bool getRejectOptionState() { return m_pRejectOption->getCheckState();}
		void checkAnswerOption(bool checked) {m_pAnswerOption->setCheckState(checked);}
		void checkRejectOption(bool checked) {m_pRejectOption->setCheckState(checked);}

	private:
		friend class BaseView;

		AnswerView(gui::NaviItem *naviItem);
		virtual ~AnswerView();
		virtual bool createViewContent() override;
		void onOptionChecked(gui::WidgetItem *item);

	private:
		gui::Genlist *m_pGenlist;
		gui::CheckboxListItem *m_pAnswerOption;
		gui::CheckboxListItem *m_pRejectOption;
		NotiHandler m_answerCheckHandler;
		NotiHandler m_rejectCheckHandler;
	};
} }

#endif /* _VIEW_ANSWER_VIEW_ */
