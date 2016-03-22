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

#include "View/MainView/RejectMsgOption.h"
#include "View/MainView/CallAceptOption.h"
#include "View/MainView/MoreOption.h"

#include "View/ViewManager/BaseView.h"
#include "View/Widgets/Genlist.h"

#ifndef MAIN_VIEW_H_
#define MAIN_VIEW_H_

namespace MainView {

	class MainView : public View::BaseView {
	public:
		void setRejectMsgHandler(NotifyHandler handler);
		void setCallAceptHandler(NotifyHandler handler);
		void setMoreHandler(NotifyHandler handler);

	private:
		friend class BaseView;

		MainView(Widgets::NaviItem *naviItem);
		virtual ~MainView();
		virtual bool createViewContent() override;

	private:
		Widgets::Genlist *m_pGenlist;
		RejectMsgOption *m_pRejectMsgOption;
		CallAceptOption *m_pCallAceptOption;
		MoreOption *m_pMoreOption;
	};
}

#endif /* MAIN_VIEW_H_ */
