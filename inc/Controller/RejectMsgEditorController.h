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

#ifndef _CONTROLLER_REJECT_MSG_EDIT_CONTROLLER_H_
#define _CONTROLLER_REJECT_MSG_EDIT_CONTROLLER_H_

#include "gui/ViewController.h"

#include "App/Application.h"
#include "View/RejectMsgEditView.h"

#include "Utils.h"

namespace CallSettings { namespace Controller {

	class RejectMsgEditorController : public gui::ViewController {
	public:
		virtual ~RejectMsgEditorController();

	protected:
		friend class ViewController;

		RejectMsgEditorController(Application &app, NotiHandler handler);
		bool initialize();
		bool initialize(Model::StringKey editableMsgId);
		bool initializeView();

		void onDoneBtnClick(gui::Widget &sender);
		void onCancelBtnClick(gui::Widget &sender);
		void onInputEvent();
		bool onInputFilterEvent(std::string &msgText);
		bool updateMsgSettings();
		bool isMsgDuplicated(const std::string &msg);
		static bool calculateSMSTextLength(const std::string &smsText, int &smsCharCount, int &smsMaxCount);

	private:
		typedef enum {
			VIEW_MODE_CREATE,
			VIEW_MODE_EDIT
		} ViewMode;

		Application &m_app;
		View::RejectMsgEditView *m_pMsgEditView;
		ViewMode m_viewMode;
		Model::StringKey m_editableMsgId;
		int m_smsInputedCharCount;
		int m_smsMaxCharCount;
	};

} }

#endif /* _CONTROLLER_REJECT_MSG_EDIT_CONTROLLER_H_ */
