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

#include "gui/Widgets/NaviItem.h"
#include "gui/Widgets/DoubleTextListItem.h"
#include "gui/Widgets/SimpleListItem.h"

#include "View/MainView/MainView.h"

namespace CallSettings { namespace View {

	using namespace gui;

	MainView::MainView(NaviItem *naviItem) :
		BaseView(naviItem),
		m_pGenlist(nullptr),
		m_pRejectMsgOption(nullptr),
		m_pCallAceptOption(nullptr),
		m_pMoreOption(nullptr)
	{
	}

	MainView::~MainView()
	{
	}

	bool MainView::createViewContent()
	{
		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		m_pRejectMsgOption = m_pGenlist->appendItem<DoubleTextListItem>(
				"IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB",
				"IDS_CST_SBODY_COMPOSE_OR_EDIT_RESPONSE_MESSAGES_TO_SEND_WHEN_REJECTING_INCOMING_CALLS",
				true, true, true);

		m_pCallAceptOption = m_pGenlist->appendItem<SimpleListItem>("IDS_CST_HEADER_ANSWERING_ENDING_CALLS_ABB");
		m_pMoreOption = m_pGenlist->appendItem<SimpleListItem>("IDS_COM_BODY_MORE");

		RETVM_IF(!m_pRejectMsgOption || !m_pCallAceptOption || !m_pMoreOption, false, "Internal error");

		m_pRejectMsgOption->setCheckMode(CheckboxListItem::HIDDEN);

		m_pNaviItem->showBackButton();
		m_pNaviItem->setTitleText("IDS_CST_HEADER_CALL_SETTINGS");

		m_pRejectMsgOption->setSelectHandler(ItemNotiHandler::wrap<MainView, &MainView::onOptionSelected>(this));
		m_pCallAceptOption->setSelectHandler(ItemNotiHandler::wrap<MainView, &MainView::onOptionSelected>(this));
		m_pMoreOption->setSelectHandler(ItemNotiHandler::wrap<MainView, &MainView::onOptionSelected>(this));
		return setViewContent(*m_pGenlist);
	}

	void MainView::onOptionSelected(WidgetItem *item)
	{
		RETM_IF(!item, "Invalid args!");

		if (item == static_cast<WidgetItem *>(m_pRejectMsgOption) && m_rejectMsgClickHandler.assigned()) {
			m_rejectMsgClickHandler();
		} else if (item == static_cast<WidgetItem *>(m_pCallAceptOption) && m_callAceptClickHandler.assigned()) {
			m_callAceptClickHandler();
		} else if (item == static_cast<WidgetItem *>(m_pMoreOption) && m_moreClickHandler.assigned()) {
			m_moreClickHandler();
		}
	}

} }
