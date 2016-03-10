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

#include "View/Widgets/Widget.h"
#include "View/Widgets/NaviItem.h"
#include "Utils/Logger.h"

namespace View { namespace Widgets {

	bool NaviItem::initialize(Elm_Object_Item *item)
	{
		RETVM_IF(!m_pEOItem, false, "Second initialize of WidgetItem is forbidden");

		m_pEOItem = item;
		elm_object_item_data_set(m_pEOItem, static_cast<NaviItem *>(this));
		elm_object_item_del_cb_set(m_pEOItem, [](void *data, Evas_Object *obj, void *event_info) {
			return static_cast<NaviItem *>(data)->onElmObjectItemDel();
		});

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

	void NaviItem::setTitleText(const char *title)
	{
		if (title) {
			elm_object_item_translatable_part_text_set(m_pEOItem, "elm.text.title", title);
			elm_naviframe_item_title_enabled_set(m_pEOItem, EINA_TRUE, EINA_TRUE);
		}
	}

	void NaviItem::setContent(Widget *content)
	{
		RETM_IF(!content || !content->getEvasObject(), "Invalid parameter");
		elm_object_item_part_content_set(m_pEOItem, "elm.swallow.content", content->getEvasObject());
	}

} }
