/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _VIEW_REJECT_MSG_EDIT_VIEW_H_
#define _VIEW_REJECT_MSG_EDIT_VIEW_H_

#include "gui/BaseView.h"
#include "gui/Layout.h"
#include "gui/Button.h"
#include "gui/Editfield.h"
#include "gui/Scroller.h"

namespace CallSettings { namespace View {

	/*
	 * @brief Represents instrance of Reject message editor view
	 */
	class RejectMsgEditView : public gui::BaseView {
	public:
		/*
		 * @brief Set current version of message text which will be edit into entry widget
		 * @param[in]	text	Reject message text
		 */
		void setMsgText(const util::TString &text);

		/*
		 * @brief Get reject message text which is currently entered into editfield
		 * @return Reject message text
		 */
		std::string getMsgText() {return m_pEditfield->getEntryRawText();}

		/*
		 * @brief Updates char counter label in editor view
		 * @param[in]	inputCount	Count of inputed symbols
		 * @param[in]	maxCount	Maximum count of symbols
		 */
		void updateInputCharCount(int inputCount, int maxCount);

		/*
		 * @brief Hide char counter label
		 */
		void hideInputCharCount();

		/*
		 * @brief Set active/disabled DONE button in title view
		 * @param[in]	disableState	If true button will be disabled, otherwise enabled
		 */
		void setDisabledDoneBtn(bool disableState) { m_pDoneBtn->setDisabled(disableState);}

		/*
		 * @brief Set Done title button click handler
		 * @param[in]	handler		click handler
		 */
		void setDoneClickHandler(gui::WidgetNotiHandler handler) { m_pDoneBtn->setClickHandler(handler);}

		/*
		 * @brief Set Cancel title button click handler
		 * @param[in]	handler		click handler
		 */
		void setCancelClickHandler(gui::WidgetNotiHandler handler) { m_pCancelBtn->setClickHandler(handler); }

		/*
		 * @brief Set input event handler
		 * @param[in]	handler		click handler
		 */
		void setInputEventHandler(NotiHandler handler) { m_pEditfield->setInputEventHandler(handler); }

		/*
		 * @brief Set input filter event handler
		 * @param[in]	handler		click handler
		 */
		void setInputFilterHandler(gui::Editfield::InputFilterHandler handler) { m_pEditfield->setInputFilterHandler(handler); }

		/*
		 * @brief Set view title text
		 * @param[in]	titleText	View Title text
		 */
		void setViewTitle(util::TString titleText);

	private:
		friend class BaseView;

		RejectMsgEditView(gui::NaviItem *naviItem);
		virtual ~RejectMsgEditView() {}

		bool initialize();

	private:
		gui::Layout *m_pEditorLayout;
		gui::Scroller *m_pScroller;
		gui::Editfield *m_pEditfield;
		gui::Button *m_pCancelBtn;
		gui::Button *m_pDoneBtn;
	};
} }
#endif /* _VIEW_REJECT_MSG_EDIT_VIEW_H_ */
