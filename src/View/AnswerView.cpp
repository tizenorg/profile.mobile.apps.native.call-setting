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

#include "gui/CategoryItem.h"

#include "View/AnswerView.h"

namespace CallSettings { namespace View {
	using namespace gui;

	AnswerView::AnswerView(NaviItem *naviItem) :
		BaseView(naviItem),
		m_pGenlist(nullptr),
		m_pAnswerOption(nullptr),
		m_pRejectOption(nullptr)
	{
	}

	AnswerView::~AnswerView()
	{
	}

	bool AnswerView::initialize()
	{
		RETVM_IF(!BaseView::initialize(), false, "Failed to initialize BaseView!");

		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		CategoryListItem *answerCategory = m_pGenlist->appendItem<CategoryListItem>("IDS_CST_HEADER_ANSWER_CALLS_BY");
		CategoryListItem *rejectCategory = m_pGenlist->appendItem<CategoryListItem>("IDS_CST_HEADER_END_CALLS_BY");
		RETVM_IF(!answerCategory || !rejectCategory, false, "Internal error");

		m_pAnswerOption = m_pGenlist->insertAfter<CheckboxListItem>(*answerCategory, "IDS_CST_MBODY_PRESSING_THE_HOME_KEY");
		m_pRejectOption = m_pGenlist->insertAfter<CheckboxListItem>(*rejectCategory, "IDS_CST_MBODY_PRESSING_THE_POWER_KEY");
		RETVM_IF(!m_pAnswerOption || !m_pRejectOption, false, "Internal error");


		answerCategory->setCheckMode(gui::CheckboxListItem::HIDDEN);
		answerCategory->setSelectionMode(GENLIST_ITEM_SELECT_MODE_NONE);

		m_pAnswerOption->setCheckboxStyle(CHECKBOX_SWITCHER);
		m_pAnswerOption->setCheckMode(gui::CategoryListItem::AUTO_CHECK);
		m_pAnswerOption->setCheckHandler(ItemNotiHandler::wrap<AnswerView, &AnswerView::onOptionChecked>(this));

		rejectCategory->setCheckMode(gui::CheckboxListItem::HIDDEN);
		rejectCategory->setSelectionMode(GENLIST_ITEM_SELECT_MODE_NONE);

		m_pRejectOption->setCheckboxStyle(CHECKBOX_SWITCHER);
		m_pRejectOption->setCheckMode(gui::CategoryListItem::AUTO_CHECK);
		m_pRejectOption->setCheckHandler(ItemNotiHandler::wrap<AnswerView, &AnswerView::onOptionChecked>(this));

		m_pNaviItem->setTitleText("IDS_CST_HEADER_ANSWERING_ENDING_CALLS_ABB");

		return setViewContent(*m_pGenlist);
	}

	void AnswerView::onOptionChecked(gui::WidgetItem &item)
	{
		CheckboxListItem *optionItem = static_cast<CheckboxListItem *>(&item);
		if (optionItem == m_pAnswerOption && m_answerCheckHandler.assigned()) {
			m_answerCheckHandler();
		} else if (optionItem == m_pRejectOption && m_rejectCheckHandler.assigned()) {
			m_rejectCheckHandler();
		}
	}
} }
