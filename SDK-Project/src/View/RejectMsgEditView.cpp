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

#include "appfw/Path.h"

#include "View/RejectMsgEditView.h"

namespace CallSettings { namespace View {

	using namespace gui;

	namespace {

		util::TString CHAR_COUNTER_FMT = "%d/%d";
	}

	RejectMsgEditView::RejectMsgEditView(NaviItem *naviItem) :
		BaseView(naviItem),
		m_pEditorLayout(nullptr),
		m_pScroller(nullptr),
		m_pEditfield(nullptr),
		m_pCancelBtn(nullptr),
		m_pDoneBtn(nullptr)
	{
	}

	bool RejectMsgEditView::initialize()
	{
		RETVM_IF(!BaseView::initialize(), false, "Failed to initialize BaseView!");

		m_pScroller = Widget::create<Scroller>(*m_pViewLayout);
		RETVM_IF(!m_pScroller, false, "Failed to create scroller, unknown error");

		m_pEditorLayout = Widget::create<Layout>(*m_pScroller, "layout", "call_setting", "reject_msg.editor_layout");
		RETVM_IF(!m_pEditorLayout, false, "Failed to create editor layout, unknown error");

		m_pEditfield = Widget::create<Editfield>(*m_pEditorLayout, Editfield::EF_MULTILINE, "IDS_CST_POP_ENTER_MESSAGE");
		RETVM_IF(!m_pEditfield, false, "Failed to create editfield, unknown error");

		if (!m_pEditorLayout->setContent("entry_part", *m_pEditfield)) {
			ERR("Failed to set scroller as editor layout content!");
			return false;
		}

		m_pNaviItem->hideBackButton();

		m_pScroller->setContent(*m_pEditorLayout);
		m_pCancelBtn = Widget::create<Button>(*m_pViewLayout, "naviframe/title_left", "IDS_MSG_ACBUTTON_CANCEL_ABB");
		RETVM_IF(!m_pCancelBtn, false, "Internal error");
		m_pNaviItem->setTitleLeftBtn(*m_pCancelBtn);

		m_pDoneBtn = Widget::create<Button>(*m_pViewLayout, "naviframe/title_right", "IDS_MSG_ACBUTTON_DONE_ABB");
		RETVM_IF(!m_pDoneBtn, false, "Internal error");
		m_pNaviItem->setTitleRightBtn(*m_pDoneBtn);

		return setViewContent(*m_pScroller);
	}

	void RejectMsgEditView::setViewTitle(const char *titleText)
	{
		m_pNaviItem->setTitleText(_(titleText));
	}

	void RejectMsgEditView::setMsgText(const util::TString &text)
	{
		if (text.isTranslatable()) {
			m_pEditfield->setEntryMarkupText(text.translate());
		} else {
			m_pEditfield->setEntryMarkupText(text.getStr());
		}
	}

	void RejectMsgEditView::updateInputCharCount(int inputCount, int maxCount)
	{
		m_pEditorLayout->setPartText("char_count_text", CHAR_COUNTER_FMT.format(inputCount, maxCount));
	}

	void RejectMsgEditView::hideInputCharCount()
	{
		m_pEditorLayout->setPartText("char_count_text", "");
	}

} }
