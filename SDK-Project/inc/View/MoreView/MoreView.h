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
#include "gui/Widgets/SimpleListItem.h"
#include "gui/Widgets/DoubleTextListItem.h"
#include "gui/Widgets/Popup.h"

namespace CallSettings { namespace View {

	class MoreView : public gui::BaseView {
	public:
		typedef enum {
			CALLER_ID_STATUS_DEFAULT,
			CALLER_ID_STATUS_SHOW,
			CALLER_ID_STATUS_HIDE,
		} CallerIdStatus;

		typedef util::Delegate <void(CallerIdStatus)> CallerIdStatusChangeHandler;

		void setCallerIdStatus(CallerIdStatus value);
		void showCallerIdPopup(CallerIdStatus selectedValue, CallerIdStatusChangeHandler statusHandler, NotiHandler popupHideHandler);
		void hideCallerIdPopup();

		void setCallerIdClickHandler(NotiHandler handler) { m_callerIdClickHandeler = handler;}
		void setCallForwardClickHandler(NotiHandler handler) { m_callFwdClickHandler = handler;}
		void setCallWaitingCheckHandler(NotiHandler handler) { m_callWaitingCheckHandler = handler;}

		void setCallerIdValue(int callerId);
		void setWaitingOptionChecked(bool checked) {m_pCallWaitingOption->setCheckState(checked);}
		bool isWaitingOptionChecked() { return m_pCallWaitingOption->getCheckState();}
		void setWaitingOptionPending(bool isPending);

	private:
		friend class BaseView;

		MoreView(gui::NaviItem *naviItem);
		virtual ~MoreView();
		virtual bool createViewContent() override;
		void onCallerIdPopupItemSelect(int value);
		void onCallerIdPopupDestroy();
		void onOptionSelected(gui::WidgetItem *item);

	private:
		gui::Genlist *m_pGenlist;
		gui::DoubleTextListItem *m_pCallerIdOption;
		gui::SimpleListItem *m_pCallFwdOption;
		gui::DoubleTextListItem *m_pCallWaitingOption;
		gui::Popup *m_pCallerIdPopup;
		CallerIdStatusChangeHandler m_idStatusHandler;
		NotiHandler m_callerIdPopupHideCb;
		NotiHandler m_callerIdClickHandeler;
		NotiHandler m_callFwdClickHandler;
		NotiHandler m_callWaitingCheckHandler;
	};

} }
#endif /* _VIEW_MORE_VIEW_H_ */
