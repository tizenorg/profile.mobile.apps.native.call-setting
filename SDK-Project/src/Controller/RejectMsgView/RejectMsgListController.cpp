
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

#include "Controller/RejectMsgView/RejectMsgListController.h"
#include "View/RejectMsgView/RejectMsgListView.h"

namespace CallSettings { namespace Controller {

	using namespace Model;
	using namespace View;

	RejectMsgListController::RejectMsgListController (Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pMsgListView(nullptr),
		m_pMsgEditorController(nullptr),
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

		for(int i = REJECT_MSG_1; i <= REJECT_MSG_6; i++) {
			RejectMsgId msgId = static_cast<RejectMsgId>(i);
			m_app.getSettingsManager().removePropertyHandler(convertToStringKey(msgId),
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgTextChanged>(this));
		}

		m_pMsgListView->setMsgCheckHandler(nullptr);
		m_pMsgListView->setMsgClickHandler(nullptr);
		m_pMsgListView->setCancelBtnClickHandler(nullptr);
		m_pMsgListView->setDeleteBtnClickHandler(nullptr);
		m_pMsgListView->setSelectAllHandler(nullptr);
		delete m_pMsgEditorController;
	}

	bool RejectMsgListController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");


		auto res = m_app.getSettingsManager().addPropertyHandler(INT_KEY_REJECT_MSG_COUNT,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgCountChanged>(this));
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup message count change listener!");

		for(int i = REJECT_MSG_1; i <= REJECT_MSG_6; i++) {
			RejectMsgId msgId = static_cast<RejectMsgId>(i);
			res = m_app.getSettingsManager().addPropertyHandler(convertToStringKey(msgId),
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onRejectMsgTextChanged>(this));
			RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to setup mgs content listener");
		}

		m_pMsgListView =  gui::ViewManager::pushView<View::RejectMsgListView>(m_app.getViewManager(), true);
		RETVM_IF(!m_pMsgListView, false, "Failed to create view instance!");

		m_pMsgListView->setMsgClickHandler(
				RejectMsgListView::MsgClickHandler::wrap<RejectMsgListController, &RejectMsgListController::onMsgItemClicked>(this));

		m_pMsgListView->setMsgCheckHandler(
					RejectMsgListView::MsgCheckHandler::wrap<RejectMsgListController, &RejectMsgListController::onMsgItemChecked>(this));

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

	void RejectMsgListController::onMsgItemChecked(int msgId, bool isChecked)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in select mode, skip select event!");

		if (isChecked) {
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

		updateTitleInfo();
	}

	void RejectMsgListController::onSelectAllItemChecked(bool isChecked)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != SELECT_MODE, "View is in display mode, skip click event!");

		m_msgDeleteList.clear();

		if (isChecked) {
			for(int i = REJECT_MSG_1; i < REJECT_MSG_1 + m_msgTotalCount; i++) {
				m_msgDeleteList.push_back(i);
			}
			m_pMsgListView->setAllMsgSelected(true);
			m_pMsgListView->setDisabledDeleteBtn(false);
		} else {
			m_pMsgListView->setAllMsgSelected(false);
			m_pMsgListView->setDisabledDeleteBtn(true);
		}

		updateTitleInfo();
	}

	void RejectMsgListController::onMsgItemClicked(int msgId)
	{
		RETM_IF(!m_isActivated, "View is not active, skip click event!");
		RETM_IF(m_viewMode != DISPLAY_MODE, "View is in edit mode, skip click event!");

		m_pMsgEditorController =  ViewController::create<Controller::RejectMsgEditorController>(m_app,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onEditorControllerDestroy>(this),
				static_cast<RejectMsgId>(msgId));

	}

	void RejectMsgListController::onEditorControllerDestroy()
	{
		delete m_pMsgEditorController;
		m_pMsgEditorController = nullptr;
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
			int msgId = REJECT_MSG_1;
			for(int i = REJECT_MSG_1; i < REJECT_MSG_1 + m_msgTotalCount; i++) {
				if (std::find(m_msgDeleteList.begin(), m_msgDeleteList.end(), i) == m_msgDeleteList.end()) {
					std::string msgText;

					Model::StringKey oldMsgKey = convertToStringKey(static_cast<RejectMsgId>(i));
					Model::StringKey newMsgKey = convertToStringKey(static_cast<RejectMsgId>(msgId));
					m_app.getSettingsManager().getProperty(oldMsgKey, msgText);
					m_app.getSettingsManager().setProperty(newMsgKey, msgText);
					msgId++;
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
			for (int i = REJECT_MSG_1; i < REJECT_MSG_1 + m_msgTotalCount; i++) {
				std::string msgText;
				RejectMsgId msgId = static_cast<RejectMsgId>(i);
				res = m_app.getSettingsManager().getProperty(convertToStringKey(msgId), msgText);

				if(res == SETTINGS_RES_SUCCESS && !msgText.empty()) {
					m_pMsgListView->addListItem(msgId, msgText);
				} else {
					ERR("Failed to get message %d", msgId);
				}
			}
		}
		updateTitleInfo();
	}

	void RejectMsgListController::updateTitleInfo()
	{
		if (m_viewMode == SELECT_MODE) {
			m_pMsgListView->updateTitleMsgCount(m_msgDeleteList.size());
		} else {
			m_pMsgListView->updateTitleMsgCount(m_msgTotalCount);
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

		RETM_IF(!m_isActivated, "View is not activse, skip click event!");
		if (m_viewMode == SELECT_MODE) {
			return;
		}

		if(m_msgTotalCount == 0) {
			m_pMsgListView->showMoreMenuPopup(
					NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onMenuOptionCreateClick>(this),
					nullptr
			);
		} else if (m_msgTotalCount < REJECT_MSG_MAX_COUNT) {
			m_pMsgListView->showMoreMenuPopup(
					NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onMenuOptionCreateClick>(this),
					NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onMenuOptionDeleteClick>(this)
			);
		} else {
			m_pMsgListView->showMoreMenuPopup(
					nullptr,
					NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onMenuOptionDeleteClick>(this)
			);
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

		m_pMsgEditorController =  ViewController::create<Controller::RejectMsgEditorController>(m_app,
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onEditorControllerDestroy>(this));
	}


	void RejectMsgListController::disableSelectMode()
	{
		if (!m_pMsgListView->disableSelectMode()) {
			ERR("Failed to setup edit mode");
			makeDestroyReqeuest();
		}

		m_viewMode = DISPLAY_MODE;
		m_msgDeleteList.clear();
		updateTitleInfo();
	}

	void RejectMsgListController::enableSelectMode()
	{
		if (!m_pMsgListView->enableSelectMode()) {
			ERR("Failed to setup edit mode");
			makeDestroyReqeuest();
		}

		m_viewMode = SELECT_MODE;
		m_pMsgListView->setCancelBtnClickHandler(
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onCancelBtnClick>(this));
		m_pMsgListView->setDeleteBtnClickHandler(
				NotiHandler::wrap<RejectMsgListController, &RejectMsgListController::onDeleteBtnClick>(this));

		m_pMsgListView->setDisabledDeleteBtn(true);

		m_pMsgListView->setSelectAllHandler(
				RejectMsgListView::MsgSelectAllHandler::wrap<RejectMsgListController, &RejectMsgListController::onSelectAllItemChecked>(this));

		updateTitleInfo();
	}

	void RejectMsgListController::updateView(int updateFlag)
	{
		if (m_needUpdate) {
			updateRejectMsgList();
		}

		if (updateFlag & UF_LANGUAGE) {
			updateTitleInfo();
		}
	}

} }
