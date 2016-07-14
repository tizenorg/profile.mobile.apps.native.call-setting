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

#include "gui/ViewController.h"
#include "gui/Popup.h"

#include "App/Application.h"
#include "View/GenlistView.h"

#include "Model/Telephony/TelephonyTypes.h"

namespace CallSettings { namespace Controller {

	/*
	 * @brief Represent Call forwarding view controller
	 */
	class CallFwController : public gui::ViewController {
	public:
		virtual ~CallFwController();

	private:
		class EditPopup;
		class Item;

	private:
		friend class ViewController;
		CallFwController(Application &app, NotiHandler handler);

		/*
		 * @brief Initialization of CallFwController
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief Show pending popup widget
		 */
		void showPendingPopup();
		/*
		 * @brief Update forward options list
		 */
		void updateFwdOptions();

		// Item events //
		/*
		 * @brief Called when Forward option item is ready for change forward condition state
		 * @param[in]	item	Option item instance
		 */
		void onFwdOptionReady(Item *item);

		/*
		 * @brief Called when Forward option item request to change forward condition state
		 * @param[in]	item	Option item instance
		 */
		void onFwdOptionChangeBegin(Item *item);

		/*
		 * @brief Called when request to change forward condition state completed
		 * @param[in]	item	Option item instance
		 */
		void onFwdOptionChangeEnd(Item *item);

		// Pending popup events

		/*
		 * @brief Callback for destroy pending popup widget
		 */
		void onPendingPopupDel();

		/*
		 * @brief Callback for pending popup block area clicked
		 * @note Needed to deny popup hide by block area click as default popup behavior
		 */
		bool onPendingPopupBlock();
		/*
		 * @brief Callback for pending popup back key clicked
		 * @note Back click on pending popup initiates view destroy
		 */
		bool onPendingPopupBack();

		// gui::ViewController //
		virtual void updateView(int updateFlag) override;

		// model::ITeleponyManager event//
		void onFwdStatusChanged();

	private:
		enum {
			UF_FWD_STATUS_CHANGE = (1 << UF_CUSTOM_SHIFT)
		};

		Application &m_app;
		View::GenlistView *m_pView;
		gui::Popup *m_pPendingPopup;
		std::vector <Item *> m_fwdItemsArray;
		Item *m_pUncondFwdItem;
		bool m_isItemChanging;
		int m_updatingItemsCount;
		int m_statusChangeCount;
	};

}}

#endif /* _CONTROLLER_CALL_FW_CONTROLLER_H_ */
