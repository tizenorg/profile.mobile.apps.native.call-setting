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

#ifndef VIEW_WIDGETS_WIDGET_ITEM_H_
#define VIEW_WIDGETS_WIDGET_ITEM_H_

#include <Elementary.h>
#include <functional>
#include "Utils/Logger.h"

namespace Widgets {
	class WidgetWrapper;
	class WidgetItem {
	public:
		template <typename ITEM_TYPE>
		static ITEM_TYPE *create(Elm_Object_Item *item);
		static void destroy(WidgetItem *widgetItem);

		Elm_Object_Item *getElmObjectItem();
		WidgetWrapper *getParent();
		bool setData(void *data);
		void *getData();
		bool setDestroyCallback(std::function<void(WidgetItem *)> callback);
		void unsetDestroyCallback();

		WidgetItem(const WidgetItem &item) = delete;
		WidgetItem(WidgetItem &&item) = delete;
		WidgetItem &operator=(const WidgetItem &item) = delete;
		WidgetItem &operator=(WidgetItem &&item) = delete;

	protected:
		WidgetItem();
		virtual ~WidgetItem();
		void onElmObjectItemDel();

	protected:
		Elm_Object_Item *m_pEOItem;

	private:
		std::function<void(WidgetItem *)> m_OnDestroyCb;
		void *m_pItemData;
	};

	template <typename ITEM_TYPE>
	ITEM_TYPE *WidgetItem::create(Elm_Object_Item *item)
	{
		RETVM_IF(!item, nullptr, "Invalid parameter");

		ITEM_TYPE *instance = new ITEM_TYPE();
		if (!instance->initialize(item)) {
			delete instance;
			return nullptr;
		}

		return instance;
	}
}

#endif /* VIEW_WIDGETS_WIDGET_ITEM_H_ */
