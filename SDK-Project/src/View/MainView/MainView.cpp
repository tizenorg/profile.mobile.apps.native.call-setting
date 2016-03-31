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

#include "View/MainView/MainView.h"
#include "View/Widgets/NaviItem.h"
#include "View/Widgets/OptionItem.h"

namespace MainView {

	using namespace Widgets;

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

		m_pRejectMsgOption = m_pGenlist->appendItem<OptionItem>(
				"IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB",
				"IDS_CST_SBODY_COMPOSE_OR_EDIT_RESPONSE_MESSAGES_TO_SEND_WHEN_REJECTING_INCOMING_CALLS");

		m_pCallAceptOption = m_pGenlist->appendItem<OptionItem>("IDS_CST_HEADER_ANSWERING_ENDING_CALLS_ABB");
		m_pMoreOption = m_pGenlist->appendItem<OptionItem>("IDS_COM_BODY_MORE");

		RETVM_IF(!m_pRejectMsgOption || !m_pCallAceptOption || !m_pMoreOption, false, "Internal error");

		m_pNaviItem->showBackButton();
		m_pNaviItem->setTitleText("IDS_CST_HEADER_CALL_SETTINGS");

		return setViewContent(*m_pGenlist);
	}

	void MainView::setRejectMsgHandler(NotifyHandler handler)
	{
		m_pRejectMsgOption->setSelectHandler(handler);
	}

	void MainView::setCallAceptHandler(NotifyHandler handler)
	{
		m_pCallAceptOption->setSelectHandler(handler);
	}

	void MainView::setMoreHandler(NotifyHandler handler)
	{
		m_pMoreOption->setSelectHandler(handler);
	}
}
