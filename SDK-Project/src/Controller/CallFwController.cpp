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

#include "gui/Widgets/DoubleTextListItem.h"
#include "gui/Widgets/RadioGroup.h"

#include "Controller/CallFwController.h"

namespace CallSettings { namespace Controller {

	using namespace gui;
	using namespace Model;

	namespace {
		enum {
			VIEW_ITEM_COUNT = 4,
			TIMEOUT_ITEM_COUNT = 6
		};

		struct TimeoutItemInfo {
			CallFwdNoReplyTime noReplyTimeId;
			int timeoutSec;
		};

		const util::TString VIEW_ITEM_LABELS[VIEW_ITEM_COUNT] = {
				"IDS_CST_BODY_ALWAYS_FORWARD",
				"IDS_CST_MBODY_FORWARD_WHEN_BUSY",
				"IDS_COM_BODY_FORWARD_WHEN_UNANSWERED",
				"IDS_CST_MBODY_FORWARD_WHEN_UNREACHABLE"
		};

		const TimeoutItemInfo TIMEOUT_ITEMS_INFO[TIMEOUT_ITEM_COUNT] = {
				{TELEPHONY_CF_NO_REPLY_5_SEC, 5},
				{TELEPHONY_CF_NO_REPLY_10_SEC, 10},
				{TELEPHONY_CF_NO_REPLY_15_SEC, 15},
				{TELEPHONY_CF_NO_REPLY_20_SEC, 20},
				{TELEPHONY_CF_NO_REPLY_25_SEC, 25},
				{TELEPHONY_CF_NO_REPLY_30_SEC, 30}
		};

		const util::TString TIMEOUT_ITEM_FMT = "IDS_CST_BODY_PD_SECONDS";
	}

	CallFwController::CallFwController(Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pView(nullptr),
		m_pTimeoutPopup(nullptr)
	{
	}

	CallFwController::~CallFwController()
	{
		Widget::destroy(m_pTimeoutPopup);
	}

	bool CallFwController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		m_pView = m_app.getViewManager().pushView<View::GenlistView>();
		RETVM_IF(!m_pView, false, "Failed to create view");

		setBaseView(m_pView);

		return true;
	}

	void CallFwController::updateView(int updateFlag)
	{
		if (updateFlag & UF_INITIAL) {

			m_pView->getItem().setTitleText("IDS_CST_BODY_CALL_FORWARDING");

			Genlist &gl = m_pView->getGenlist();

			for (int i = 0; i < VIEW_ITEM_COUNT; ++i) {
				DoubleTextListItem *item = gl.appendItem<DoubleTextListItem>(VIEW_ITEM_LABELS[i]);
				if (item) {
					item->setCheckboxStyle(CHECKBOX_SWITCHER);
					item->setCheckMode(CheckboxListItem::SKIP_EVENTS);
					item->setSelectHandler(ItemNotiHandler::wrap<
							CallFwController, &CallFwController::onItemClick>(this));
				} else {
					ERR("Item create failed!");
				}
			}
		}
	}

	void CallFwController::onItemClick(WidgetItem *pItem)
	{
		DBG("View itme clicked");
		showTimeoutPopup();
	}

	void CallFwController::showTimeoutPopup()
	{
		Widget::destroy(m_pTimeoutPopup);

		m_pTimeoutPopup = Widget::create<Popup>(WidgetWrapper(m_app.getViewManager().getWindow()),
				"IDS_CST_BODY_WAITING_TIME");
		RETM_IF(!m_pTimeoutPopup, "Failed to create timeout popup!");

		m_pTimeoutPopup->setDestroyHandler(NotiHandler::wrap<
				CallFwController, &CallFwController::onTimeoutPopupDel>(this));

		RadioGroup *group = Widget::create<RadioGroup>(*m_pTimeoutPopup);
		RETM_IF(!group,  "Failed to RadioGroup");

		m_pTimeoutPopup->setContent(*group);

		for (int i = 0; i < TIMEOUT_ITEM_COUNT; ++i) {
			const TimeoutItemInfo info = TIMEOUT_ITEMS_INFO[i];
			group->addItem(TIMEOUT_ITEM_FMT.format(info.timeoutSec), info.noReplyTimeId);
		}

		group->selectRadioItem(TELEPHONY_CF_NO_REPLY_10_SEC);

		group->setSelectedCallback(RadioSelectHandler::wrap<
				CallFwController, &CallFwController::onTimeoutValueSelected>(this));
	}

	void CallFwController::onTimeoutPopupDel()
	{
		m_pTimeoutPopup = nullptr;
	}

	void CallFwController::onTimeoutValueSelected(int noReplyTimeId)
	{
		INF("noReplyTimeId: %d", noReplyTimeId);
		Widget::destroy(m_pTimeoutPopup);
	}
}}
