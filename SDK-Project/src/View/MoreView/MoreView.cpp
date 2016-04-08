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
#include "gui/Widgets/RadioGroup.h"

#include "View/MoreView/MoreView.h"


namespace CallSettings { namespace View {

	using namespace gui;

	MoreView::MoreView(NaviItem *naviItem) :
		BaseView(naviItem),
		m_pGenlist(nullptr),
		m_pCallerIdOption(nullptr),
		m_pCallFwdOption(nullptr),
		m_pCallWaitingOption(nullptr),
		m_pCallerIdPopup(nullptr)
	{
	}

	MoreView::~MoreView()
	{
		if (m_pCallerIdPopup) {
			hideCallerIdPopup();
		}
	}

	bool MoreView::createViewContent()
	{
		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		m_pCallerIdOption = m_pGenlist->appendItem<OptionItem>("IDS_CST_MBODY_MY_CALLER_ID");
		m_pCallFwdOption = m_pGenlist->appendItem<OptionItem>("IDS_CST_BODY_CALL_FORWARDING");
		m_pCallWaitingOption = m_pGenlist->appendItem<CheckOptionItem>(
				"IDS_CST_BODY_CALL_WAITING",
				"IDS_CST_SBODY_RECEIVE_CALL_ALERTS_DURING_CALLS_ABB");

		RETVM_IF(!m_pCallerIdOption || !m_pCallFwdOption || !m_pCallWaitingOption, false, "Internal error");

		m_pNaviItem->showBackButton();
		m_pNaviItem->setTitleText("IDS_COM_BODY_MORE");

		return setViewContent(*m_pGenlist);
	}

	void MoreView::setCallerIdStatus(CallerIdStatus value)
	{
		switch(value) {
		case CALLER_ID_STATUS_DEFAULT:
			m_pCallerIdOption->setSubText("IDS_CST_BODY_BY_NETWORK");
			break;
		case CALLER_ID_STATUS_SHOW:
			m_pCallerIdOption->setSubText("IDS_CST_OPT_SHOW");
			break;
		case CALLER_ID_STATUS_HIDE:
			m_pCallerIdOption->setSubText("IDS_CST_OPT_HIDE");
			break;
		default:
			m_pCallerIdOption->setSubText(nullptr);
			break;
		}
	}

	void MoreView::showCallerIdPopup(CallerIdStatus selectedValue, CallerIdStatusChangeHandler statusHandler, NotiHandler popupHideCb)
	{
		RETM_IF(m_pCallerIdPopup, "Popup is already shown!");

		RadioGroup *group = Widget::create<RadioGroup>(*m_pViewLayout);
		group->addItem("IDS_CST_BODY_BY_NETWORK", CALLER_ID_STATUS_DEFAULT);
		group->addItem("IDS_CST_OPT_SHOW", CALLER_ID_STATUS_SHOW);
		group->addItem("IDS_CST_OPT_HIDE", CALLER_ID_STATUS_HIDE);
		group->selectRadioItem(selectedValue);
		group->setSelectedCallback(RadioSelectHandler::wrap<MoreView, &MoreView::onCallerIdPopupItemSelect>(this));

		m_pCallerIdPopup = Widget::create<Popup>(
				WidgetWrapper(m_pNaviItem->getParent()), "IDS_CST_MBODY_MY_CALLER_ID", group);
		RETM_IF(!m_pCallerIdPopup, "Failed to create Caller Id popup!");

		m_pCallerIdPopup->setDestroyHandler(NotiHandler::wrap<MoreView, &MoreView::onCallerIdPopupDestroy>(this));
		m_idStatusHandler = statusHandler;
		m_CallerIdPopupHideCb = popupHideCb;
	}

	void MoreView::hideCallerIdPopup()
	{
		if (m_pCallerIdPopup) {
			Widget::destroy(m_pCallerIdPopup);
			m_pCallerIdPopup = nullptr;
		}
	}

	void MoreView::onCallerIdPopupDestroy()
	{
		m_pCallerIdPopup = nullptr;
		if (m_CallerIdPopupHideCb.assigned()) {
			m_CallerIdPopupHideCb();
		}
	}

	void MoreView::onCallerIdPopupItemSelect(int value)
	{
		if (m_idStatusHandler.assigned()) {
			m_idStatusHandler(static_cast<CallerIdStatus>(value));
		}
	}
} }
