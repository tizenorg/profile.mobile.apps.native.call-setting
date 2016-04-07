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

#include "gui/Base/ViewController.h"
#include "Model/Telephony/TelRequestListener.h"
#include "Model/Telephony/TelephonyTypes.h"

#include "App/Application.h"
#include "View/MoreView/MoreView.h"

namespace CallSettings { namespace Controller {

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
		bool initialize();
		virtual void onBackKeyPressed() override;
		virtual void updateView(int updateFlag) override;
		virtual void onRequestComplete(Model::TelResultCode result, const Model::CallWaitingReqData *value) override;
		virtual void onRequestComplete(Model::TelResultCode result) override;

		void onCallerIdItemClick();
		void onForwardingItemClick();
		void onWaitingItemClick();
		void onWaitingOptionCheck();

		void onCallerIdOptionChanged();
		void onWaitingOptionChanged();

		void updateCallerIdOption();
		void updateWaitingOption();
		void setupWaitingOption(bool state);

	private:
		Application &m_app;
		View::MoreView *m_pMoreView;
		Model::CallWaitingReqData m_callWaitData;
		bool m_needUpdateCallerId;
		bool m_needUpdateWaiting;
		bool m_waitRequestPending;
	};
} }



#endif /* _CONTROLLER_MORE_VIEW_CONTROLLER_H_ */
