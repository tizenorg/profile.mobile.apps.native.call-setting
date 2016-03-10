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
#include "View/Widgets/WidgetItem.h"
#include "View/Widgets/Widget.h"
#include "Utils/Logger.h"

namespace Widgets {
	WidgetItem::WidgetItem() :
		m_pEOItem(nullptr),
		m_destroyHandler(),
		m_isDestroying(false)
	{
	}

	void WidgetItem::destroy(WidgetItem *item)
	{
		if (item) {
			item->m_isDestroying = true;
			delete item;
		}
	}

	void WidgetItem::onElmObjectItemDelBase(Evas_Object *obj, void *event_info)
	{
		m_pEOItem = nullptr;
		onElmObjectItemDel();
		if (! m_isDestroying) {
			delete this;
		}
	}

	void WidgetItem::prepare(WidgetItem *widgetItem)
	{
		elm_object_item_data_set(widgetItem->m_pEOItem, widgetItem);
		elm_object_item_del_cb_set(widgetItem->m_pEOItem,
				makeEvasSmartCb(WidgetItem, &WidgetItem::onElmObjectItemDelBase));
	}

	WidgetItem::~WidgetItem()
	{
		elm_object_item_del(m_pEOItem);

		if (m_destroyHandler.assigned()) {
			m_destroyHandler();
		}
	}

	Elm_Object_Item *WidgetItem::getElmObjectItem()
	{
		return m_pEOItem;
	}

	Evas_Object *WidgetItem::getParent()
	{
		Evas_Object *parentObj = elm_object_item_widget_get(m_pEOItem);
		if (parentObj) {
			return parentObj;
		}

		return nullptr;
	}

	void WidgetItem::setDestroyHandler(NotifyHandler handler)
	{
		m_destroyHandler = handler;
	}
}
