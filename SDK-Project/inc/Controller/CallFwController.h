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

#include "Model/Telephony/TelephonyTypes.h"

namespace CallSettings { namespace Controller {

	class CallFwController : public gui::ViewController {
	public:
		virtual ~CallFwController();

	private:
		class EditPopup;
		class Item;

	private:
		friend class ViewController;
		CallFwController(Application &app, NotiHandler handler);

		bool initialize();

		void showPendingPopup();
		void updateFwdOptions();

		// Item events //
		void onFwdOptionReady();
		void onFwdOptionChangeBegin();
		void onFwdOptionChangeEnd();

		// Pending popup events

		void onPendingPopupDel();
		bool onPendingPopupBlock();
		bool onPendingPopupBack();

		// gui::ViewController //
		virtual void updateView(int updateFlag) override;

		// model::ITeleponyManager event//
		void onFwdStatusChanged();

	private:
		Application &m_app;
		View::GenlistView *m_pView;
		gui::Popup *m_pPendingPopup;
		std::vector <Item *> m_fwdItemsArray;
		Item *m_pUncondFwdItem;
		bool m_needUpdate;
		int m_updatingItemsCount;
		int m_statusChangeCount;
	};

}}

#endif /* _CONTROLLER_CALL_FW_CONTROLLER_H_ */
