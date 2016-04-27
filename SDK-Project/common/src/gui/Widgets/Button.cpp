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

#include "gui/Widgets/Button.h"

namespace gui {

	Button::Button()
	{
	}

	Button::~Button()
	{
	}

	bool Button::initialize(const Widget &parent, const char *style, const util::TString &text)
	{
		Evas_Object *btnParent = parent.getEvasObject();
		RETVM_IF(!btnParent, false, "Failed to create checkbox: Parent in null");

		m_pEvasObject = elm_button_add(btnParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create button: Internal error");
		if (style) {
			elm_object_style_set(m_pEvasObject, style);
		} else {
			elm_object_style_set(m_pEvasObject, "default");
		}

		setText(text);
		evas_object_smart_callback_add(m_pEvasObject, "clicked",
			EoSmartCb::make<Button, &Button::onButtonClick>(), this);

		evas_object_show(m_pEvasObject);
		return true;
	}

	void Button::setClickHandler(NotiHandler handler)
	{
		m_clickHandler = handler;
	}

	void Button::onButtonClick(Evas_Object *obj, void *event_info)
	{

		if (m_clickHandler.assigned()) {
			m_clickHandler();
		}
	}
}
