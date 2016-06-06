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

#include "gui/NaviItem.h"
#include "gui/Widget.h"

namespace gui {

	NaviItem::NaviItem() :
		WidgetItem(),
		m_pBackButton(nullptr)
	{
	}

	NaviItem::~NaviItem()
	{
	}

	bool NaviItem::initialize(Elm_Object_Item *item, Evas_Object *backButton)
	{
		RETVM_IF(m_pEOItem, false, "Second initialize of WidgetItem is forbidden");
		RETVM_IF(!item || !backButton, false, "Invalid args");

		m_pEOItem = item;
		m_pBackButton = backButton;

		showBackButton();

		return true;
	}

	void NaviItem::showTitle()
	{
		elm_naviframe_item_title_enabled_set(m_pEOItem, EINA_TRUE, EINA_TRUE);
	}

	void NaviItem::hideTitle()
	{
		elm_naviframe_item_title_enabled_set(m_pEOItem, EINA_FALSE, EINA_FALSE);
	}

	void NaviItem::setTitleText(const util::TString &text)
	{
		if (text.isTranslatable()) {
			if (text.hasDomain()) {
				elm_object_item_domain_translatable_part_text_set(m_pEOItem, "elm.text.title", text.getDomain(), ne(text));
			} else {
				elm_object_item_translatable_part_text_set(m_pEOItem, "elm.text.title", ne(text));
			}
		} else {
			elm_object_item_part_text_set(m_pEOItem, "elm.text.title", ne(text));
		}
	}

	void NaviItem::setContent(const Widget &content)
	{
		setObjectToPart(content.getEvasObject(), "elm.swallow.content");
	}

	void NaviItem::unsetContent()
	{
		unsetObjectFromPart("elm.swallow.content");
	}

	void NaviItem::setTitleLeftBtn(const Widget &content)
	{
		setObjectToPart(content.getEvasObject(), "title_left_btn");
	}

	void NaviItem::unsetTitleLeftBtn()
	{
		unsetObjectFromPart("title_left_btn");
	}

	void NaviItem::setTitleRightBtn(const Widget &content)
	{
		setObjectToPart(content.getEvasObject(), "title_right_btn");
	}

	void NaviItem::unsetTitleRightBtn()
	{
		unsetObjectFromPart("title_right_btn");
	}

	void NaviItem::showBackButton()
	{
		setObjectToPart(m_pBackButton, "prev_btn");
	}

	void NaviItem::hideBackButton()
	{
		unsetObjectFromPart("prev_btn");
	}

	void NaviItem::setObjectToPart(Evas_Object *content, const char *part)
	{
		RETM_IF(!content || !part, "Invalid args");

		elm_object_item_part_content_set(m_pEOItem, part, content);
		evas_object_show(content);
	}

	void NaviItem::unsetObjectFromPart(const char *part)
	{
		RETM_IF(!part, "Invalid args");

		Evas_Object *content = elm_object_item_part_content_get(m_pEOItem, part);

		if (content) {
			evas_object_hide(content);
		}
	}
}
