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

namespace View { namespace Widgets {
	WidgetItem::WidgetItem() :
		m_pEOItem(nullptr),
		m_pOnDestroyCb(nullptr),
		m_pItemData(nullptr)
	{
	}

	void WidgetItem::destroy(WidgetItem *item)
	{
		if (item) {
			delete item;
		}
	}

	void WidgetItem::onElmObjectItemDel()
	{
		if (m_pOnDestroyCb && m_pEOItem) {
			m_pOnDestroyCb(this);
		}

		m_pEOItem = nullptr;
		delete this;
	}

	WidgetItem::~WidgetItem()
	{
		if (m_pEOItem) {
			elm_object_item_del(m_pEOItem);
		}
	}

	Elm_Object_Item *WidgetItem::getElmObjectItem()
	{
		return m_pEOItem;
	}

	WidgetWrapper *WidgetItem::getParent()
	{
		Evas_Object *parentObj = elm_object_item_widget_get(m_pEOItem);
		if (parentObj) {
			WidgetWrapper *parent = new WidgetWrapper(parentObj);
			return parent;
		}
		return nullptr;
	}

	bool WidgetItem::setData(void *data)
	{
		RETVM_IF(!data, false, "Invalid parameter");
		if (m_pItemData) {
			ERR("Can not set WidgetItem data twice!");
			return false;
		} else {
			return true;
		}

	}

	void *WidgetItem::getData()
	{
		if (m_pItemData) {
			return (m_pItemData);
		} else {
			return nullptr;
		}

	}

	bool WidgetItem::setDestroyCallback(std::function<void(WidgetItem *)> callback)
	{
		if (callback && !m_pOnDestroyCb) {
			m_pOnDestroyCb = std::move(callback);
			return true;
		}

		return false;
	}

	void WidgetItem::unsetDestroyCallback()
	{
		if (m_pOnDestroyCb) {
			m_pOnDestroyCb = nullptr;
		}
	}
} }
