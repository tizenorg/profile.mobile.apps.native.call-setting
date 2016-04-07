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

#ifndef _VIEW_MORE_VIEW_H_
#define _VIEW_MORE_VIEW_H_

#include "gui/Base/BaseView.h"
#include "gui/Widgets/Genlist.h"
#include "gui/Widgets/CheckOptionItem.h"

namespace CallSettings { namespace View {

	class MoreView : public gui::BaseView {
	public:
		void setCallerIdClickHandler(NotiHandler handler) { m_pCallerIdOption->setSelectHandler(handler);}
		void setCallForwardClickHandler(NotiHandler handler) { m_pCallFwdOption->setSelectHandler(handler);}
		void setCallWaitingClickHandler(NotiHandler handler) { m_pCallWaitingOption->setSelectHandler(handler);}
		void setCallWaitingCheckHandler(NotiHandler handler) { m_pCallWaitingOption->setCheckHandler(handler);}

		void setCallerIdValue(int callerId);
		void checkWaitingOption(bool checked) {m_pCallWaitingOption->setCheckState(checked);}
		bool getWaitingOptionState() { return m_pCallWaitingOption->getCheckState();}
		void setWaitingOptionPending(bool isPending) {m_pCallWaitingOption->setCheckPending(isPending);}

	private:
		friend class BaseView;

		MoreView(gui::NaviItem *naviItem);
		virtual ~MoreView();
		virtual bool createViewContent() override;

	private:
		gui::Genlist *m_pGenlist;
		gui::OptionItem *m_pCallerIdOption;
		gui::OptionItem *m_pCallFwdOption;
		gui::CheckOptionItem *m_pCallWaitingOption;
	};

} }
#endif /* _VIEW_MORE_VIEW_H_ */
