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

#ifndef _VIEW_REJECT_MSG_LIST_VIEW_H_
#define _VIEW_REJECT_MSG_LIST_VIEW_H_

#include "gui/Base/BaseView.h"
#include "gui/Widgets/Genlist.h"
#include "gui/Widgets/Layout.h"
#include "gui/Widgets/Button.h"
#include "gui/Widgets/CategoryItem.h"
#include "gui/Widgets/DoubleTextListItem.h"
#include "gui/Widgets/OptionMenuPopup.h"

namespace CallSettings { namespace View {

	class RejectMsgListView : public gui::BaseView {
	public:
		typedef util::Delegate<void(int)> MsgClickHandler;
		typedef util::Delegate<void(int, bool)> MsgCheckHandler;
		typedef util::Delegate<void(bool)> MsgSelectAllHandler;

		void invalidateMsgList();
		bool addListItem(int index, util::TString msgText);
		void removeListItem(int index);
		bool enableSelectMode();
		bool disableSelectMode();
		void updateTitleMsgCount(int msgCount = 0);
		void setMsgClickHandler(MsgClickHandler clickHandler) {m_msgClickHandler = clickHandler;}
		void setMsgCheckHandler(MsgCheckHandler selectHandler) {m_msgCheckHandler = selectHandler;}
		void setSelectAllHandler(MsgSelectAllHandler selectHandler) {m_selectAllHandler = selectHandler;}
		void setSelectAllChecked(bool chackedState);
		bool isSelectAllChecked();
		void setAllMsgSelected(bool chackedState);
		void setCancelBtnClickHandler(NotiHandler clickHandler);
		void setDeleteBtnClickHandler(NotiHandler clickHandler);
		void setDisabledDeleteBtn(bool isDisabled);
		bool showMoreMenuPopup(NotiHandler createOptionHandler, NotiHandler deleteOptionHandler);
		void hideMoreMenuPopup();

	protected:
		RejectMsgListView(gui::NaviItem *naviItem);
		virtual ~RejectMsgListView() {}

		bool initialize();
		void changeViewToMsgList();
		void changeViewToEmpty();
		void onMsgItemClicked(gui::WidgetItem *item);
		void onMsgItemChecked(gui::WidgetItem *item);
		int getMsgItemIndex(gui::DoubleTextListItem *item);
		void onCancelTitleBtnClick();
		void onDeleteTitleBtnClick();
		void onMenuPopupDestroy();
		void onSelectAllChecked(gui::WidgetItem *item);

	private:
		friend class BaseView;

		gui::Genlist *m_pGenlist;
		gui::Layout *m_pNoContentLayout;
		gui::Button *m_pCancelBtn;
		gui::Button *m_pDeleteBtn;
		gui::CategoryListItem *m_pSelectAllItem;
		gui::OptionMenuPopup *m_pMoreMenuPopup;
		bool m_isSelectModeEnabled;
		MsgClickHandler m_msgClickHandler;
		MsgCheckHandler m_msgCheckHandler;
		MsgSelectAllHandler m_selectAllHandler;

		typedef std::pair<int, gui::DoubleTextListItem *> MsgItemRecord;
		std::vector<MsgItemRecord> m_msgItemsArray;
	};

} }
#endif /* _VIEW_REJECT_MSG_LIST_VIEW_H_ */
