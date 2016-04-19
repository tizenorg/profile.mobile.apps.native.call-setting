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
		m_subText(),
		m_isSubTextLocalized(false),
		m_isMultiline(false)
	{
	}

	bool DoubleTextListItem::initialize(ItemAddMethod createItem, const char *text, const char *subText, bool isTextLocalized,
			bool isSubTextLocalized, bool isMultiline, ItemSelectionMode selectMode)
	{
		if (subText) {
			m_subText.assign(subText);
			m_isSubTextLocalized = isSubTextLocalized;
		}

		m_isMultiline = isMultiline;

		return CheckboxListItem::initialize(createItem, text, isTextLocalized, selectMode);
	}

	Elm_Genlist_Item_Class *DoubleTextListItem::getItemClass()
	{
		const char *className = (m_isMultiline) ? "multiline" : "type1";
		static Elm_Genlist_Item_Class itc = createItemClass(className);
		return &itc;
	}

	void DoubleTextListItem::setSubText(const char *subtext, bool isLocalized)
	{
		if (!subtext) {
			m_subText.erase();
			m_isSubTextLocalized = false;
		} else {
			m_subText.assign(subtext);
			m_isSubTextLocalized = isLocalized;
		}

		update("*", GL_PART_TYPE_TEXT);
	}

	char *DoubleTextListItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text.multiline") == 0) {
			return makeTextLabel(m_subText, m_isSubTextLocalized);
		} else if (strcmp(part, "elm.text.sub") == 0) {
			return makeTextLabel(m_subText, m_isSubTextLocalized);
		} else {
			return SimpleListItem::getText(part);
		}
	}
}
