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


#include "gui/Widgets/DoubleTextListItem.h"

namespace gui {
	DoubleTextListItem::DoubleTextListItem() :
		m_isMultiline(false)
	{
	}

	bool DoubleTextListItem::initialize(ItemAddMethod createItem, util::TString text, util::TString subText,
			bool isMultiline, ItemSelectionMode selectMode)
	{
		m_subText = std::move(subText);
		m_isMultiline = isMultiline;

		return CheckboxListItem::initialize(createItem, std::move(text), selectMode);
	}

	Elm_Genlist_Item_Class *DoubleTextListItem::getItemClass()
	{
		if (m_isMultiline) {
			static Elm_Genlist_Item_Class itc = createItemClass("multiline");
			return &itc;
		}
		return SimpleListItem::getItemClass();
	}

	void DoubleTextListItem::setSubText(util::TString subText)
	{
		m_subText = std::move(subText);

		update("*", GL_PART_TYPE_TEXT);
	}

	char *DoubleTextListItem::getText(const char *part)
	{
		if (( m_isMultiline && (strcmp(part, "elm.text.multiline") == 0)) ||
			(!m_isMultiline && (strcmp(part, "elm.text.sub") == 0))) {
			return util::strDupSafe(util::ne(m_subText.translate()));
		} else {
			return SimpleListItem::getText(part);
		}
	}
}
