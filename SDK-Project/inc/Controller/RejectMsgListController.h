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

#include "gui/Base/ViewController.h"

#include "App/Application.h"
#include "View/RejectMsgListView.h"

#include "RejectMsgEditorController.h"

namespace CallSettings { namespace Controller {

	class RejectMsgListController : public gui::ViewController {
	public:
		virtual ~RejectMsgListController();

	protected:
		friend class ViewController;

		RejectMsgListController(Application &app, NotiHandler handler);
		virtual void onBackKeyPressed() override;
		virtual void onMenuKeyPressed() override;
		virtual void updateView(int updateFlag) override;
		bool initialize();

		void onRejectMsgCountChanged();
		void onRejectMsgTextChanged();
		void updateRejectMsgList();
		void updateTitleInfo();
		void enableSelectMode();
		void disableSelectMode();
		void deleteSelectedMsg();
		void onMsgItemChecked(int msgId, bool isChecked);
		void onMsgItemClicked(int msgId);
		void onSelectAllItemChecked(bool isChecked);
		void onDeleteBtnClick(gui::Widget &sender);
		void onCancelBtnClick(gui::Widget &sender);
		void onMenuOptionDeleteClick();
		void onMenuOptionCreateClick();
		void onEditorControllerDestroy();

	private:
		typedef enum {
			DISPLAY_MODE,
			SELECT_MODE,
		} ViewMode;

		Application &m_app;
		View::RejectMsgListView *m_pMsgListView;
		RejectMsgEditorController *m_pMsgEditorController;
		ViewMode m_viewMode;
		int m_msgTotalCount;
		bool m_needUpdate;
		bool m_isOptionMenuShown;
		bool m_isMsgListLocked;

		std::vector<int> m_msgDeleteList;
	};

} }
#endif /* _CONTROLLER_REJECT_MSG_LIST_CONTROLLER_H_ */
