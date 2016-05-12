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

#include "gui/Base/WidgetItem.h"

namespace gui {

	WidgetItem::WidgetItem() :
		m_pEOItem(nullptr),
		m_tag(0)
	{
	}

	WidgetItem::~WidgetItem()
	{
		if (m_destroyHandler.assigned()) {
			m_destroyHandler();
		}
	}

	void WidgetItem::doDestroy()
	{
		if (m_pEOItem) {
			Elm_Object_Item *eoItem = m_pEOItem;
			m_pEOItem = nullptr;
			elm_object_item_del(eoItem);
		} else {
			delete this;
		}
	}

	void WidgetItem::onElmObjectItemDelBase(Evas_Object *obj, void *event_info)
	{
		m_pEOItem = nullptr;
		delete this;
	}

	bool WidgetItem::prepare(WidgetItem *widgetItem)
	{
		WidgetItem *itemData = static_cast<WidgetItem *>(elm_object_item_data_get(widgetItem->m_pEOItem));

		if (!itemData) {
			elm_object_item_data_set(widgetItem->m_pEOItem, widgetItem);
		} else if (itemData != widgetItem) {
			ERR("Item data pointer is not set!");
			return false;
		}

		elm_object_item_del_cb_set(widgetItem->m_pEOItem,
			EoSmartCb::make<WidgetItem, &WidgetItem::onElmObjectItemDelBase>());

		return true;
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

	void WidgetItem::setDestroyHandler(NotiHandler handler)
	{
		m_destroyHandler = handler;
	}

	void WidgetItem::setTag(int tag)
	{
		m_tag = tag;
	}

	int WidgetItem::getTag() const
	{
		return m_tag;
	}

	void WidgetItem::setDisabled(bool disabledState)
	{
		elm_object_item_disabled_set(m_pEOItem, TO_EINA_BOOL(disabledState));

	}

}
