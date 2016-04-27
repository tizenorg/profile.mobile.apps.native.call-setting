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

#include "gui/Widgets/RadioGroup.h"
#include "gui/Widgets/RadioOptionItem.h"

namespace gui {

	RadioGroup::RadioGroup() :
		m_pGenlist(nullptr),
		m_pRadioGroup(nullptr)
	{
	}

	bool RadioGroup::initialize(const Widget &parent)
	{
		m_pEvasObject = elm_box_add(parent.getEvasObject());
		RETVM_IF(!m_pEvasObject, false, "Failed to create box, unknown error");
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		m_pGenlist = Widget::create<Genlist>(WidgetWrapper(m_pEvasObject));
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");
		elm_box_pack_end(m_pEvasObject, m_pGenlist->getEvasObject());
		m_pGenlist->setContentMinLimit(false, true);
		m_pGenlist->show();

		m_pRadioGroup = elm_radio_add(m_pGenlist->getEvasObject());
		RETVM_IF(!m_pRadioGroup, false, "Failed to create radio group!");

		elm_radio_state_value_set(m_pRadioGroup, -1);
		return true;
	}

	bool RadioGroup::addItem(util::TString text, int itemIndex)
	{
		RadioOptionItem *item = m_pGenlist->appendItem<RadioOptionItem>(m_pRadioGroup, itemIndex, std::move(text));
		RETVM_IF(!item, false, "Failed to add item!");

		item->setSelectHandler(ItemNotiHandler::wrap<RadioGroup, &RadioGroup::onRadioItemSelected>(this));
		return true;
	}

	void RadioGroup::onRadioItemSelected(WidgetItem *item)
	{
		if (m_radioSelectHandler.assigned()) {
			m_radioSelectHandler(elm_radio_value_get(m_pRadioGroup));
		}
	}

	void RadioGroup::selectRadioItem(int itemIndex)
	{
		elm_radio_value_set(m_pRadioGroup, itemIndex);
	}
}
