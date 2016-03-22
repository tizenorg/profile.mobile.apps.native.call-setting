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

#include "View/Widgets/Genlist.h"
#include "View/Widgets/GenlistItem.h"

namespace Widgets {

	GenlistItem::GenlistItem() :
		WidgetItem(),
		m_pItemClass(nullptr),
		m_onSelectHandler()
	{
	}

	GenlistItem::~GenlistItem()
	{
		if (m_pItemClass) {
			elm_genlist_item_class_free(m_pItemClass);
		}
	}

	bool GenlistItem::initialize(Genlist &list, ItemAddMethod itemAdd)
	{
		RETVM_IF(m_pEOItem, false, "Second initialize of WidgetItem is forbidden");
		RETVM_IF(!itemAdd, false, "Invalid fabric method");

		m_pEOItem = itemAdd(list, this);

		if (m_pEOItem) {
			return true;
		}

		return false;
	}

	bool GenlistItem::initialize(Genlist &list, GenlistItem &parentItem, ItemInsertMethod itemInsert)
	{
		RETVM_IF(m_pEOItem, false, "Second initialize of WidgetItem is forbidden");
		RETVM_IF(!itemInsert, false, "Invalid fabric method");

		m_pEOItem = itemInsert(list, parentItem, this);

		if (m_pEOItem) {
			return true;
		}

		return false;
	}

	void GenlistItem::setSelectHandler(NotifyHandler handler)
	{
		m_onSelectHandler = handler;
	}

	Elm_Genlist_Item_Class *GenlistItem::getItemClass()
	{
		static Elm_Genlist_Item_Class itc = createItemClass("type1");
		return &itc;
	}

	Elm_Genlist_Item_Class GenlistItem::createItemClass(const char *style)
	{
		Elm_Genlist_Item_Class itc = { ELM_GEN_ITEM_CLASS_HEADER, 0 };
		itc.item_style = style;
		itc.decorate_item_style = NULL;
		itc.decorate_all_item_style = NULL;
		itc.func.text_get = [](void *data, Evas_Object *obj, const char *part) -> char * {
				return static_cast<GenlistItem *>(data)->getText(part);
			};

		itc.func.content_get = [](void *data, Evas_Object *obj, const char *part) -> Evas_Object * {
				return static_cast<GenlistItem *>(data)->getContent(part);
			};

		itc.func.state_get = [](void *data, Evas_Object *obj, const char *part) -> Eina_Bool {
				return static_cast<GenlistItem *>(data)->getState(part);
			};

		itc.func.del = NULL;
		return itc;
	}

	void GenlistItem::onSelected()
	{
		elm_genlist_item_selected_set(m_pEOItem, EINA_FALSE);

		if (m_onSelectHandler.assigned()) {
			m_onSelectHandler();
		}
	}
}
