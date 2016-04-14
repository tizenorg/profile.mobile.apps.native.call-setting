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

#include "Controller/RejectMsgListView/RejectMsgListController.h"
#include "View/RejectMsgListView/RejectMsgListView.h"

namespace CallSettings { namespace Controller {

	using namespace Model;
	using namespace View;

	RejectMsgListController::RejectMsgListController (Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pMsgListView(nullptr),
		m_viewMode(DISPLAY_MODE),
		m_msgTotalCount(0),
		m_needUpdate(true),
		m_isOptionMenuShown(false),
		m_isMsgListLocked(false)
	{
	}

	RejectMsgListController::~RejectMsgListController()
	{
		m_app.getSettingsManager().removePropertyHandler(INT_KEY_REJECT_MSG_COUNT,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgCountChanged>(this));

		for(int key = STRING_KEY_REJECT_MSG_ITEM_1; key <= STRING_KEY_REJECT_MSG_ITEM_6; key++) {
			m_app.getSettingsManager().removePropertyHandler(static_cast<StringKey>(key),
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgTextChanged>(this));
		}
	}

	bool RejectMsgListController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");


		auto res = m_app.getSettingsManager().addPropertyHandler(INT_KEY_REJECT_MSG_COUNT,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgCountChanged>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup message count change listener!");

		for(int key = STRING_KEY_REJECT_MSG_ITEM_1; key <= STRING_KEY_REJECT_MSG_ITEM_6; key++) {
			res = m_app.getSettingsManager().addPropertyHandler(static_cast<StringKey>(key),
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgTextChanged>(this));
			RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup mgs content listener");
		}

		m_pMsgListView =  gui::ViewManager::pushView<View::RejectMsgListView>(m_app.getViewManager(), true);
		RETVM_IF(!m_pMsgListView, false, "Failed to create view instance!");
		m_pMsgListView->setMsgClickHandler(
				RejectMsgListView::MsgClickHandler::wrap<RejectMsgListController, &RejectMsgListController::onMsgItemClicked>(this));

		setBaseView(m_pMsgListView);

		return true;
	}

	void RejectMsgListController::onRejectMsgCountChanged()
	{
		if (m_isMsgListLocked) {
			return;
		}

		if (m_isVisible) {
			updateRejectMsgList();
		} else {
			m_needUpdate = true;
		}
	}

	void RejectMsgListController::onRejectMsgTextChanged()
	{
		if (m_isMsgListLocked) {
			return;
		}

		if (m_isVisible) {
			updateRejectMsgList();
		} else {
			m_needUpdate = true;
		}
	}

	void RejectMsgListController::onMsgItemSelected(int msgId, bool isSelected)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in select mode, skip select event!");

		if (isSelected) {
			m_msgDeleteList.push_back(msgId);
		} else {
			m_msgDeleteList.erase(std::remove(m_msgDeleteList.begin(), m_msgDeleteList.end(), msgId),
				m_msgDeleteList.end());
		}

		if (m_msgDeleteList.empty()) {
			m_pMsgListView->setDisabledDeleteBtn(true);
		} else {
			m_pMsgListView->setDisabledDeleteBtn(false);
		}

		bool isSelectAllChecked = m_pMsgListView->isSelectAllChecked();
		if (m_msgDeleteList.size() == m_msgTotalCount && !isSelectAllChecked) {
			m_pMsgListView->setSelectAllChecked(true);

		} else if(m_msgDeleteList.size() != m_msgTotalCount && isSelectAllChecked) {
			m_pMsgListView->setSelectAllChecked(false);
		}

		m_pMsgListView->updateTitleText(m_msgDeleteList.size());
	}

	void RejectMsgListController::onSelectAllItemChecked(bool isSelected)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in display mode, skip click event!");

		m_msgDeleteList.clear();

		if (isSelected) {
			for(int i = STRING_KEY_REJECT_MSG_ITEM_1; i < STRING_KEY_REJECT_MSG_ITEM_1 + m_msgTotalCount; i++) {
				m_msgDeleteList.push_back(i);
			}
			m_pMsgListView->setAllMsgSelected(true);
			m_pMsgListView->updateTitleText(m_msgTotalCount);
			m_pMsgListView->setDisabledDeleteBtn(false);
		} else {
			m_pMsgListView->setAllMsgSelected(false);
			m_pMsgListView->updateTitleText(0);
			m_pMsgListView->setDisabledDeleteBtn(true);
		}
	}

	void RejectMsgListController::onMsgItemClicked(int msgId)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != DISPLAY_MODE, "View is in edit mode, skip click event!");

		//TODO Msg Edit View will be called here later.

	}

	void RejectMsgListController::deleteSelectedMsg()
	{
		if (m_msgDeleteList.empty()) {
			DBG("No items selected for delete");
			return;
		}

		int deletedMsgCount = m_msgDeleteList.size();
		int newMsgCount = m_msgTotalCount - deletedMsgCount;
		m_isMsgListLocked = true;
		if (newMsgCount > 0) {
			int key = STRING_KEY_REJECT_MSG_ITEM_1;
			for(int i = STRING_KEY_REJECT_MSG_ITEM_1; i < STRING_KEY_REJECT_MSG_ITEM_1 + m_msgTotalCount; i++) {
				if (std::find(m_msgDeleteList.begin(), m_msgDeleteList.end(), i) == m_msgDeleteList.end()) {
					std::string msgText;
					m_app.getSettingsManager().getProperty(static_cast<StringKey>(i), msgText);
					m_app.getSettingsManager().setProperty(static_cast<StringKey>(key), msgText);
					key++;
				}
			}
		}

		m_app.getSettingsManager().setProperty(INT_KEY_REJECT_MSG_COUNT, newMsgCount);
		m_isMsgListLocked = false;
		updateRejectMsgList();
	}

	void RejectMsgListController::updateRejectMsgList()
	{
		m_needUpdate = false;

		SettingResultCode res = m_app.getSettingsManager().getProperty(INT_KEY_REJECT_MSG_COUNT, m_msgTotalCount);
		RETM_IF(res != SETTINGS_RES_SUCCESS, "Failed to get mgs count");

		if (m_viewMode == SELECT_MODE) {
			disableSelectMode();
		}

		m_pMsgListView->invalidateMsgList();
		if (m_msgTotalCount > 0) {
			m_pMsgListView->updateTitleText(m_msgTotalCount);
			for (int key = STRING_KEY_REJECT_MSG_ITEM_1; key < STRING_KEY_REJECT_MSG_ITEM_1 + m_msgTotalCount; key++) {
				std::string msgText;
				res = m_app.getSettingsManager().getProperty(static_cast<StringKey>(key), msgText);

				if(res == SETTINGS_RES_SUCCESS && !msgText.empty()) {
					m_pMsgListView->addListItem(key, msgText);
				} else {
					ERR("Failed to get message %d", key);
				}
			}
		}
	}

	void RejectMsgListController::onBackKeyPressed()
	{
		if (m_viewMode == SELECT_MODE) {
			disableSelectMode();
		} else {
			makeDestroyReqeuest();
		}
	}

	void RejectMsgListController::onMenuKeyPressed()
	{
		if (m_viewMode != SELECT_MODE) {
			enableSelectMode();
			m_viewMode = SELECT_MODE;
		} else {
			disableSelectMode();
			m_viewMode = DISPLAY_MODE;
		}
	}

	void RejectMsgListController::onDeleteBtnClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		deleteSelectedMsg();
	}

	void RejectMsgListController::onCancelBtnClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		disableSelectMode();
	}

	void RejectMsgListController::onMenuOptionDeleteClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		enableSelectMode();
	}

	void RejectMsgListController::onMenuOptionCreateClick()
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");

		//TODO Edit reject MSG view will be created here
	}


	void RejectMsgListController::disableSelectMode()
	{
		RETM_IF(!m_pMsgListView->disableSelectMode(), "Failed to setup disaply mode");
		m_viewMode = DISPLAY_MODE;
		m_msgDeleteList.clear();
		m_pMsgListView->updateTitleText(m_msgTotalCount);
		m_pMsgListView->setMsgSelectHandler(nullptr);
		m_pMsgListView->setMsgClickHandler(
				RejectMsgListView::MsgClickHandler::wrap<RejectMsgListController, &RejectMsgListController::onMsgItemClicked>(this));
	}

	void RejectMsgListController::enableSelectMode()
	{
		RETM_IF(!m_pMsgListView->enableSelectMode(), "Failed to setup edit mode");

		m_viewMode = SELECT_MODE;
		m_pMsgListView->updateTitleText(m_msgDeleteList.size());
		m_pMsgListView->setCancelBtnClickHandler(
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onCancelBtnClick>(this));
		m_pMsgListView->setDeleteBtnClickHandler(
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onDeleteBtnClick>(this));

		m_pMsgListView->setDisabledDeleteBtn(true);

		m_pMsgListView->setMsgSelectHandler(
				RejectMsgListView::MsgSelectHandler::wrap<RejectMsgListController, &RejectMsgListController::onMsgItemSelected>(this));

		m_pMsgListView->setSelectAllHandler(
				RejectMsgListView::MsgSelectAllHandler::wrap<RejectMsgListController, &RejectMsgListController::onSelectAllItemChecked>(this));

		m_pMsgListView->setMsgClickHandler(nullptr);

	}

	void RejectMsgListController::updateView(int updateFlag)
	{
		if (m_needUpdate) {
			updateRejectMsgList();
		}
	}

} }
