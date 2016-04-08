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

		void setCallerIdClickHandler(NotiHandler handler) { m_pCallerIdOption->setSelectHandler(handler);}
		void setCallerIdStatus(CallerIdStatus value);
		void showCallerIdPopup(CallerIdStatus selectedValue, CallerIdStatusChangeHandler statusHandler, NotiHandler popupHideHandler);
		void hideCallerIdPopup();

		void setCallForwardClickHandler(NotiHandler handler) { m_pCallFwdOption->setSelectHandler(handler);}
		void setCallWaitingClickHandler(NotiHandler handler) { m_pCallWaitingOption->setSelectHandler(handler);}
		void setCallWaitingCheckHandler(NotiHandler handler) { m_pCallWaitingOption->setCheckHandler(handler);}


		void checkWaitingOption(bool checked) {m_pCallWaitingOption->setCheckState(checked);}
		bool getWaitingOptionState() { return m_pCallWaitingOption->getCheckState();}
		void setWaitingOptionPending(bool isPending) {m_pCallWaitingOption->setCheckPending(isPending);}

	private:
		friend class BaseView;

		MoreView(gui::NaviItem *naviItem);
		virtual ~MoreView();
		virtual bool createViewContent() override;
		void onCallerIdPopupItemSelect(int value);
		void onCallerIdPopupDestroy();

	private:
		gui::Genlist *m_pGenlist;
		gui::OptionItem *m_pCallerIdOption;
		gui::OptionItem *m_pCallFwdOption;
		gui::CheckOptionItem *m_pCallWaitingOption;
		gui::Popup *m_pCallerIdPopup;
		CallerIdStatusChangeHandler m_idStatusHandler;
		NotiHandler m_CallerIdPopupHideCb;
	};

} }
#endif /* _VIEW_MORE_VIEW_H_ */
