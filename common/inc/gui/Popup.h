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


#ifndef _GUI_POPUP_H_
#define _GUI_POPUP_H_

#include "Widget.h"
#include "Button.h"

#define POPUP_BUTTON_MAX_COUNT 3

namespace gui {

	/*
	 * @brief Popup bottom button position type
	 */
	typedef enum {
		POPUP_BUTTON_LEFT,
		POPUP_BUTTON_MIDDLE,
		POPUP_BUTTON_RIGHT,
	} PopupButtonPosition;

	/**
	 * @brief Popup click callback
	 * @return true to destroy the Popup, otherwise false
	 */
	typedef util::Delegate<bool()> PopupClickHandler;

	/*
	 * @brief Represents Popup widget
	 */
	class Popup : public Widget {
	public:
		/*
		 * @brief Set popup title
		 * @param[in]	text	Title text
		 */
		void setTitle(const util::TString &text);

		/*
		 * @brief Set popup content
		 * @param[in]	content		Content widget
		 */
		virtual void setContent(const Widget &content);

		/*
		 * @brief Unset popup content
		 */
		void unsetContent();

		/*
		 * @brief Add bottom button to popup widget
		 * @param[in]	position		Button position
		 * @param[in]	text			Button text label
		 * @param[in]	clickHandler	Button click handler
		 * @return true on success, otherwise false
		 */
		bool addButton(PopupButtonPosition position, const util::TString &text, PopupClickHandler clickHandler);

		/*
		 * @brief Disable/Enable popup button
		 * @param[in]	position		Button position
		 * @param[in]	disabled		Button disable state
		 */
		void setButtonDisabled(PopupButtonPosition position, bool disabled);

		/*
		 * @brief Set popup back button click handler. Handler invokes when back button clicked and popup is top most widget
		 * @param[in]	handler		back button click handler.
		 */
		void setBackClickHandler(PopupClickHandler handler) { m_backClickHandler = handler; }

		/*
		 * @brief Set popup menu button click handler. Handler invokes when menu button clicked and popup is top most widget
		 * @param[in]	handler		back button click handler.
		 */
		void setMenuClickHandler(PopupClickHandler handler) { m_moreClickHandler = handler; }

		/*
		 * @brief Set popup block area click handler. Handler invokes when popup block area clicked.
		 * @param[in]	handler		back button click handler.
		 */
		void setBlockClickHandler(PopupClickHandler handler) { m_blockClickHandler = handler; }

	protected:
		Popup() {};
		virtual ~Popup() {}
		/*
		 * @brief Popup initialization
		 * @param[in]	parent		Popup widget parent
		 * @param[in]	title		Popup title text
		 * @param[in]	content		Poup Content widget
		 * @param[in]	text		Popup text
		 * @return	true on success, otherwise false
		 */
		bool initialize(const Widget &parent, const util::TString &title = "", Widget *content = nullptr, const util::TString &text = "");

		/*
		 * @brief Popup button press callback
		 * @param[in]	sender		Button which dispatch click event
		 */
		void onButtonPressed(Widget &sender);

		/*
		 * @brief Popup back button click callback
		 * @param[in]	obj			Popup evas object entity
		 * @param[in]	event_info	Click callback event info data.
		 */
		void onBackPressed(Evas_Object *obj, void *eventInfo);

		/*
		 * @brief Popup menu button click callback
		 * @param[in]	obj			Popup evas object entity
		 * @param[in]	event_info	Click callback event info data.
		 */
		void onMenuPressed(Evas_Object *obj, void *eventInfo);

		/*
		 * @brief Popup block area click callback
		 * @param[in]	obj			Popup evas object entity
		 * @param[in]	event_info	Click callback event info data.
		 */
		void onBlockAreaClick(Evas_Object *obj, void *eventInfo);

	private:
		struct PopupButton {
			PopupClickHandler clickHandler;
			Button *pButton;
		};

	private:
		friend class Widget; // to be used in Widget::create

		PopupButton m_buttons[POPUP_BUTTON_MAX_COUNT];
		PopupClickHandler m_backClickHandler;
		PopupClickHandler m_moreClickHandler;
		PopupClickHandler m_blockClickHandler;
	};

}

#endif /* _GUI_POPUP_H_ */
