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

#ifndef _VIEW_MAIN_VIEW_H_
#define _VIEW_MAIN_VIEW_H_

#include "gui/Base/BaseView.h"
#include "gui/Widgets/Genlist.h"
#include "gui/Widgets/OptionItem.h"

namespace CallSettings { namespace View {

	class MainView : public gui::BaseView {
	public:
		void setRejectMsgHandler(NotifyHandler handler);
		void setCallAceptHandler(NotifyHandler handler);
		void setMoreHandler(NotifyHandler handler);

	private:
		friend class BaseView;

		MainView(gui::NaviItem *naviItem);
		virtual ~MainView();
		virtual bool createViewContent() override;

	private:
		gui::Genlist *m_pGenlist;
		gui::OptionItem *m_pRejectMsgOption;
		gui::OptionItem *m_pCallAceptOption;
		gui::OptionItem *m_pMoreOption;
	};
} }

#endif /* _VIEW_MAIN_VIEW_H_ */
