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

#include "gui/Widgets/CheckboxListItem.h"
#include "gui/Widgets/ProcessingCircle.h"

namespace gui {

	CheckboxListItem::CheckboxListItem() :
		m_pCheckbox(nullptr),
		m_checkMode(NORMAL),
		m_checkStyle(CHECKBOX_DEFAULT),
		m_checkState(false)
	{
	}

	CheckboxListItem::~CheckboxListItem()
	{
	}

	void CheckboxListItem::setCheckState(bool state)
	{
		m_checkState = state;

		if (m_pCheckbox) {
			m_pCheckbox->setChecked(m_checkState);
		}
	}

	bool CheckboxListItem::getCheckState()
	{
		return m_checkState;
	}

	void CheckboxListItem::setCheckboxStyle(CheckboxStyle checkStyle)
	{
		if (m_checkStyle != checkStyle) {
			m_checkStyle = checkStyle;
			if (m_pCheckbox) {
				m_pCheckbox->setCheckStyle(checkStyle);
			}
		}
	}

	void CheckboxListItem::setCheckMode(CheckMode mode)
	{
		if (m_checkMode != mode) {
			m_checkMode = mode;
			update("elm.swallow.end", GL_PART_TYPE_CONTENT);
		}
	}

	CheckboxListItem::CheckMode CheckboxListItem::getCheckMode()
	{
		return m_checkMode;
	}

	void CheckboxListItem::setCheckHandler(ItemNotiHandler handler)
	{
		m_checkHandler = handler;
	}

	Evas_Object *CheckboxListItem::getContent(Evas_Object *genlist, const char *part)
	{
		if (strcmp(part, "elm.swallow.end") == 0 && m_checkMode != HIDDEN) {
			if (m_checkMode == PENDING) {
					ProcessingCircle *circlePendingItem = Widget::create<ProcessingCircle>(WidgetWrapper(genlist));
					RETVM_IF(!circlePendingItem, nullptr, "Failed to create processing item");
					return circlePendingItem->getEvasObject();
			} else if (m_checkMode != HIDDEN) {
					m_pCheckbox = Widget::create<Checkbox>(WidgetWrapper(genlist),
							m_checkStyle, (m_checkMode == SKIP_EVENTS));
					RETVM_IF(!m_pCheckbox, nullptr, "Failed to create checkbox!");

					m_pCheckbox->setChecked(m_checkState);
					m_pCheckbox->setCheckHandler(
							NotiHandler::wrap<CheckboxListItem, &CheckboxListItem::onCheckChanged>(this));
					m_pCheckbox->setDestroyHandler(
							NotiHandler::wrap<CheckboxListItem, &CheckboxListItem::onCheckboxDestroyed>(this));
				return m_pCheckbox->getEvasObject();
			}
		}

		return nullptr;
	}

	void CheckboxListItem::onCheckChanged()
	{
		bool isChecked = m_pCheckbox->isChecked();
		if (m_checkState != isChecked) {
			m_checkState = isChecked;
			if (m_checkHandler.assigned()) {
				m_checkHandler(*this);
			}
		}
	}

	void CheckboxListItem::onCheckboxDestroyed()
	{
		m_pCheckbox = nullptr;
	}

	void CheckboxListItem::onSelected()
	{
		if (m_checkMode == AUTO_CHECK && m_pCheckbox) {
			m_checkState = !m_pCheckbox->isChecked();
			m_pCheckbox->setChecked(m_checkState);
			elm_genlist_item_selected_set(m_pEOItem, EINA_FALSE);
			if (m_checkHandler.assigned()) {
				m_checkHandler(*this);
			}
		} else {
			GenlistItem::onSelected();
		}
	}
}
