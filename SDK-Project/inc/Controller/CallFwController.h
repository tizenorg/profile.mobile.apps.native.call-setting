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

#ifndef _CONTROLLER_CALL_FW_CONTROLLER_H_
#define _CONTROLLER_CALL_FW_CONTROLLER_H_

#include "gui/Base/ViewController.h"
#include "gui/Widgets/Popup.h"

#include "App/Application.h"
#include "View/GenlistView.h"

namespace CallSettings { namespace Controller {

	class CallFwController : public gui::ViewController {
	public:
		virtual ~CallFwController();

	private:
		class EditPopup;

	private:
		friend class ViewController;
		CallFwController(Application &app, NotiHandler handler);

		bool initialize();

		void showPendingPopup();

		// Pending popup events

		void onPendingPopupDel();
		bool onPendingPopupBlock();
		bool onPendingPopupBack();

		// View item events

		void onItemClick(gui::WidgetItem &item);

		// Edit popup events

		void onEditPopupDel();

		// gui::ViewController //

		virtual void updateView(int updateFlag) override;

	private:
		Application &m_app;
		View::GenlistView *m_pView;
		EditPopup *m_pEditPopup;
		gui::Popup *m_pPendingPopup;
	};

}}

#endif /* _CONTROLLER_CALL_FW_CONTROLLER_H_ */
