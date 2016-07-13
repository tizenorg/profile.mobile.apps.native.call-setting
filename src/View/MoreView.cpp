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

#include "gui/NaviItem.h"
#include "gui/RadioGroup.h"

#include "View/MoreView.h"

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

	bool MoreView::initialize()
	{
		RETVM_IF(!BaseView::initialize(), false, "Failed to initialize BaseView!");

		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		m_pGenlist->setHomogeneousMode(false);

		m_pCallerIdOption = m_pGenlist->appendItem<DoubleTextListItem>("IDS_CST_MBODY_MY_CALLER_ID");
		m_pCallFwdOption = m_pGenlist->appendItem<SimpleListItem>("IDS_CST_BODY_CALL_FORWARDING");
		m_pCallWaitingOption = m_pGenlist->appendItem<DoubleTextListItem>(
				"IDS_CST_BODY_CALL_WAITING",
				"IDS_CST_SBODY_RECEIVE_CALL_ALERTS_DURING_CALLS_ABB",
				true);

		RETVM_IF(!m_pCallerIdOption || !m_pCallFwdOption || !m_pCallWaitingOption, false, "Internal error");

		m_pCallerIdOption->setSubTextFormat(gui::GenlistItem::getSubItemTextColorFmt());
		m_pCallerIdOption->setCheckMode(gui::CheckboxListItem::HIDDEN);
		m_pCallWaitingOption->setCheckboxStyle(gui::Checkbox::CHECKBOX_SWITCHER);
		m_pCallWaitingOption->setCheckMode(gui::CheckboxListItem::AUTO_CHECK);

		m_pCallerIdOption->setSelectHandler(ItemNotiHandler::wrap<MoreView, &MoreView::onOptionSelected>(this));
		m_pCallFwdOption->setSelectHandler(ItemNotiHandler::wrap<MoreView, &MoreView::onOptionSelected>(this));
		m_pCallWaitingOption->setCheckHandler(ItemNotiHandler::wrap<MoreView, &MoreView::onOptionSelected>(this));

		m_pNaviItem->setTitleText("IDS_CST_MBODY_MORE");

		return setViewContent(*m_pGenlist);
	}

	void MoreView::onOptionSelected(WidgetItem &item)
	{
		if (&item == static_cast<WidgetItem *>(m_pCallerIdOption) && m_callerIdClickHandeler.assigned()) {
			m_callerIdClickHandeler();
		} else if (&item == static_cast<WidgetItem *>(m_pCallFwdOption) && m_callFwdClickHandler.assigned()) {
			m_callFwdClickHandler();
		} else if (&item == static_cast<WidgetItem *>(m_pCallWaitingOption) && m_callWaitingCheckHandler.assigned()) {
			m_callWaitingCheckHandler();
		}
	}

	void MoreView::setWaitingOptionPending(bool isPending)
	{
		if (isPending) {
			m_pCallWaitingOption->setCheckMode(gui::CheckboxListItem::PENDING);
		} else {
			m_pCallWaitingOption->setCheckMode(gui::CheckboxListItem::AUTO_CHECK);
		}
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
		m_callerIdPopupHideCb = popupHideCb;
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
		if (m_callerIdPopupHideCb.assigned()) {
			m_callerIdPopupHideCb();
		}
	}

	void MoreView::onCallerIdPopupItemSelect(int value)
	{
		if (m_idStatusHandler.assigned()) {
			m_idStatusHandler(static_cast<CallerIdStatus>(value));
		}
	}
} }
