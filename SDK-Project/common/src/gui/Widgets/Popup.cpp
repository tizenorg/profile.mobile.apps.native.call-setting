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

#include "gui/Widgets/Popup.h"

namespace gui {

	const char *buttonParts[POPUP_BUTTON_MAX_COUNT] = { "button1", "button3", "button2"};

	bool Popup::initialize(const Widget &parent, const util::TString &title, Widget *content, const util::TString &text)
	{
		m_pEvasObject = elm_popup_add(parent.getEvasObject());
		RETVM_IF(!m_pEvasObject, false, "Failed to create Popup");

		elm_popup_align_set(m_pEvasObject, ELM_NOTIFY_ALIGN_FILL, 1.0);

		eext_object_event_callback_add(m_pEvasObject, EEXT_CALLBACK_BACK,
			EoSmartCb::make<Popup, &Popup::onBackPressed>(), this);

		eext_object_event_callback_add(m_pEvasObject, EEXT_CALLBACK_MORE,
			EoSmartCb::make<Popup, &Popup::onMenuPressed>(), this);

		evas_object_smart_callback_add(m_pEvasObject, "block,clicked",
			EoSmartCb::make<Popup, &Popup::onBlockAreaClick>(), this);

		if (title.isNotEmpty()) {
			setTitle(title);
		}

		if (text.isNotEmpty()) {
			setText(text);
		}

		if (content) {
			setContent(*content);
		}

		evas_object_show(m_pEvasObject);
		return true;
	}

	void Popup::setTitle(const util::TString &text)
	{
		setPartText("title,text", text);
	}

	void Popup::setContent(const Widget &parent)
	{
		elm_object_content_set(m_pEvasObject, parent.getEvasObject());
		evas_object_show(parent.getEvasObject());
	}

	void Popup::unsetContent()
	{
		elm_object_content_unset(m_pEvasObject);
	}

	bool Popup::addButton(PopupButtonPosition position, const util::TString &text, PopupClickHandler clickHandler)
	{
		Button *button = Widget::create<Button>(*this, "popup", text);
		RETVM_IF(!button, false, "Failed to create button");

		setPartContent(buttonParts[position], button);

		button->setTag(position);

		m_buttons[position].clickHandler = clickHandler;
		m_buttons[position].pButton = button;

		button->setClickHandler(WidgetNotiHandler::wrap<Popup, &Popup::onButtonPressed>(this));

		return true;
	}

	void Popup::setButtonDisabled(PopupButtonPosition position, bool disabled)
	{
		if (m_buttons[position].pButton) {
			m_buttons[position].pButton->setDisabled(disabled);
		}
	}

	void Popup::onButtonPressed(Widget &sender)
	{
		PopupButton &btn = m_buttons[sender.getTag()];
		if (btn.clickHandler.assigned() && btn.clickHandler()) {
			Widget::destroy(this);
		}
	}

	void Popup::onBackPressed(Evas_Object *obj, void *eventInfo)
	{
		if (m_backClickHandler.assigned()) {
			if (m_backClickHandler()) {
				Widget::destroy(this);
			}
		} else {
			Widget::destroy(this);
		}

	}

	void Popup::onMenuPressed(Evas_Object *obj, void *eventInfo)
	{
		if (m_moreClickHandler.assigned() && m_moreClickHandler()) {
			Widget::destroy(this);
		}
	}

	void Popup::onBlockAreaClick(Evas_Object *obj, void *eventInfo)
	{
		if (m_blockClickHandler.assigned()) {
			if (m_blockClickHandler()) {
				Widget::destroy(this);
			}
		} else {
			Widget::destroy(this);
		}
	}
}
