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

#include "View/PhoneEditListItem.h"

#include "Controller/CallFwController.h"

namespace CallSettings { namespace Controller {

	using namespace gui;
	using namespace Model;

	namespace {
		enum {
			VIEW_ITEM_COUNT = 4,
			TIMEOUT_ITEM_COUNT = 6
		};

		const CallFwdCondition VIEW_ITEMS_COND[VIEW_ITEM_COUNT] = {
				TELEPHONY_CF_UNCONDITIONAL,
				TELEPHONY_CF_IF_BUSY,
				TELEPHONY_CF_IF_NO_REPLY,
				TELEPHONY_CF_IF_NO_REACHABLE
		};

		const CallFwdNoReplyTime TIMEOUT_ITEMS_TIME[TIMEOUT_ITEM_COUNT] = {
				TELEPHONY_CF_NO_REPLY_5_SEC,
				TELEPHONY_CF_NO_REPLY_10_SEC,
				TELEPHONY_CF_NO_REPLY_15_SEC,
				TELEPHONY_CF_NO_REPLY_20_SEC,
				TELEPHONY_CF_NO_REPLY_25_SEC,
				TELEPHONY_CF_NO_REPLY_30_SEC
		};

		const util::TString TIMEOUT_ITEM_FMT = "IDS_CST_BODY_PD_SECONDS";
		const util::TString TIMEOUT_ITEM_COLOR_FMT = "<color=#3DB9CC>%s</color>";

		util::TString getConditionLabel(CallFwdCondition condition)
		{
			switch (condition)
			{
			case TELEPHONY_CF_UNCONDITIONAL:	return "IDS_CST_BODY_ALWAYS_FORWARD";
			case TELEPHONY_CF_IF_BUSY:			return "IDS_CST_MBODY_FORWARD_WHEN_BUSY";
			case TELEPHONY_CF_IF_NO_REPLY:		return "IDS_COM_BODY_FORWARD_WHEN_UNANSWERED";
			case TELEPHONY_CF_IF_NO_REACHABLE:	return "IDS_CST_MBODY_FORWARD_WHEN_UNREACHABLE";
			default:
				ERR("Invalid CallFwdCondition: %d", static_cast<int>(condition));
				return "none";
			}
		}

		int getTimeoutDurationSec(CallFwdNoReplyTime noReplyTimeId)
		{
			switch (noReplyTimeId)
			{
			case TELEPHONY_CF_NO_REPLY_5_SEC:	return 5;
			case TELEPHONY_CF_NO_REPLY_10_SEC:	return 10;
			case TELEPHONY_CF_NO_REPLY_15_SEC:	return 15;
			case TELEPHONY_CF_NO_REPLY_20_SEC:	return 20;
			case TELEPHONY_CF_NO_REPLY_25_SEC:	return 25;
			case TELEPHONY_CF_NO_REPLY_30_SEC:	return 30;
			default:
				ERR("Invalid CallFwdNoReplyTime: %d", static_cast<int>(noReplyTimeId));
				return 0;
			}
		}

		util::TString getTimeoutLabel(CallFwdNoReplyTime noReplyTimeId)
		{
			return TIMEOUT_ITEM_FMT.format(getTimeoutDurationSec(noReplyTimeId));
		}

		util::TString getColoredTimeoutLabel(CallFwdNoReplyTime noReplyTimeId)
		{
			return TIMEOUT_ITEM_COLOR_FMT.format(
					TIMEOUT_ITEM_FMT.format(getTimeoutDurationSec(noReplyTimeId)).getCStr());
		}
	}

	class CallFwController::EditPopup {
	public:
		~EditPopup()
		{
			if (m_pPopup) {
				m_pPopup->setDestroyHandler(nullptr);
				Widget::destroy(m_pPopup);
			}
			if (m_delHandler.assigned()) {
				m_delHandler();
			}
		}

		static EditPopup *create(Application &app, CallFwdCondition condition, NotiHandler delHandler)
		{
			EditPopup *obj = new EditPopup(app, condition, delHandler);
			if (!obj->init()) {
				delete obj;
				return nullptr;
			}
			return obj;
		}

	private:
		EditPopup(Application &app, CallFwdCondition condition, NotiHandler delHandler,
				CallFwdNoReplyTime noReplyTimeId = TELEPHONY_CF_NO_REPLY_30_SEC) :
			m_app(app),
			m_condition(condition),
			m_delHandler(delHandler),
			m_noReplyTimeId(noReplyTimeId),
			m_pPopup(nullptr),
			m_pEditfield(nullptr),
			m_pTimeoutItem(nullptr)
		{
		}

		bool init()
		{
			m_pPopup = Widget::create<Popup>(WidgetWrapper(m_app.getViewManager().getNaviframe()),
					getConditionLabel(m_condition));
			RETVM_IF(!m_pPopup, false, "Failed to create popup!");

			if (!m_pPopup->addButton(POPUP_BUTTON_LEFT, "IDS_CST_BUTTON2_CANCEL",
					PopupClickHandler::wrap<EditPopup, &EditPopup::onCancelBtnClick>(this)) ||
				!m_pPopup->addButton(POPUP_BUTTON_RIGHT, "IDS_CST_BUTTON_ENABLE",
					PopupClickHandler::wrap<EditPopup, &EditPopup::onEnableBtnClick>(this))) {
				ERR("Popup button add failed!");
				return false;
			}

			m_pPopup->setButtonDisabled(POPUP_BUTTON_RIGHT, true);

			Genlist *genlist = Widget::create<Genlist>(*m_pPopup);
			RETVM_IF(!genlist, false, "Failed to create Genlist");
			m_pPopup->setContent(*genlist);
			genlist->setContentMinLimit(false, true);

			auto *editItem = genlist->appendItem<View::PhoneEditListItem>();
			RETVM_IF(!editItem, false, "Failed to create edit item");
			m_pEditfield = &editItem->getEditfield();

			if (m_condition == TELEPHONY_CF_IF_NO_REPLY) {
				m_pTimeoutItem = genlist->appendItem<DoubleTextListItem>(
						"IDS_CST_BODY_WAITING_TIME", getColoredTimeoutLabel(m_noReplyTimeId));
				RETVM_IF(!m_pTimeoutItem, false, "Failed to create timeout item");
				m_pTimeoutItem->setCheckMode(CheckboxListItem::HIDDEN);
			}

			m_pPopup->setDestroyHandler(NotiHandler::wrap<EditPopup, &EditPopup::onPopupDel>(this));

			return true;
		}

		void onPopupDel()
		{
			m_pPopup = nullptr;
			delete this;
		}

		bool onCancelBtnClick()
		{
			return true;
		}

		bool onEnableBtnClick()
		{
			return true;
		}

	private:
		Application &m_app;
		const CallFwdCondition m_condition;
		const NotiHandler m_delHandler;
		CallFwdNoReplyTime m_noReplyTimeId;
		// gui
		Popup *m_pPopup;
		Editfield *m_pEditfield;
		DoubleTextListItem *m_pTimeoutItem;
	};

	CallFwController::CallFwController(Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pView(nullptr),
		m_pTimeoutPopup(nullptr),
		m_pEditPopup(nullptr)
	{
	}

	CallFwController::~CallFwController()
	{
		Widget::destroy(m_pTimeoutPopup);
		delete m_pEditPopup;
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
				const CallFwdCondition condition = VIEW_ITEMS_COND[i];
				DoubleTextListItem *item = gl.appendItem<DoubleTextListItem>(getConditionLabel(condition));
				if (item) {
					item->setTag(condition);
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

	void CallFwController::onItemClick(WidgetItem &item)
	{
		DBG("View item clicked");
		m_pEditPopup = EditPopup::create(m_app, static_cast<CallFwdCondition>(item.getTag()),
				NotiHandler::wrap<CallFwController, &CallFwController::onEditPopupDel>(this));
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
		RETM_IF(!group,  "Failed to create RadioGroup");

		m_pTimeoutPopup->setContent(*group);

		for (int i = 0; i < TIMEOUT_ITEM_COUNT; ++i) {
			const CallFwdNoReplyTime noReplyTimeId = TIMEOUT_ITEMS_TIME[i];
			group->addItem(getTimeoutLabel(noReplyTimeId), noReplyTimeId);
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

	void CallFwController::onEditPopupDel()
	{
		m_pEditPopup = nullptr;
	}

}}
