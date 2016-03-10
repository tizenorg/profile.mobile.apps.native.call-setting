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
#include "Utils/Common.h"

namespace Widgets {

	class WidgetItem : private Utils::NonCopyable {
	public:
		template <typename ITEM_TYPE, typename... TYPE_ARGS>
		static ITEM_TYPE *create(TYPE_ARGS&&... args);
		static void destroy(WidgetItem *widgetItem);

		Elm_Object_Item *getElmObjectItem();
		Evas_Object *getParent();
		void setDestroyHandler(NotifyHandler handler);

	protected:
		WidgetItem();
		virtual ~WidgetItem();
		virtual void onElmObjectItemDel() {};

	protected:
		Elm_Object_Item *m_pEOItem;

	private:
		static void prepare(WidgetItem *widgetItem);
		void onElmObjectItemDelBase(Evas_Object *obj, void *event_info);

	private:
		NotifyHandler m_destroyHandler;
		bool m_isDestroying;
	};

	template <typename ITEM_TYPE, typename... TYPE_ARGS>
	ITEM_TYPE *WidgetItem::create(TYPE_ARGS&&... args)
	{
		ITEM_TYPE *instance = new ITEM_TYPE();
		if (!instance->initialize(std::forward<TYPE_ARGS>(args)...)) {
			delete instance;
			return nullptr;
		}
		WidgetItem::prepare(instance);
		return instance;
	}
}

#endif /* VIEW_WIDGETS_WIDGET_ITEM_H_ */
