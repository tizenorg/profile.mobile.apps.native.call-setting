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

#include "gui/Widgets/CategoryItem.h"

#include "View/AnswerView/AnswerView.h"

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

	bool AnswerView::createViewContent()
	{
		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		CategoryItem *answerCategory = m_pGenlist->appendItem<CategoryItem>("IDS_CST_HEADER_ANSWER_CALLS_BY");
		CategoryItem *rejectCategory = m_pGenlist->appendItem<CategoryItem>("IDS_CST_HEADER_END_CALLS_BY");

		m_pAnswerOption = m_pGenlist->insertAfter<CheckOptionItem>(*answerCategory, "IDS_CST_MBODY_PRESSING_THE_HOME_KEY");
		m_pRejectOption = m_pGenlist->insertAfter<CheckOptionItem>(*rejectCategory, "IDS_CST_MBODY_PRESSING_THE_POWER_KEY");

		RETVM_IF(!m_pAnswerOption || !m_pRejectOption, false, "Internal error");

		m_pNaviItem->showBackButton();
		m_pNaviItem->setTitleText("IDS_CST_HEADER_ANSWERING_ENDING_CALLS_ABB");

		return setViewContent(*m_pGenlist);
	}
} }
