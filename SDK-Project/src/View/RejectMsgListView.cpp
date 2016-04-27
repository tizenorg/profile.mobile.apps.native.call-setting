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

#include "View/RejectMsgListView.h"

#define MAX_HEADER_TEXT_LEN 256

namespace CallSettings { namespace View {

	using namespace gui;

	RejectMsgListView::RejectMsgListView(NaviItem *naviItem) :
		BaseView(naviItem),
		m_pGenlist(nullptr),
		m_pNoContentLayout(nullptr),
		m_pCancelBtn(nullptr),
		m_pDeleteBtn(nullptr),
		m_pSelectAllItem(nullptr),
		m_pMoreMenuPopup(nullptr),
		m_isSelectModeEnabled(false)
	{
	}

	bool RejectMsgListView::initialize()
	{
		RETVM_IF(!BaseView::initialize(), false, "Failed to initialize BaseView!");

		m_pNoContentLayout = Widget::create<Layout>(*m_pViewLayout, "layout", "nocontents", "default");
		RETVM_IF(!m_pNoContentLayout, false, "Failed to create no content layout, unknown error");

		m_pNoContentLayout->setPartText("elm.text", "IDS_CST_NPBODY_NO_MESSAGES");
		m_pNoContentLayout->setPartText("elm.help.text",
				"IDS_CST_BODY_AFTER_YOU_CREATE_CALL_REJECT_MESSAGES_THEY_WILL_BE_SHOWN_HERE");

		m_pNoContentLayout->emitSignal("align.center", "elm");
		m_pNoContentLayout->emitSignal("text,disabled", "");
		m_pNoContentLayout->hide();

		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");
		m_pGenlist->setHomogeneousMode(false);

		m_pNaviItem->setTitleText("IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB");

		return setViewContent(*m_pGenlist);
	}

	bool RejectMsgListView::addListItem(int index, util::TString msgText)
	{
		DoubleTextListItem *msgItem = m_pGenlist->appendItem<DoubleTextListItem>("", std::move(msgText), true);
		RETVM_IF(!msgItem, false, "Failed to append item");

		if (m_isSelectModeEnabled) {
			msgItem->setCheckMode(CheckboxListItem::AUTO_CHECK);
		} else {
			msgItem->setCheckMode(CheckboxListItem::HIDDEN);
		}

		if (m_msgItemsArray.empty()) {
			changeViewToMsgList();
		}

		msgItem->setSelectHandler(ItemNotiHandler::wrap<RejectMsgListView, &RejectMsgListView::onMsgItemClicked>(this));
		msgItem->setCheckHandler(ItemNotiHandler::wrap<RejectMsgListView, &RejectMsgListView::onMsgItemChecked>(this));
		m_msgItemsArray.push_back(std::make_pair(index, msgItem));
		return true;
	}

	void RejectMsgListView::removeListItem(int index)
	{
		auto it = std::find_if(m_msgItemsArray.begin(), m_msgItemsArray.end(),
				[&index](MsgItemRecord &itemPair) {
					return itemPair.first == index;
				}
		);

		if (it != m_msgItemsArray.end()) {
			m_msgItemsArray.erase(it);
		}
		if (m_msgItemsArray.empty()) {
			changeViewToEmpty();
		}
	}

	void RejectMsgListView::onMsgItemChecked(WidgetItem *item)
	{
		RETM_IF(!item, "Invalid args!");
		auto *msgItem = static_cast<DoubleTextListItem *>(item);

		if (m_msgCheckHandler.assigned()) {
			int msgIndex = getMsgItemIndex(msgItem);
			if (msgIndex > 0) {
				m_msgCheckHandler(msgIndex, msgItem->getCheckState());
			}
		}
	}

	void RejectMsgListView::onMsgItemClicked(WidgetItem *item)
	{
		RETM_IF(!item, "Invalid args!");
		auto *msgItem = static_cast<DoubleTextListItem *>(item);
		if (m_msgClickHandler.assigned()) {
			int msgIndex = getMsgItemIndex(msgItem);
			if (msgIndex > 0) {
				m_msgClickHandler(msgIndex);
			}
		}
	}

	int RejectMsgListView::getMsgItemIndex(DoubleTextListItem *item)
	{
		auto it = std::find_if(m_msgItemsArray.begin(), m_msgItemsArray.end(),
				[&item](MsgItemRecord &itemPair) {
					return itemPair.second == item;
				}
		);

		RETVM_IF(it == m_msgItemsArray.end(), -1, "Item does not exist any more, callback will be skipped.");

		return it->first;
	}

	void RejectMsgListView::onSelectAllChecked(WidgetItem *item)
	{
		bool selectAllCheckState = m_pSelectAllItem->getCheckState();
		if (m_selectAllHandler.assigned()) {
			if (selectAllCheckState) {
				m_selectAllHandler(true);
			} else {
				m_selectAllHandler(false);
			}
		}
	}

	void RejectMsgListView::setSelectAllChecked(bool chackedState)
	{
		if (m_pSelectAllItem) {
			m_pSelectAllItem->setCheckState(chackedState);
		}
	}

	bool RejectMsgListView::isSelectAllChecked()
	{
		if (m_pSelectAllItem) {
			return m_pSelectAllItem->getCheckState();
		}

		return false;
	}

	void RejectMsgListView::setAllMsgSelected(bool chackedState)
	{
		for (auto it : m_msgItemsArray) {
			it.second->setCheckState(chackedState);
		}
	}

	void RejectMsgListView::changeViewToMsgList()
	{
		m_pViewLayout->unsetContent("elm.swallow.content");
		m_pNoContentLayout->hide();
		m_pGenlist->show();
		m_pViewLayout->setContent("elm.swallow.content", *m_pGenlist);
	}

	void RejectMsgListView::changeViewToEmpty()
	{
		m_pNaviItem->setTitleText("IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB");
		m_pViewLayout->unsetContent("elm.swallow.content");
		m_pGenlist->hide();
		m_pNoContentLayout->show();
		m_pViewLayout->setContent("elm.swallow.content", *m_pNoContentLayout);
	}

	bool RejectMsgListView::enableSelectMode()
	{
		RETVM_IF(m_isSelectModeEnabled, false, "Select mode is already enabled");

		m_isSelectModeEnabled = true;
		m_pCancelBtn = Widget::create<Button>(*m_pViewLayout, "naviframe/title_left", "IDS_MSG_ACBUTTON_CANCEL_ABB");
		RETVM_IF(!m_pCancelBtn, false, "Internal error");
		m_pNaviItem->setTitleLeftBtn(*m_pCancelBtn);

		m_pDeleteBtn = Widget::create<Button>(*m_pViewLayout, "naviframe/title_right", "IDS_MSG_ACBUTTON_DELETE_ABB");
		RETVM_IF(!m_pDeleteBtn, false, "Internal error");
		m_pNaviItem->setTitleRightBtn(*m_pDeleteBtn);

		m_pSelectAllItem = m_pGenlist->prependItem<CategoryListItem>("IDS_COM_BODY_SELECT_ALL");
		RETVM_IF(!m_pSelectAllItem, false, "Failed to enable Select Mode");

		m_pSelectAllItem->setCheckMode(CheckboxListItem::AUTO_CHECK);
		m_pSelectAllItem->setCheckHandler(ItemNotiHandler::wrap<RejectMsgListView, &RejectMsgListView::onSelectAllChecked>(this));

		for (MsgItemRecord itemPair : m_msgItemsArray) {
			auto msgItem = itemPair.second;
			msgItem->setCheckMode(CheckboxListItem::AUTO_CHECK);
			msgItem->setCheckState(false);
		}

		return true;
	}

	void RejectMsgListView::setCancelBtnClickHandler(NotiHandler clickHandler)
	{
		RETM_IF(!m_pCancelBtn, "Invalid state!");

		m_pCancelBtn->setClickHandler(clickHandler);
	}

	void RejectMsgListView::setDeleteBtnClickHandler(NotiHandler clickHandler)
	{
		RETM_IF(!m_pDeleteBtn, "Invalid state!");

		m_pDeleteBtn->setClickHandler(clickHandler);
	}

	void RejectMsgListView::setDisabledDeleteBtn(bool isDisabled)
	{
		RETM_IF(!m_pDeleteBtn, "Invalid state!");

		m_pDeleteBtn->setDisabled(isDisabled);
	}

	bool RejectMsgListView::disableSelectMode()
	{
		RETVM_IF(!m_isSelectModeEnabled, false, "Select mode is already disabled");

		for (MsgItemRecord itemPair : m_msgItemsArray) {
			auto msgItem = itemPair.second;
			msgItem->setCheckMode(CheckboxListItem::HIDDEN);
		}

		m_isSelectModeEnabled = false;
		m_pNaviItem->unsetTitleLeftBtn();
		m_pNaviItem->unsetTitleRightBtn();

		WidgetItem::destroy(m_pSelectAllItem);
		Widget::destroy(m_pCancelBtn);
		Widget::destroy(m_pDeleteBtn);
		m_pSelectAllItem = nullptr;
		m_pCancelBtn = nullptr;
		m_pDeleteBtn = nullptr;

		return true;
	}

	void RejectMsgListView::updateTitleMsgCount(int msgCount)
	{
		char headerText[MAX_HEADER_TEXT_LEN] = { 0, };

		if (m_isSelectModeEnabled) {
			snprintf(headerText, sizeof(char)*MAX_HEADER_TEXT_LEN, _("IDS_ST_HEADER_PD_SELECTED"), msgCount);
			m_pNaviItem->setTitleText(util::TString(headerText, false));
		} else {
			if (msgCount > 0) {
				snprintf(headerText, sizeof(char)*MAX_HEADER_TEXT_LEN, "%s (%d/6)", _("IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB"), msgCount);
				m_pNaviItem->setTitleText(util::TString(headerText, false));
			} else {
				m_pNaviItem->setTitleText("IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB");
			}
		}
	}

	void RejectMsgListView::invalidateMsgList()
	{
		m_msgItemsArray.clear();
		m_pGenlist->clear();
		changeViewToEmpty();
	}

	bool RejectMsgListView::showMoreMenuPopup(NotiHandler createOptionHandler, NotiHandler deleteOptionHandler)
	{
		m_pMoreMenuPopup = Widget::create<OptionMenuPopup>(*m_pViewLayout);
		RETVM_IF(!m_pMoreMenuPopup, false, "Failed to create menu popup!");

		m_pMoreMenuPopup->setDestroyHandler(
				NotiHandler::wrap<RejectMsgListView, &RejectMsgListView::onMenuPopupDestroy>(this));

		if (createOptionHandler != nullptr) {
			m_pMoreMenuPopup->addItem(createOptionHandler, "IDS_CST_SK3_CREATE");
		}

		if (deleteOptionHandler != nullptr) {
			m_pMoreMenuPopup->addItem(deleteOptionHandler, "IDS_CST_HEADER_DELETE");
		}

		m_pMoreMenuPopup->show();
		return true;
	}

	void RejectMsgListView::hideMoreMenuPopup()
	{
		m_pMoreMenuPopup->hide();
	}

	void RejectMsgListView::onMenuPopupDestroy()
	{
		m_pMoreMenuPopup = nullptr;
	}

} }
