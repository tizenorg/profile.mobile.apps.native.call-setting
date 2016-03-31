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

#ifndef ANSWER_VIEW_
#define ANSWER_VIEW_

#include "View/ViewManager/BaseView.h"
#include "View/Widgets/CheckOptionItem.h"
#include "View/Widgets/Genlist.h"

namespace AnswerView {

	class AnswerView : public View::BaseView {
	public:
		void setAnswerCheckHandler(NotiHandler handler) {m_pAnswerOption->setCheckHandler(handler);}
		void setRejectCheckHandler(NotiHandler handler) {m_pRejectOption->setCheckHandler(handler);}
		void setAnswerItemClickHandler(NotiHandler handler) {m_pAnswerOption->setSelectHandler(handler);}
		void setRejectItemClickHandler(NotiHandler handler) {m_pRejectOption->setSelectHandler(handler);}

		bool getAnswerOptionState() { return m_pAnswerOption->getCheckState();}
		bool getRejectOptionState() { return m_pRejectOption->getCheckState();}
		void checkAnswerOption(bool checked) {m_pAnswerOption->setCheckState(checked);}
		void checkRejectOption(bool checked) {m_pRejectOption->setCheckState(checked);}

	private:
		friend class BaseView;

		AnswerView(Widgets::NaviItem *naviItem);
		virtual ~AnswerView();
		virtual bool createViewContent() override;

	private:
		Widgets::Genlist *m_pGenlist;
		Widgets::CheckOptionItem *m_pAnswerOption;
		Widgets::CheckOptionItem *m_pRejectOption;
	};
}

#endif /* ANSWER_VIEW_ */
