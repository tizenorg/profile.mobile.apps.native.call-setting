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

#include "View/MoreView/MoreView.h"

namespace CallSettings { namespace View {

	using namespace gui;

	MoreView::MoreView(NaviItem *naviItem) :
		BaseView(naviItem),
		m_pGenlist(nullptr),
		m_pCallerIdOption(nullptr),
		m_pCallFwdOption(nullptr),
		m_pCallWaitingOption(nullptr)
	{
	}

	MoreView::~MoreView()
	{
	}

	bool MoreView::createViewContent()
	{
		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		m_pCallerIdOption = m_pGenlist->appendItem<OptionItem>( "IDS_CST_MBODY_MY_CALLER_ID");
		m_pCallFwdOption = m_pGenlist->appendItem<OptionItem>("IDS_CST_BODY_CALL_FORWARDING");
		m_pCallWaitingOption = m_pGenlist->appendItem<CheckOptionItem>(
				"IDS_CST_BODY_CALL_WAITING",
				"IDS_CST_SBODY_RECEIVE_CALL_ALERTS_DURING_CALLS_ABB");

		RETVM_IF(!m_pCallerIdOption || !m_pCallFwdOption || !m_pCallWaitingOption, false, "Internal error");

		m_pNaviItem->showBackButton();
		m_pNaviItem->setTitleText("IDS_COM_BODY_MORE");

		return setViewContent(*m_pGenlist);
	}

	void MoreView::setCallerIdValue(int callerId)
	{
		switch(callerId) {
		case 0:
			m_pCallerIdOption->setSubText("IDS_CST_BODY_BY_NETWORK");
			break;
		case 1:
			m_pCallerIdOption->setSubText("IDS_CST_OPT_SHOW");
			break;
		case 2:
			m_pCallerIdOption->setSubText("IDS_CST_OPT_HIDE");
			break;
		default:
			m_pCallerIdOption->setSubText(nullptr);
			break;
		}
	}
} }
