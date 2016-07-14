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

#ifndef _CONTROLLER_MORE_VIEW_CONTROLLER_H_
#define _CONTROLLER_MORE_VIEW_CONTROLLER_H_

#include "gui/ViewController.h"

#include "App/Application.h"
#include "View/MoreView.h"

#include "Model/Telephony/TelRequestListener.h"
#include "Model/Telephony/TelephonyTypes.h"

#include "CallFwController.h"

namespace CallSettings { namespace Controller {

	/*
	 * @brief Represents Controller of More view
	 */
	class MoreViewController :
			public gui::ViewController,
			private Model::TelRequestListener<Model::CallWaitingReqData>,
			private Model::SimpleTelRequestListener {
	public:
		virtual ~MoreViewController();

	private:
		friend class ViewController;

		MoreViewController();
		MoreViewController(Application &app, NotiHandler handler);

		/*
		 * @brief initialization of MainViewController
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief Called when view needs update for some reasons
		 * @param[in] Update flags which describes update reason
		 */
		virtual void updateView(int updateFlag) override;

		/*
		 * @brief Called when telephony request get call waiting option state is finished
		 * @param[in]	result		Result code
		 * @param[in]	value		Call waiting option status data
		 */
		virtual void onRequestComplete(Model::TelResultCode result, const Model::CallWaitingReqData *value) override;

		/*
		 * @brief Called when telephony request for setup call waiting option state is completed
		 * @param[in]	result		Result code
		 *
		 */
		virtual void onRequestComplete(Model::TelResultCode result) override;

		/*
		 * @brief Called when My Caller id list item clicked
		 */
		void onCallerIdItemClick();

		/*
		 * @brief Called when Call Forwarding list item clicked
		 */
		void onForwardingItemClick();

		/*
		 * @brief Called when Call waiting checkbox checked
		 */
		void onWaitingOptionCheck();

		/*
		 * @brief Called when Caller Id status changed. Invoked by model
		 */
		void onCallerIdOptionChanged();

		/*
		 * @brief Called when Call waiting option statues changed. Invoked by model
		 */
		void onWaitingOptionChanged();

		/*
		 * @brief Called to update My Caller Id list option subtext
		 */
		void updateCallerIdOption();

		/*
		 * @brief Called to update Waiting list option checkbox state
		 */
		void updateWaitingOption();

		/*
		 * @brief Called to enable/disable waiting option
		 * @param[in]	value	Waiting option value, true if call waiting enabled, otherwise false
		 */
		void setupWaitingOption(bool state);

		/*
		 * @brief Called when New caller id status selected in Caller Id popup widget
		 * @param[in]	value	Caller Id new value
		 */
		void onCallerIdStatusSelected(View::MoreView::CallerIdStatus value);

		/*
		 * @brief Called to show Caller Id options popup widget
		 */
		void onCallerIdPopupHide();

		void onCallFwControllerDestroy();

	private:
		Application &m_app;
		View::MoreView *m_pMoreView;
		Model::CallWaitingReqData m_callWaitData;
		CallFwController *m_pCallFwController;
		bool m_needUpdateCallerId;
		bool m_needUpdateWaiting;
		bool m_waitRequestPending;
	};
} }

#endif /* _CONTROLLER_MORE_VIEW_CONTROLLER_H_ */
