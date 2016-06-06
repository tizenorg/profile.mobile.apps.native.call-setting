
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

#include "Controller/RejectMsgListController.h"

#include "gui/Genlist.h"
#include "gui/DoubleTextListItem.h"

namespace CallSettings { namespace Controller {

	using namespace Model;
	using namespace gui;

	namespace {
		const util::TString MSG_ITEMS_HEADER_COUNTER_FMT("%s (%d/6)", false);
		const util::TString MSG_ITEMS_HEADER_TEXT = "IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB";
		const util::TString SELECTED_ITEMS_COUNTER_FMT = "IDS_ST_HEADER_PD_SELECTED";
	}

	class RejectMsgListController::MsgItem {

	public:
		static MsgItem *create(RejectMsgListController &parent, StringKey msgKey)
		{
			MsgItem *obj = new MsgItem(parent, msgKey);
			if (!obj->init()) {
				delete obj;
				return nullptr;
			}

			return obj;
		}

		~MsgItem()
		{
			m_settingsManager.removePropertyHandler(m_msgKey,
				NotiHandler::wrap<MsgItem, &MsgItem::onRejectMsgTextChanged>(this));

			m_pItem->setSelectHandler(nullptr);
			m_pItem->setCheckHandler(nullptr);
			WidgetItem::destroy(m_pItem);
		}

		StringKey getMsgKey() const
		{
			return m_msgKey;
		}

		void setCheckMode(CheckboxListItem::CheckMode checkMode)
		{
			m_pItem->setCheckMode(checkMode);
		}

		void setCheckState(bool isItemChecked)
		{
			m_pItem->setCheckState(isItemChecked);
		}

		bool getCheckState()
		{
			return m_pItem->getCheckState();
		}

	private:
		MsgItem(RejectMsgListController &listController, StringKey msgKey) :
			m_parent(listController),
			m_settingsManager(m_parent.m_app.getSettingsManager()),
			m_msgKey(msgKey),
			m_pItem(nullptr)
		{
		}

		bool init()
		{
			std::string msgText;

			auto res = m_settingsManager.getProperty(m_msgKey, msgText);
			RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to get mgs text for msgId %d", m_msgKey);

			res = m_settingsManager.addPropertyHandler(m_msgKey,
						NotiHandler::wrap<MsgItem, &MsgItem::onRejectMsgTextChanged>(this));
			RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup message change listener!");

			Genlist &gl = m_parent.m_pMsgListView->getGenlist();

			util::TString rejectMsgText(std::move(msgText), msgText.find("IDS_CST") == 0);
			m_pItem = gl.appendItem<DoubleTextListItem>("", std::move(rejectMsgText), true);
			RETVM_IF(!m_pItem, false, "Item create failed!");

			m_pItem->setCheckMode(CheckboxListItem::HIDDEN);
			m_pItem->setSelectHandler(ItemNotiHandler::wrap<MsgItem, &MsgItem::onClicked>(this));
			m_pItem->setCheckHandler(ItemNotiHandler::wrap<MsgItem, &MsgItem::onChecked>(this));

			return true;
		}

		void onRejectMsgTextChanged()
		{
			std::string msgText;
			auto res = m_settingsManager.getProperty(m_msgKey, msgText);
			RETM_IF(res != SETTINGS_RES_SUCCESS, "Failed to get mgs text for msgId %d", m_msgKey);

			m_pItem->setSubText(msgText);
		}

		void onChecked(WidgetItem &item)
		{
			m_parent.onMsgItemChecked(this);
		}

		void onClicked(WidgetItem &item)
		{
			m_parent.onMsgItemClicked(this);
		}

	private:
		RejectMsgListController &m_parent;
		ISettingsManager &m_settingsManager;
		const StringKey m_msgKey;
		gui::DoubleTextListItem *m_pItem;
	};

	RejectMsgListController::RejectMsgListController (Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pMsgListView(nullptr),
		m_pCancelBtn(nullptr),
		m_pDeleteBtn(nullptr),
		m_pSelectAllItem(nullptr),
		m_pMoreMenuPopup(nullptr),
		m_viewMode(DISPLAY_MODE),
		m_pMsgEditorController(nullptr)
	{
	}

	RejectMsgListController::~RejectMsgListController()
	{
		Widget::destroy(m_pMoreMenuPopup);

		m_app.getSettingsManager().removePropertyHandler(INT_KEY_REJECT_MSG_COUNT,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgCountChanged>(this));
	}

	bool RejectMsgListController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");


		auto res = m_app.getSettingsManager().addPropertyHandler(INT_KEY_REJECT_MSG_COUNT,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgCountChanged>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup message count change listener!");

		m_pMsgListView = m_app.getViewManager().pushView<View::GenlistView>();
		RETVM_IF(!m_pMsgListView, false, "Failed to create view");

		m_pMsgListView->getGenlist().setHomogeneousMode(false);
		setBaseView(m_pMsgListView);

		return true;
	}

	void RejectMsgListController::onRejectMsgCountChanged()
	{
		updateMsgList();
	}

	void RejectMsgListController::deleteSelectedMsg()
	{
		if (m_selectedMsgItems.empty()) {
			DBG("No items selected for delete");
			return;
		}

		int currentMsgCount = m_msgItemsArray.size();
		int newMsgCount = currentMsgCount - m_selectedMsgItems.size();

		if (newMsgCount > 0) {
			int msgId = STRING_KEY_REJECT_MSG_ITEM_1;
			for(int i = STRING_KEY_REJECT_MSG_ITEM_1; i < STRING_KEY_REJECT_MSG_ITEM_1 + currentMsgCount; i++) {
				auto it = std::find_if(m_selectedMsgItems.begin(), m_selectedMsgItems.end(), [i] (MsgItem *item) {
							return item->getMsgKey() == i;
					});

				if (it == m_selectedMsgItems.end()) {
					if (i != msgId) {
						std::string msgText;
						m_app.getSettingsManager().getProperty(static_cast<StringKey>(i), msgText);
						m_app.getSettingsManager().setProperty(static_cast<StringKey>(msgId), msgText);
					}
					msgId++;
				}
			}
		}

		m_selectedMsgItems.clear();
		m_app.getSettingsManager().setProperty(INT_KEY_REJECT_MSG_COUNT, newMsgCount);
	}

	void RejectMsgListController::updateMsgList()
	{
		int newMsgCount = 0;
		SettingResultCode res = m_app.getSettingsManager().getProperty(INT_KEY_REJECT_MSG_COUNT, newMsgCount);
		RETM_IF(res != SETTINGS_RES_SUCCESS, "Failed to get msg count");

		if (m_viewMode != DISPLAY_MODE) {
			changeViewToDisplayMode();
		}
		int currentMsgCount = m_msgItemsArray.size();
		if (newMsgCount > currentMsgCount) {
			for (int i = currentMsgCount; i < newMsgCount; i++) {
				addMessageItem(static_cast<StringKey>(STRING_KEY_REJECT_MSG_ITEM_1 + i));
			}
		} else if (newMsgCount < currentMsgCount) {
			m_msgItemsArray.resize(newMsgCount);
		}

		if (newMsgCount == 0) {
			m_pMsgListView->showEmptyView("IDS_CST_NPBODY_NO_MESSAGES", "IDS_CST_BODY_AFTER_YOU_CREATE_CALL_REJECT_MESSAGES_THEY_WILL_BE_SHOWN_HERE");
		} else {
			m_pMsgListView->showListView();
		}

		updateTitleMsgCount();
	}

	void RejectMsgListController::updateView(int updateFlag)
	{
		if (updateFlag & UF_INITIAL) {
			updateMsgList();
			return;
		}

		if (updateFlag & UF_LANGUAGE) {
			updateTitleMsgCount();
		}
	}

	void RejectMsgListController::addMessageItem(StringKey msgKey)
	{
		MsgItem *obj =  MsgItem::create(*this, msgKey);

		RETM_IF(!obj, "Failed to create new MsgItem!");

		if (obj) {
			m_msgItemsArray.push_back(std::unique_ptr<MsgItem>(obj));
		}
	}

	void RejectMsgListController::updateTitleMsgCount()
	{
		if (m_viewMode == SELECT_MODE) {
			m_pMsgListView->getItem().setTitleText(SELECTED_ITEMS_COUNTER_FMT.format(m_selectedMsgItems.size()));
		} else {
			int msgCount = m_msgItemsArray.size();
			if (msgCount > 0) {
				m_pMsgListView->getItem().setTitleText(MSG_ITEMS_HEADER_COUNTER_FMT.format(MSG_ITEMS_HEADER_TEXT.translate(), msgCount));
			} else {
				m_pMsgListView->getItem().setTitleText(MSG_ITEMS_HEADER_TEXT);
			}
		}
	}

	void RejectMsgListController::changeViewToSelectMode()
	{
		RETM_IF(m_viewMode == SELECT_MODE, "Select mode is already applied!");

		auto cancelBtn = createDeleteButton();
		auto deleteBtn = createCancelButton();
		auto selectAllItem = createSelectAllItem();
		if (!deleteBtn || !cancelBtn || !selectAllItem) {
			ERR("Failed to setup select mode!");
			Widget::destroy(deleteBtn);
			Widget::destroy(cancelBtn);
			WidgetItem::destroy(selectAllItem);
			return;
		}

		m_pDeleteBtn = deleteBtn;
		m_pCancelBtn = cancelBtn;
		m_pSelectAllItem = selectAllItem;

		m_pMsgListView->getItem().setTitleLeftBtn(*m_pCancelBtn);
		m_pMsgListView->getItem().setTitleRightBtn(*m_pDeleteBtn);

		for (auto &item : m_msgItemsArray) {
			item->setCheckMode(CheckboxListItem::AUTO_CHECK);
			item->setCheckState(false);
		}

		m_viewMode = SELECT_MODE;
		updateTitleMsgCount();
	}

	void RejectMsgListController::changeViewToDisplayMode()
	{
		RETM_IF(m_viewMode == DISPLAY_MODE, "Display mode is already applied!");

		m_pMsgListView->getItem().unsetTitleLeftBtn();
		m_pMsgListView->getItem().unsetTitleRightBtn();

		WidgetItem::destroy(m_pSelectAllItem);
		m_pSelectAllItem = nullptr;

		Widget::destroy(m_pCancelBtn);
		m_pCancelBtn = nullptr;

		Widget::destroy(m_pDeleteBtn);
		m_pDeleteBtn = nullptr;

		for (auto &item : m_msgItemsArray) {
				item->setCheckMode(CheckboxListItem::HIDDEN);
		}

		m_viewMode = DISPLAY_MODE;
		m_selectedMsgItems.clear();

		updateTitleMsgCount();
	}

	Button *RejectMsgListController::createDeleteButton()
	{
		Button *cancelBtn = Widget::create<Button>(m_pMsgListView->getContent(), "naviframe/title_left", "IDS_MSG_ACBUTTON_CANCEL_ABB");
		RETVM_IF(!cancelBtn, nullptr, "Internal error: failed to create Cancel button");

		cancelBtn->setClickHandler(WidgetNotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onCancelButtonClick>(this));

		return cancelBtn;
	}

	Button *RejectMsgListController::createCancelButton()
	{
		Button *deleteBtn = Widget::create<Button>(m_pMsgListView->getContent(), "naviframe/title_right", "IDS_MSG_ACBUTTON_DELETE_ABB");
		RETVM_IF(!deleteBtn, nullptr, "Internal error: failed to create Delete button");

		deleteBtn->setClickHandler(WidgetNotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onDeleteButtonClick>(this));
		return deleteBtn;
	}

	CategoryListItem *RejectMsgListController::createSelectAllItem()
	{
		CategoryListItem *selectAllItem = m_pMsgListView->getGenlist().prependItem<CategoryListItem>("IDS_COM_BODY_SELECT_ALL");
		RETVM_IF(!selectAllItem, nullptr, "Failed to add select all item");

		selectAllItem->setCheckMode(CheckboxListItem::AUTO_CHECK);
		selectAllItem->setCheckHandler(ItemNotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onSelectAllChecked>(this));

		return selectAllItem;

	}

	void RejectMsgListController::onDeleteButtonClick(Widget &item)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in display mode, skip click event!");

		deleteSelectedMsg();
	}

	void RejectMsgListController::onCancelButtonClick(Widget &item)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in display mode, skip click event!");

		changeViewToDisplayMode();
	}

	void RejectMsgListController::onSelectAllChecked(WidgetItem &item)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in display mode, skip click event!");

		m_selectedMsgItems.clear();

		if (m_pSelectAllItem->getCheckState()) {
			for (auto &item : m_msgItemsArray) {
				m_selectedMsgItems.push_back(item.get());
				item->setCheckState(true);
			}
		} else {
			for (auto &item : m_msgItemsArray) {
				item->setCheckState(false);
			}
		}

		updateTitleMsgCount();
		updateDeleteBtnState();
	}

	void RejectMsgListController::updateSelectAllItemState()
	{
		if (m_selectedMsgItems.size() == m_msgItemsArray.size()) {
			m_pSelectAllItem->setCheckState(true);
		} else {
			m_pSelectAllItem->setCheckState(false);
		}
	}

	void RejectMsgListController::updateDeleteBtnState()
	{
		if (m_selectedMsgItems.size() > 0) {
			m_pDeleteBtn->setDisabled(false);
		} else {
			m_pDeleteBtn->setDisabled(true);
		}
	}

	void RejectMsgListController::onMsgItemClicked(MsgItem *item)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != DISPLAY_MODE, "View is not in display mode!");

		m_pMsgEditorController =  ViewController::create<Controller::RejectMsgEditorController>(m_app,
						NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onEditorControllerDestroy>(this),
						item->getMsgKey());

	}

	void RejectMsgListController::onMsgItemChecked(MsgItem *item)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is not in select mode!");

		if (item->getCheckState()) {
			m_selectedMsgItems.push_back(item);
		} else {
			m_selectedMsgItems.erase(std::remove(m_selectedMsgItems.begin(), m_selectedMsgItems.end(), item),
					m_selectedMsgItems.end());
		}

		updateDeleteBtnState();
		updateSelectAllItemState();
		updateTitleMsgCount();
	}

	void RejectMsgListController::showMoreMenuPopup()
	{
		Widget::destroy(m_pMoreMenuPopup);

		m_pMoreMenuPopup = Widget::create<OptionMenuPopup>(WidgetWrapper(m_app.getViewManager().getWindow()));
		RETM_IF(!m_pMoreMenuPopup, "Failed to create menu popup!");

		int msgTotalCount = m_msgItemsArray.size();
		if (msgTotalCount < REJECT_MSG_MAX_COUNT) {
			m_pMoreMenuPopup->addItem(NotiHandler::wrap<RejectMsgListController,
					&RejectMsgListController::onMenuOptionCreateClick>(this), "IDS_CST_SK3_CREATE");
		}

		if (msgTotalCount > 0) {
			m_pMoreMenuPopup->addItem(NotiHandler::wrap<RejectMsgListController,
					&RejectMsgListController::onMenuOptionDeleteClick>(this), "IDS_CST_HEADER_DELETE");
		}

		m_pMoreMenuPopup->setDestroyHandler(
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onMenuPopupDestroy>(this));

		m_pMoreMenuPopup->show();
	}

	void RejectMsgListController::onMenuOptionDeleteClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		changeViewToSelectMode();
	}

	void RejectMsgListController::onMenuOptionCreateClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		m_pMsgEditorController =  ViewController::create<Controller::RejectMsgEditorController>(m_app,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onEditorControllerDestroy>(this));
	}

	void RejectMsgListController::onMenuPopupDestroy()
	{
		m_pMoreMenuPopup = nullptr;
	}

	void RejectMsgListController::onEditorControllerDestroy()
	{
		delete m_pMsgEditorController;
		m_pMsgEditorController = nullptr;
	}

	void RejectMsgListController::onBackKeyPressed()
	{
		if (m_viewMode == SELECT_MODE) {
			changeViewToDisplayMode();
		} else {
			makeDestroyReqeuest();
		}
	}

	void RejectMsgListController::onMenuKeyPressed()
	{
		RETM_IF(!m_isActivated, "View is not activse, skip click event!");
		if (m_viewMode == SELECT_MODE) {
			return;
		}

		showMoreMenuPopup();
	}

} }
