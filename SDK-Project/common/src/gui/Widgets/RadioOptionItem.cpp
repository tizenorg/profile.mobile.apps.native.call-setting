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

#include "gui/Widgets/RadioOptionItem.h"

namespace gui {

RadioOptionItem::RadioOptionItem() :
		m_pRadioGroup(nullptr),
		m_value(-1)
	{
	}

	RadioOptionItem::~RadioOptionItem()
	{
	}

	bool RadioOptionItem::initialize(ItemAddMethod createItem, Evas_Object *radioGroup, int value, util::TString text,
						ItemSelectionMode selectMode)
	{
		RETVM_IF(!radioGroup, false, "Invalid args!");

		if (!SimpleListItem::initialize(createItem, std::move(text), selectMode)) {
			ERR("Failed to initialize TextItem");
			return false;
		}

		m_pRadioGroup = radioGroup;
		m_value = value;
		return true;
	}

	Evas_Object *RadioOptionItem::getContent(Evas_Object *genlist, const char *part)
	{
		if (strcmp(part, "elm.swallow.end") == 0) {
			Evas_Object *radio = elm_radio_add(genlist);
			elm_radio_group_add(radio, m_pRadioGroup);
			elm_radio_state_value_set(radio,  m_value);
			return radio;
		}

		return nullptr;
	}

	void RadioOptionItem::onSelected()
	{
		elm_radio_value_set(m_pRadioGroup, m_value);
		GenlistItem::onSelected();
	}
}
