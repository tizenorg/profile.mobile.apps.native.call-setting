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

#include "gui/Widgets/CheckOptionItem.h"

namespace gui {

	CheckOptionItem::CheckOptionItem() :
		m_pCheckbox(nullptr),
		m_checkState(false)
	{
	}

	CheckOptionItem::~CheckOptionItem()
	{
	}

	void CheckOptionItem::setCheckState(bool state)
	{
		m_checkState = state;

		if (m_pCheckbox) {
			m_pCheckbox->setChecked(m_checkState);
		}
	}

	bool CheckOptionItem::getCheckState()
	{
		return m_checkState;
	}

	void CheckOptionItem::setCheckHandler(NotifyHandler handler)
	{
		m_checkHandler = handler;
	}

	Evas_Object *CheckOptionItem::getContent(Evas_Object *genlist, const char *part)
	{

		if (strcmp(part, "elm.swallow.end") == 0) {
			m_pCheckbox = Widget::create<Checkbox>(WidgetWrapper(genlist), CHECKBOX_SWITCHER);
			RETVM_IF(!m_pCheckbox, nullptr, "Failed to create checkbox!");

			m_pCheckbox->setChecked(m_checkState);
			m_pCheckbox->setCheckHandler(
					NotifyHandler::wrap<CheckOptionItem, &CheckOptionItem::onCheckChanged>(this));
			m_pCheckbox->setDestroyHandler(
					NotifyHandler::wrap<CheckOptionItem, &CheckOptionItem::onCheckboxDestroyed>(this));
			return m_pCheckbox->getEvasObject();
		}

		return nullptr;
	}

	void CheckOptionItem::onCheckChanged()
	{
		bool isChecked = m_pCheckbox->isChecked();
		if (m_checkState != isChecked) {
			m_checkState = isChecked;
			if (m_checkHandler.assigned()) {
				m_checkHandler();
			}
		}
	}

	void CheckOptionItem::onCheckboxDestroyed()
	{
		m_pCheckbox = nullptr;
	}
}
