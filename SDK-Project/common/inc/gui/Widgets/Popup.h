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

#include "gui/Base/Widget.h"
#include "gui/Widgets/Button.h"

#define POPUP_BUTTON_MAX_COUNT 3

namespace gui {

	typedef enum {
		POPUP_BUTTON_LEFT,
		POPUP_BUTTON_MIDDLE,
		POPUP_BUTTON_RIGHT,
	} PopupButtonPosition;

	typedef util::Delegate<bool()> PopupClickHandler;

	class Popup : public Widget {
	public:
		void setTitle(const util::TString &text);
		virtual void setContent(const Widget &parent);
		void unsetContent();
		bool addButton(PopupButtonPosition position, const util::TString &text, PopupClickHandler clickHandler);
		void setButtonDisabled(PopupButtonPosition position, bool disabled);
		void setBackClickHandler(PopupClickHandler handler) { m_backClickHandler = handler; }
		void setMenuClickHandler(PopupClickHandler handler) { m_moreClickHandler = handler; }
		void setBlockClickHandler(PopupClickHandler handler) { m_blockClickHandler = handler; }

	protected:
		Popup() {};
		virtual ~Popup() {}
		bool initialize(const Widget &parent, const util::TString &title = "", Widget *content = nullptr, const util::TString &text = "");
		void onButtonPressed(Widget &sender);
		void onBackPressed(Evas_Object *obj, void *eventInfo);
		void onMenuPressed(Evas_Object *obj, void *eventInfo);
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
