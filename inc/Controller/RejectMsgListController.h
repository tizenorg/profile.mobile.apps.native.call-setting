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

#include "gui/Button.h"
#include "gui/CategoryItem.h"
#include "gui/OptionMenuPopup.h"

namespace CallSettings { namespace Controller {

	/*
	 * @brief Represents Reject Messages List View
	 */
	class RejectMsgListController : public gui::ViewController {
	public:
		virtual ~RejectMsgListController();

	private:
		class MsgItem;
		friend class ViewController;

		RejectMsgListController(Application &app, NotiHandler handler);

		/*
		 * @brief Called when view needs update for some reasons
		 * @param[in] Update flags which describes update reason
		 */
		virtual void updateView(int updateFlag) override;

		/*
		 * @brief controller initialization
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief Callback for change reject message count system settings
		 */
		void onRejectMsgCountChanged();
		/*
		 * @brief Called to delete selected reject messages
		 */
		void deleteSelectedMsg();

		/*
		 * @brief Update reject messages list view
		 */
		void updateMsgList();

		/*
		 * @brief Update count of messages in view title text
		 */
		void updateTitleMsgCount();

		/*
		 * @brief Add new list item for represent Reject message
		 * @paramp[in]	msgKey		Reject message key
		 */
		void addMessageItem(Model::StringKey msgKey);

		/*
		 * @brief Change view to select mode. In select mode title buttons are added
		 * and all list items has checkboxes for mark/unmark
		 */
		void changeViewToSelectMode();

		/*
		 * @brief Change view to display mode
		 */
		void changeViewToDisplayMode();

		/*
		 * @brief Create Delete title button
		 * @note needed for select mode only
		 */
		void createDeleteButton();

		/*
		 * @brief Create Cancel title button
		 * @note needed for select mode only
		 */
		void createCancelButton();

		/*
		 * @brief Create Select All list item. This item has different text style than othe message items
		 * @note needed for select mode only
		 */
		void createSelectAllItem();

		/*
		 * @brief Delete title button click callback
		 */
		void onDeleteButtonClick(gui::Widget &item);

		/*
		 * @brief Cancel title button click callback
		 */
		void onCancelButtonClick(gui::Widget &item);

		/*
		 * @brief Select all list item click callback
		 */
		void onSelectAllChecked(gui::WidgetItem &item);

		/*
		 * @brief Enable/Disable Delete title button in selection mode
		 * @note In case of no one message is marked for delete delete button must be disabled, otherwise enabled
		 */
		void updateDeleteBtnState();

		/*
		 * @brief Update select all list item check state
		 */
		void updateSelectAllItemState();

		/*
		 * @brief Mark/Unmark all list items in selection mode
		 * @param[in]	isItemChecked	If true all items will be marked, otherwise unmarked
		 */
		void setAllItemsCheckState(bool isItemChecked);

		/*
		 * @brief Callback for Message list item click
		 * @param[in]	item	Message list item instance
		 */
		void onMsgItemClicked(MsgItem *item);

		/*
		 * @brief Callback for Message list check event
		 * @param[in]	item	Message list item instance
		 * @note This callback invoked in select mode only
		 */
		void onMsgItemChecked(MsgItem *item);

		/*
		 * @brief Create More menu options context popup
		 */
		void showMoreMenuPopup();

		/*
		 * @brief Callback for delete option click in more menu
		 */
		void onMenuOptionDeleteClick();

		/*
		 * @brief Callback for create option click in more menu
		 */
		void onMenuOptionCreateClick();

		/*
		 * @brief Callback for more menu destroy popup
		 */
		void onMenuPopupDestroy();

		/*
		 * @brief Callback for hardware back key press
		 * @note Click on back key in selection mode cause change view to disaply mode
		 * CLick on back key in display mode cause pop current view.
		 */
		void onBackKeyPressed() override;

		/*
		 * @brief Callback for menu hardware key
		 */
		void onMenuKeyPressed() override;

		/*
		 * @brief Child Editor controller destroy callback
		 */
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
		gui::CheckboxListItem *m_pSelectAllItem;
		gui::OptionMenuPopup *m_pMoreMenuPopup;

		ViewMode m_viewMode;
		RejectMsgEditorController *m_pMsgEditorController;

		std::vector<std::unique_ptr<MsgItem>> m_msgItemsArray;
		std::vector<MsgItem *> m_selectedMsgItems;
	};

} }
#endif /* _CONTROLLER_REJECT_MSG_LIST_CONTROLLER_H_ */
