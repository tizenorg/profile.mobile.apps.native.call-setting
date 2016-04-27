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

#include "gui/Base/BaseView.h"
#include "gui/Widgets/Layout.h"
#include "gui/Widgets/Button.h"
#include "gui/Widgets/Editfield.h"
#include "gui/Widgets/Scroller.h"

namespace CallSettings { namespace View {

	class RejectMsgEditView : public gui::BaseView {
	public:
		void setMsgText(const std::string &text) {m_pEditfield->setEntryMarkupText(text);}
		std::string getMsgText() {return m_pEditfield->getEntryRawText();}
		void updateInputCharCount(int inputCount, int maxCount);
		void hideInputCharCount();
		void setDisabledDoneBtn(bool disableState) { m_pDoneBtn->setDisabled(disableState);}
		void setDoneClickHandler(NotiHandler handler) { m_pDoneBtn->setClickHandler(handler);}
		void setCancelClickHandler(NotiHandler handler) { m_pCancelBtn->setClickHandler(handler); }
		void setInputEventHandler(NotiHandler handler) { m_pEditfield->setInputEventHandler(handler); }
		void setInputFilterHandler(gui::Editfield::InputFilterHandler handler) { m_pEditfield->setInputFilterHandler(handler); }
		void setViewTitle(const char *titleText);

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
