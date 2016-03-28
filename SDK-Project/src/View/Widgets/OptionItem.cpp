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

#include "View/Widgets/OptionItem.h"

namespace Widgets {

	OptionItem::OptionItem() :
		m_pCheckbox(nullptr),
		m_clickHandler(),
		m_pCheckState(false)
	{
		setSelectHandler(NotifyHandler::wrap<OptionItem, &OptionItem::onItemSelected>(this));
	}

	OptionItem::~OptionItem()
	{
	}

	void OptionItem::setCheckState(bool state)
	{
		m_pCheckState = state;

		if (m_pCheckbox) {
			m_pCheckbox->setChecked(m_pCheckState);
		}
	}

	void OptionItem::setClickHandler(NotifyHandler handler)
	{
		m_clickHandler = handler;
	}

	Evas_Object *OptionItem::getContent(Evas_Object *genlist, const char *part)
	{
		if (strcmp(part, "elm.swallow.end") == 0) {
			m_pCheckbox = Widget::create<Checkbox>(WidgetWrapper(genlist), Checkbox::CHECKBOX_SWITCHER);
			RETVM_IF(!m_pCheckbox, nullptr, "Failed to create checkbox!");

			m_pCheckbox->setChecked(m_pCheckState);
			m_pCheckbox->setCheckHandler(makeNotifyHandler(OptionItem, onCheckChanged, this));
			m_pCheckbox->setDestroyHandler(makeNotifyHandler(OptionItem, onCheckboxDestroyed, this));

			return m_pCheckbox->getEvasObject();
		}

		return nullptr;
	}

	void OptionItem::onItemSelected()
	{
		m_pCheckState = !m_pCheckState;

		if (m_pCheckbox) {
			m_pCheckbox->setChecked(m_pCheckState);
		}

		if (m_clickHandler.assigned()) {
			m_clickHandler();
		}
	}

	void OptionItem::onCheckChanged()
	{
		m_pCheckState = !m_pCheckState;

		if (m_clickHandler.assigned()) {
			m_clickHandler();
		}
	}

	void OptionItem::onCheckboxDestroyed()
	{
		m_pCheckbox = nullptr;
	}
}
