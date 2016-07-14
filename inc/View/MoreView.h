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

#include "gui/BaseView.h"
#include "gui/Genlist.h"
#include "gui/SimpleListItem.h"
#include "gui/DoubleTextListItem.h"
#include "gui/Popup.h"

namespace CallSettings { namespace View {


	/*
	 * @brief Represents More view instance
	 */
	class MoreView : public gui::BaseView {
	public:
		/*
		 * @brief Caller id status type
		 */
		typedef enum {
			CALLER_ID_STATUS_DEFAULT,
			CALLER_ID_STATUS_SHOW,
			CALLER_ID_STATUS_HIDE,
		} CallerIdStatus;


		/*
		 * @brief Callback called when caller id new status changed in Caller Id settings popup
		 */
		typedef util::Delegate <void(CallerIdStatus)> CallerIdStatusChangeHandler;

		/*
		 * @brief Set caller id status text in caller id list item
		 * @param[in]	value	caller id value
		 */
		void setCallerIdStatus(CallerIdStatus value);

		/*
		 * @brief Set handler for click caller id list item event
		 * @param[in]	handler		event handler
		 */
		void setCallerIdClickHandler(NotiHandler handler) { m_callerIdClickHandeler = handler;}

		/*
		 * @brief Set handler for click call forwarding list item event
		 * @param[in]	handler		event handler
		 */
		void setCallForwardClickHandler(NotiHandler handler) { m_callFwdClickHandler = handler;}

		/*
		 * @brief Set handler for check call waiting option checkbox
		 * @param[in]	handler		event handler
		 */
		void setCallWaitingCheckHandler(NotiHandler handler) { m_callWaitingCheckHandler = handler;}

		/*
		 * @brief Show caller id option radiogroup popup
		 * @param[in]	selectedValue		Value of caller Id which will be marked as current
		 * @param[in]	statusHandler		Handler for option select event
		 * @param[in]	popupHideHandler	Handler for popup hide event
		 */
		void showCallerIdPopup(CallerIdStatus selectedValue, CallerIdStatusChangeHandler statusHandler, NotiHandler popupHideHandler);

		/*
		 * @brief Hide caller is option setup popup
		 */
		void hideCallerIdPopup();

		/*
		 * @brief Set checkbox state in Call Waiting list item
		 * @param[in]	checked		checkbox state, true if checked, otherwise false
		 */
		void setWaitingOptionChecked(bool checked) {m_pCallWaitingOption->setCheckState(checked);}

		/*
		 * @return true if Call waiting option is checked, otherwise false
		 */
		bool isWaitingOptionChecked() { return m_pCallWaitingOption->getCheckState();}

		/*
		 * @brief	Set waiting option checkbox into pending state
		 * @param[in]	If true checkbox will be hide and pending circle will be shown, otherwise checkbox will be shown
		 */
		void setWaitingOptionPending(bool isPending);


	private:
		friend class BaseView;

		MoreView(gui::NaviItem *naviItem);
		virtual ~MoreView();

		bool initialize();
		void onOptionSelected(gui::WidgetItem &item);
		void onCallerIdPopupDestroy();
		void onCallerIdPopupItemSelect(int value);

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
