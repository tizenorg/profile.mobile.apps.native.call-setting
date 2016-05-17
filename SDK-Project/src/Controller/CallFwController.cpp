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
#include "gui/Widgets/PendingPopupContent.h"

#include "View/PhoneEditListItem.h"

#include "Controller/Utils.h"
#include "Controller/CallFwController.h"
#include "Controller/TelNumberPicker.h"

#include "Model/Telephony/TelRequestListener.h"

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
		const util::TString FWD_CONDITION_TIMEOUT_ITEM_FMT = "<color=" GUI_COLOR_SUBITEM_TEXT_HEX ">%s, %s</color>";

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
			return ITEM_SUB_TEXT_COLOR_FMT.format(getTimeoutLabel(noReplyTimeId).getCStr());
		}

		util::TString getConditionSublabel(const util::TString &telNumber)
		{
			return ITEM_SUB_TEXT_COLOR_FMT.format(telNumber.translate());
		}

		util::TString getConditionTimeoutSublabel(const util::TString &telNumber, CallFwdNoReplyTime noReplyTimeId)
		{
			return FWD_CONDITION_TIMEOUT_ITEM_FMT.format(
					telNumber.getCStr() , getTimeoutLabel(noReplyTimeId).getCStr());
		}

	}

	class CallFwController::EditPopup {
	public:
		~EditPopup()
		{
			Widget::destroy(m_pTimeoutPopup);
			if (m_pPopup) {
				m_pPopup->setDestroyHandler(nullptr);
				Widget::destroy(m_pPopup);
			}

			delete m_pTelNumberPicker;

			if (m_delHandler.assigned()) {
				m_delHandler();
			}
		}

		static EditPopup *create(Application &app, CallFwdReqData &callFwdData, NotiHandler delHandler, NotiHandler enableClickHandler)
		{
			EditPopup *obj = new EditPopup(app, callFwdData, delHandler, enableClickHandler);
			if (!obj->init()) {
				delete obj;
				return nullptr;
			}
			return obj;
		}

	private:
		EditPopup(Application &app, CallFwdReqData &callFwdData, NotiHandler delHandler, NotiHandler enableClickHandler) :
			m_app(app),
			m_callFwdReqData(callFwdData),
			m_noReplyTimeId(m_callFwdReqData.waitTime),
			m_pTelNumberPicker(nullptr),
			m_delHandler(delHandler),
			m_enableClickHandler(enableClickHandler),
			m_pPopup(nullptr),
			m_pTimeoutPopup(nullptr),
			m_pEditfield(nullptr),
			m_pContactButton(nullptr),
			m_pTimeoutItem(nullptr)
		{
		}

		bool init()
		{
			m_pPopup = Widget::create<Popup>(WidgetWrapper(m_app.getViewManager().getNaviframe()),
					getConditionLabel(m_callFwdReqData.condition));

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
			m_pEditfield->setInputLimit(0, TELEPHONY_NUMBER_LENGTH_MAX);
			m_pEditfield->setInputCharRestriction(TELEPHONY_NUMBER_ALLOWED_SYMBOLS, "");
			m_pEditfield->setInputEventHandler(NotiHandler::wrap<EditPopup, &EditPopup::onInputEvent>(this));
			m_pEditfield->setSipReturnClickHandler(NotiHandler::wrap<EditPopup, &EditPopup::onSipReturnClick>(this));

			m_pContactButton = &editItem->getContactButton();
			m_pContactButton->setClickHandler(WidgetNotiHandler::wrap<
					EditPopup, &EditPopup::onContactBtnClick>(this));

			if (m_callFwdReqData.mode == TELEPHONY_CF_MODE_ACTIVATE
					|| m_callFwdReqData.mode == TELEPHONY_CF_MODE_REGISTER) {
				m_pEditfield->setEntryRawText(m_callFwdReqData.telNumber);
			} else {
				m_pEditfield->setEntryRawText("");
			}

			m_pTelNumberPicker = TelNumberPicker::create();
			RETVM_IF(!m_pTelNumberPicker, false, "Failed to create TelNumberPicker");


			if (m_callFwdReqData.condition == TELEPHONY_CF_IF_NO_REPLY) {
				if (m_noReplyTimeId == TELEPHONY_CF_NO_REPLY_UNDEFINED) {
					m_noReplyTimeId = TELEPHONY_CF_NO_REPLY_15_SEC;
				}

				m_pTimeoutItem = genlist->appendItem<DoubleTextListItem>(
						"IDS_CST_BODY_WAITING_TIME", getColoredTimeoutLabel(m_noReplyTimeId));

				RETVM_IF(!m_pTimeoutItem, false, "Failed to create timeout item");
				m_pTimeoutItem->setCheckMode(CheckboxListItem::HIDDEN);
				m_pTimeoutItem->setSelectHandler(ItemNotiHandler::wrap<
						EditPopup, &EditPopup::onTimeoutItemSelect>(this));
			}

			m_pPopup->setDestroyHandler(NotiHandler::wrap<EditPopup, &EditPopup::onPopupDel>(this));
			m_pPopup->setBlockClickHandler(PopupClickHandler::wrap<
					EditPopup, &EditPopup::onPopupBlockClick>(this));

			return true;
		}

		void updateEnableBtnState()
		{
			bool disabledState = !canDataBeApplied();
			m_pPopup->setButtonDisabled(POPUP_BUTTON_RIGHT, disabledState);
			m_pEditfield->setSipReturnKeydDisabled(disabledState);
		}

		bool canDataBeApplied()
		{
			std::string newTelNumber = m_pEditfield->getEntryRawText();

			if (newTelNumber.empty()) {
				return false;
			}

			if (newTelNumber.compare(m_callFwdReqData.telNumber) == 0 &&
					m_noReplyTimeId == m_callFwdReqData.waitTime) {
				return false;
			} else {
				return true;
			}
		}

		void applyNewData()
		{
			if (!canDataBeApplied()) {
				ERR("Input data is invalid");
			}

			m_callFwdReqData.telNumber = m_pEditfield->getEntryRawText();
			if (m_callFwdReqData.condition == TELEPHONY_CF_IF_NO_REPLY) {
				m_callFwdReqData.waitTime = m_noReplyTimeId;
			}

			if (m_enableClickHandler.assigned()) {
				m_enableClickHandler();
			}
		}

		// Editfield Events //
		void onInputEvent()
		{
			updateEnableBtnState();
		}

		// Main popup events
		void onPopupDel()
		{
			m_pPopup = nullptr;
			delete this;
		}

		bool onPopupBlockClick()
		{
			return false;
		}

		bool onCancelBtnClick()
		{
			return true;
		}

		void onSipReturnClick()
		{
			applyNewData();
			delete this;
		}

		bool onEnableBtnClick()
		{
			applyNewData();
			return true;
		}

		void onContactBtnClick(Widget &button)
		{
			auto result = m_pTelNumberPicker->launch(TelNumberPicker::ResultEventHandler::wrap<
					EditPopup, &EditPopup::contactsReplyHandler>(this));
			if (result != TelNumberPicker::NUM_PICKER_RES_SUCCESS) {
				//TODO Add toast popup about contact launch faile
				ERR("Failed to launch contacts!");
			}
		}

		void contactsReplyHandler(TelNumberPicker::NumPickerResCode result, std::string telNumber)
		{
			if (result == TelNumberPicker::NUM_PICKER_RES_SUCCESS) {
					m_pEditfield->setEntryRawText(telNumber);
			} else {
				ERR("Failed to get telNumber!");
			}
		}

		void onTimeoutItemSelect(WidgetItem &item)
		{
			showTimeoutPopup();
		}

		void showTimeoutPopup()
		{
			Widget::destroy(m_pTimeoutPopup);

			m_pTimeoutPopup = Widget::create<Popup>(WidgetWrapper(m_app.getViewManager().getWindow()),
					"IDS_CST_BODY_WAITING_TIME");
			RETM_IF(!m_pTimeoutPopup, "Failed to create timeout popup!");

			m_pTimeoutPopup->setDestroyHandler(NotiHandler::wrap<
					EditPopup, &EditPopup::onTimeoutPopupDel>(this));

			RadioGroup *group = Widget::create<RadioGroup>(*m_pTimeoutPopup);
			RETM_IF(!group,  "Failed to create RadioGroup");

			m_pTimeoutPopup->setContent(*group);

			for (int i = 0; i < TIMEOUT_ITEM_COUNT; ++i) {
				const CallFwdNoReplyTime noReplyTimeId = TIMEOUT_ITEMS_TIME[i];
				group->addItem(getTimeoutLabel(noReplyTimeId), noReplyTimeId);
			}

			group->selectRadioItem(m_noReplyTimeId);

			group->setSelectedCallback(RadioSelectHandler::wrap<
					EditPopup, &EditPopup::onTimeoutValueSelected>(this));
		}

		// Timeout select popup events

		void onTimeoutPopupDel()
		{
			m_pTimeoutPopup = nullptr;
		}

		void onTimeoutValueSelected(int noReplyTimeId)
		{
			INF("noReplyTimeId: %d", noReplyTimeId);
			Widget::destroy(m_pTimeoutPopup);
			m_noReplyTimeId = static_cast<CallFwdNoReplyTime>(noReplyTimeId);
			m_pTimeoutItem->setSubText(getColoredTimeoutLabel(m_noReplyTimeId));
			updateEnableBtnState();
		}

	private:
		Application &m_app;
		CallFwdReqData &m_callFwdReqData;
		CallFwdNoReplyTime m_noReplyTimeId;
		TelNumberPicker *m_pTelNumberPicker;
		const NotiHandler m_delHandler;
		const NotiHandler m_enableClickHandler;

		// gui
		Popup *m_pPopup;
		Popup *m_pTimeoutPopup;
		Editfield *m_pEditfield;
		Button *m_pContactButton;
		DoubleTextListItem *m_pTimeoutItem;
	};

	class CallFwController::Item :
			private Model::TelRequestListener<CallFwdReqData>,
			private Model::SimpleTelRequestListener
	{

	public:

		typedef util::Delegate<void(Item *)> ItemChangeHandler;

		~Item()
		{
			delete m_pEditPopup;
		}

		static Item *create(Application &app, CallFwdCondition condition, Genlist &gl,
				ItemChangeHandler updateFinishedHandler,
				ItemChangeHandler changeBeginHandler,
				ItemChangeHandler changeEndHandler)
		{
			Item *obj = new Item(app, condition, updateFinishedHandler, changeBeginHandler, changeEndHandler);
			if (!obj->init(gl)) {
				delete obj;
				return nullptr;
			}
			return obj;
		}

		void update()
		{
			if (!Model::TelRequestListener<CallFwdReqData>::isAttached()) {
				if (Model::SimpleTelRequestListener::isAttached()) {
					Model::SimpleTelRequestListener::cancelRequest();
				}
				requestFwdState();
			} else {
				DBG("Setup is in progress, skip update request");
			}
		}

		void setDisabled(bool isDisabled)
		{
			if (m_isDisabled != isDisabled) {
				m_pItem->setDisabled(isDisabled);
				m_isDisabled = isDisabled;
			}
		}

		CallForwardMode getOptionMode() const
		{
			return m_callFwdReqData.mode;
		}

	private:
		Item(Application &app, CallFwdCondition condition,
				ItemChangeHandler updateFinishedHandler, ItemChangeHandler changeBeginHandler, ItemChangeHandler changeEndHandler):
			m_app(app),
			m_pItem(nullptr),
			m_pEditPopup(nullptr),
			m_isDisabled(false),
			m_updateFinihedHandler(updateFinishedHandler),
			m_changeBeginHandler(changeBeginHandler),
			m_changeEndHandler(changeEndHandler)
		{

			m_callFwdReqData.callType = TELEPHONY_CALLTYPE_VOICE;
			m_callFwdReqData.condition = condition;
		}

		bool init(Genlist &gl)
		{
			m_pItem = gl.appendItem<DoubleTextListItem>(getConditionLabel(m_callFwdReqData.condition));
			RETVM_IF(!m_pItem, false, "Item create failed!");

			m_pItem->setCheckboxStyle(CHECKBOX_SWITCHER);
			m_pItem->setCheckMode(CheckboxListItem::SKIP_EVENTS);
			m_pItem->setSelectHandler(ItemNotiHandler::wrap<
					Item, &Item::onItemClick>(this));
			m_pItem->setCheckHandler(ItemNotiHandler::wrap<
					Item, &Item::onItemCheck>(this));

			return true;
		}

		void requestFwdState()
		{
			m_callFwdReqData.mode = TELEPHONY_CF_MODE_UNDEFINED;
			m_callFwdReqData.telNumber = "";
			m_callFwdReqData.waitTime = TELEPHONY_CF_NO_REPLY_UNDEFINED;

			m_pItem->setCheckMode(CheckboxListItem::PENDING);
			m_pItem->setSubText(nullptr);

			TelResultCode res = m_app.getTelephonyManager().requestCallFwdState(&m_callFwdReqData, this);
			if (res != TELEPHONY_RES_SUCCESS) {
				ERR("Failed to send state request, error code: %d", res);
				//TODO
				//Add toast popup here about fail
			}
		}

		void requestFwdSetup(CallForwardMode setupMode)
		{
			m_pItem->setCheckMode(CheckboxListItem::PENDING);
			m_pItem->setSubText(nullptr);

			if (m_changeBeginHandler.assigned()) {
				m_changeBeginHandler(this);
			}

			m_callFwdReqData.mode = setupMode;
			TelResultCode res = m_app.getTelephonyManager().requestCallFwdSetup(&m_callFwdReqData, this);
			if (res != TELEPHONY_RES_SUCCESS) {
				ERR("Failed to send setup request, error code: %d", res);
				//TODO
				//Add toast popup here about fail
				if (m_changeEndHandler.assigned()) {
					m_changeEndHandler(this);
				}
			}
		}

		void disableFwd()
		{
			requestFwdSetup(TELEPHONY_CF_MODE_DEACTIVATE);
		}

		void enableFwd()
		{
			requestFwdSetup(TELEPHONY_CF_MODE_REGISTER);
		}

		virtual void onRequestComplete(TelResultCode result, const CallFwdReqData *data)
		{
			if (result != TELEPHONY_RES_SUCCESS) {
				ERR("Failed to get Call Forward state for condition %d, result code: %d!", m_callFwdReqData.condition, result);
				//TODO
				//Add toast popup here about fail
			}

			refreshView();

			if (m_updateFinihedHandler.assigned()) {
				m_updateFinihedHandler(this);
			}
		}

		virtual void onRequestComplete(TelResultCode result)
		{
			if (result != TELEPHONY_RES_SUCCESS) {
				ERR("Failed to setup Call Forward for condition %d, result code: %d!", m_callFwdReqData.condition, result);
				// TODO
				// Add toast popup about reject by network here in future
			}

			if (m_changeEndHandler.assigned()) {
				m_changeEndHandler(this);
			}
		}

		void refreshView()
		{
			if (m_callFwdReqData.mode == TELEPHONY_CF_MODE_ACTIVATE) {
				m_pItem->setCheckState(true);
				m_pItem->setCheckMode(CheckboxListItem::NORMAL);

				if (m_callFwdReqData.telNumber.empty()) {
					m_pItem->setSubText(getConditionSublabel("IDS_COM_BODY_UNKNOWN"));
				} else if (m_callFwdReqData.condition == TELEPHONY_CF_IF_NO_REPLY) {
					m_pItem->setSubText(getConditionTimeoutSublabel(m_callFwdReqData.telNumber, m_callFwdReqData.waitTime));
				} else {
					m_pItem->setSubText(getConditionSublabel(util::TString(m_callFwdReqData.telNumber, false)));
				}
			} else {
				m_pItem->setCheckState(false);
				m_pItem->setCheckMode(CheckboxListItem::SKIP_EVENTS);
				m_pItem->setSubText(nullptr);
			}
		}

		void onItemCheck(WidgetItem &item)
		{
			RETM_IF(m_isDisabled, "Item is disabled, skip event!");

			disableFwd();
		}

		void onItemClick(WidgetItem &item)
		{
			RETM_IF(m_isDisabled, "Item is disabled, skip event!");

			m_pEditPopup = EditPopup::create(m_app, m_callFwdReqData,
					NotiHandler::wrap<Item, &Item::onEditPopupDel>(this),
					NotiHandler::wrap<Item, &Item::onEditPopupEnableClick>(this));
		}

		void onEditPopupDel()
		{
			m_pEditPopup = nullptr;
		}

		void onEditPopupEnableClick()
		{
			enableFwd();
		}

	private:
		Application &m_app;
		DoubleTextListItem *m_pItem;
		CallFwdReqData m_callFwdReqData;
		EditPopup *m_pEditPopup;
		bool m_isDisabled;
		ItemChangeHandler m_updateFinihedHandler;
		ItemChangeHandler m_changeBeginHandler;
		ItemChangeHandler m_changeEndHandler;
	};

	CallFwController::CallFwController(Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pView(nullptr),
		m_pPendingPopup(nullptr),
		m_pUncondFwdItem(nullptr),
		m_isItemChanging(false),
		m_updatingItemsCount(0),
		m_statusChangeCount(0)
	{
	}

	CallFwController::~CallFwController()
	{
		Widget::destroy(m_pPendingPopup);
		m_app.getTelephonyManager().removeCallFwdChangeHandler(
				NotiHandler::wrap<CallFwController, &CallFwController::onFwdStatusChanged>(this));
		for (auto item : m_fwdItemsArray) {
			delete item;
		}
	}

	bool CallFwController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		m_app.getTelephonyManager().addCallFwdChangeHandler(
				NotiHandler::wrap<CallFwController, &CallFwController::onFwdStatusChanged>(this));
		m_pView = m_app.getViewManager().pushView<View::GenlistView>();
		RETVM_IF(!m_pView, false, "Failed to create view");

		setBaseView(m_pView);

		return true;
	}

	void CallFwController::onFwdStatusChanged()
	{
		if (m_isItemChanging) {
			m_statusChangeCount++;
		} else {
			addCustomUpdateFlags(UF_FWD_STATUS_CHANGE);
		}
	}

	void CallFwController::updateFwdOptions()
	{
		showPendingPopup();

		m_updatingItemsCount = m_fwdItemsArray.size();
		for (auto item : m_fwdItemsArray) {
			item->update();
		}
	}

	void CallFwController::updateView(int updateFlag)
	{
		if (updateFlag & UF_INITIAL) {

			m_pView->getItem().setTitleText("IDS_CST_BODY_CALL_FORWARDING");

			Genlist &gl = m_pView->getGenlist();

			for (int i = 0; i < VIEW_ITEM_COUNT; ++i) {
				const CallFwdCondition condition = VIEW_ITEMS_COND[i];
				Item *obj =  Item::create(m_app, condition, gl,
						Item::ItemChangeHandler::wrap<CallFwController, &CallFwController::onFwdOptionReady>(this),
						Item::ItemChangeHandler::wrap<CallFwController, &CallFwController::onFwdOptionChangeBegin>(this),
						Item::ItemChangeHandler::wrap<CallFwController, &CallFwController::onFwdOptionChangeEnd>(this)
					);

				if (obj) {
					m_fwdItemsArray.push_back(obj);
					if (condition == TELEPHONY_CF_UNCONDITIONAL) {
						m_pUncondFwdItem = obj;
					}
				} else {
					ERR("Item create failed!");
				}
			}

			updateFwdOptions();
		}

		if (updateFlag & UF_FWD_STATUS_CHANGE) {
			updateFwdOptions();
		}
	}

	void CallFwController::onFwdOptionReady(Item *item)
	{
		DBG("onFwdOptionReady");
		m_updatingItemsCount--;

		if (m_updatingItemsCount == 0) {
			DBG("All options are ready!");

			if (m_pPendingPopup) {
				Widget::destroy(m_pPendingPopup);
				m_pPendingPopup = nullptr;
			}

			for (auto item : m_fwdItemsArray) {
				if (m_pUncondFwdItem && m_pUncondFwdItem->getOptionMode() == TELEPHONY_CF_MODE_ACTIVATE &&
						item != m_pUncondFwdItem) {
					item->setDisabled(true);
				} else {
					item->setDisabled(false);
				}
			}
		}
	}

	void CallFwController::onFwdOptionChangeBegin(Item *item)
	{
		DBG("onFwdOptionChangeBegin");
		m_isItemChanging = true;

		for (auto item : m_fwdItemsArray) {
			item->setDisabled(true);
		}
	}

	void CallFwController::onFwdOptionChangeEnd(Item *item)
	{
		DBG("onFwdOptionChangeEnd");

		bool isSyncNeeded = (m_statusChangeCount > 1);
		m_statusChangeCount = 0;
		m_isItemChanging = false;

		if (isSyncNeeded) {
			DBG("Another update was performed in process of fwd setup, need sync state!");
			addCustomUpdateFlags(UF_FWD_STATUS_CHANGE);
		} else if (!isUpdateFlagsSet(UF_FWD_STATUS_CHANGE)) {
			m_updatingItemsCount = 1;
			item->update();
		}
	}

	void CallFwController::showPendingPopup()
	{
		Widget::destroy(m_pPendingPopup);

		m_pPendingPopup = Widget::create<Popup>(WidgetWrapper(m_app.getViewManager().getWindow()));
		RETM_IF(!m_pPendingPopup, "Failed to create pending popup!");

		m_pPendingPopup->setDestroyHandler(NotiHandler::wrap<
				CallFwController, &CallFwController::onPendingPopupDel>(this));
		m_pPendingPopup->setBlockClickHandler(PopupClickHandler::wrap<
				CallFwController, &CallFwController::onPendingPopupBlock>(this));
		m_pPendingPopup->setBackClickHandler(PopupClickHandler::wrap<
				CallFwController, &CallFwController::onPendingPopupBack>(this));

		PendingPopupContent *content = Widget::create<PendingPopupContent>(*m_pPendingPopup,
				"IDS_COM_POP_LOADING_ING");
		RETM_IF(!content,  "Failed to create PendingPopupContent");

		m_pPendingPopup->setContent(*content);
	}

	void CallFwController::onPendingPopupDel()
	{
		m_pPendingPopup = nullptr;
	}

	bool CallFwController::onPendingPopupBlock()
	{
		return false;
	}

	bool CallFwController::onPendingPopupBack()
	{
		if (m_isActivated) {
			makeDestroyReqeuest();
		}
		return false;
	}

}}
