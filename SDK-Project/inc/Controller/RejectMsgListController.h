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

#ifndef _CONTROLLER_REJECT_MSG_LIST_CONTROLLER_H_
#define _CONTROLLER_REJECT_MSG_LIST_CONTROLLER_H_

#include "App/Application.h"
#include "View/GenlistView.h"

#include "RejectMsgEditorController.h"

#include "gui/Widgets/Button.h"
#include "gui/Widgets/CategoryItem.h"
#include "gui/Widgets/OptionMenuPopup.h"

namespace CallSettings { namespace Controller {

	class RejectMsgListController : public gui::ViewController {
	public:
		virtual ~RejectMsgListController();

	private:
		class MsgItem;
		friend class ViewController;

		RejectMsgListController(Application &app, NotiHandler handler);
		virtual void updateView(int updateFlag) override;
		bool initialize();

		void onRejectMsgCountChanged();
		void deleteSelectedMsg();

		void updateMsgList();
		void updateTitleMsgCount();

		void addMessageItem(Model::StringKey msgKey);

		void changeViewToSelectMode();
		void changeViewToDisplayMode();

		gui::Button *createDeleteButton();
		gui::Button *createCancelButton();
		gui::CategoryListItem *createSelectAllItem();

		void onDeleteButtonClick(gui::Widget &item);
		void onCancelButtonClick(gui::Widget &item);
		void onSelectAllChecked(gui::WidgetItem &item);

		void updateDeleteBtnState();
		void updateSelectAllItemState();
		void setAllItemsCheckState(bool isItemChecked);

		void onMsgItemClicked(MsgItem *item);
		void onMsgItemChecked(MsgItem *item);

		void showMoreMenuPopup();
		void onMenuOptionDeleteClick();
		void onMenuOptionCreateClick();
		void onMenuPopupDestroy();

		void onBackKeyPressed() override;
		void onMenuKeyPressed() override;
		void onEditorControllerDestroy();


	private:
		typedef enum {
			DISPLAY_MODE,
			SELECT_MODE,
		} ViewMode;

		Application &m_app;
		View::GenlistView *m_pMsgListView;
		gui::Button *m_pCancelBtn;
		gui::Button *m_pDeleteBtn;
		gui::CategoryListItem *m_pSelectAllItem;
		gui::OptionMenuPopup *m_pMoreMenuPopup;

		ViewMode m_viewMode;
		RejectMsgEditorController *m_pMsgEditorController;

		std::vector<std::unique_ptr<MsgItem>> m_msgItemsArray;
		std::vector<MsgItem *> m_selectedMsgItems;
	};

} }
#endif /* _CONTROLLER_REJECT_MSG_LIST_CONTROLLER_H_ */
