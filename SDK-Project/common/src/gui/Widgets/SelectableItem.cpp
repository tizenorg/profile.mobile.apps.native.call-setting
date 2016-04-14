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

#include "gui/Widgets/SelectableItem.h"

namespace gui {

	SelectableItem::SelectableItem() :
		m_pCheckbox(nullptr),
		m_isItemSelected(false),
		m_isSelectModeActive(false)
	{
	}

	SelectableItem::~SelectableItem()
	{
	}

	void SelectableItem::enableSelectMode()
	{
		m_isSelectModeActive = true;
		update("*", GL_PART_TYPE_CONTENT);
	}

	void SelectableItem::disableSelectMode()
	{
		m_isSelectModeActive = false;
		m_isItemSelected = false;
		update("*", GL_PART_TYPE_CONTENT);
	}

	void SelectableItem::markAsSelected(bool selectState)
	{
		if(m_isSelectModeActive) {
			m_isItemSelected = selectState;
			m_pCheckbox->setChecked(m_isItemSelected);
		}
	}

	bool SelectableItem::getSelectedState()
	{
		return m_isItemSelected;
	}

	Evas_Object *SelectableItem::getContent(Evas_Object *genlist, const char *part)
	{
		if (strcmp(part, "elm.swallow.end") == 0) {
			if(m_isSelectModeActive) {
				m_pCheckbox = Widget::create<Checkbox>(WidgetWrapper(genlist), CHECKBOX_DEFAULT);
				RETVM_IF(!m_pCheckbox, nullptr, "Failed to create checkbox!");

				m_pCheckbox->setChecked(m_isItemSelected);
				m_pCheckbox->setCheckHandler(
						NotiHandler::wrap<SelectableItem, &SelectableItem::onCheckChanged>(this));
				m_pCheckbox->setDestroyHandler(
						NotiHandler::wrap<SelectableItem, &SelectableItem::onCheckboxDestroyed>(this));
				return m_pCheckbox->getEvasObject();
			}
		}

		return nullptr;
	}

	void SelectableItem::onSelected()
	{
		GenlistItem::onSelected();
		if (m_isSelectModeActive) {
			m_isItemSelected = !m_pCheckbox->isChecked();
			m_pCheckbox->setChecked(m_isItemSelected);
		}

		if (m_selectHandler.assigned()) {
			m_selectHandler(this);
		}
	}

	void SelectableItem::onCheckChanged()
	{

		bool isChecked = m_pCheckbox->isChecked();
		if (m_isItemSelected != isChecked) {
			m_isItemSelected = isChecked;
		}

		if (m_selectHandler.assigned()) {
			m_selectHandler(this);
		}
	}

	void SelectableItem::onCheckboxDestroyed()
	{
		m_pCheckbox = nullptr;
	}

	Elm_Genlist_Item_Class *SelectableGroupItem::getItemClass()
	{	static Elm_Genlist_Item_Class itc = createItemClass("group_index");
		return &itc;
	}

	char *SelectableGroupItem::getText(const char *part)
	{
		return SimpleTextItem::getText(part);
	}
}
