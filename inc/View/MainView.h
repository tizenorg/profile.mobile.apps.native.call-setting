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

#include "gui/BaseView.h"
#include "gui/Genlist.h"
#include "gui/DoubleTextListItem.h"

namespace CallSettings { namespace View {

	/*
	 * @brief Represents Main view instance
	 */
	class MainView : public gui::BaseView {
	public:
		/*
		 * @brief Set handler for click reject messages list item
		 * @param[in]	handler		event handler
		 */
		void setRejectMsgHandler(NotiHandler handler) { m_rejectMsgClickHandler = handler;}

		/*
		 * @brief Set handler for click Answering/ending calls list item
		 * @param[in]	handler		event handler
		 */
		void setCallAceptHandler(NotiHandler handler) { m_callAceptClickHandler = handler;}

		/*
		 * @brief Set handler for click More list item
		 * @param[in]	handler		event handler
		 */
		void setMoreHandler(NotiHandler handler) { m_moreClickHandler = handler;}

	private:
		friend class BaseView;

		MainView(gui::NaviItem *naviItem);
		virtual ~MainView();

		bool initialize();

		/*
		 * @brief Callback for list click event
		 * @param[in]	item	clicked list item
		 */
		void onOptionSelected(gui::WidgetItem &item);

	private:
		gui::Genlist *m_pGenlist;
		gui::DoubleTextListItem *m_pRejectMsgOption;
		gui::SimpleListItem *m_pCallAceptOption;
		gui::SimpleListItem *m_pMoreOption;

		NotiHandler m_rejectMsgClickHandler;
		NotiHandler m_callAceptClickHandler;
		NotiHandler m_moreClickHandler;
	};
} }

#endif /* _VIEW_MAIN_VIEW_H_ */
