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

#ifndef MAIN_VIEW_H_
#define MAIN_VIEW_H_

#include "View/ViewManager/BaseView.h"
#include "View/Widgets/OptionItem.h"
#include "View/Widgets/Genlist.h"

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
		Widgets::OptionItem *m_pRejectMsgOption;
		Widgets::OptionItem *m_pCallAceptOption;
		Widgets::OptionItem *m_pMoreOption;
	};
}

#endif /* MAIN_VIEW_H_ */
