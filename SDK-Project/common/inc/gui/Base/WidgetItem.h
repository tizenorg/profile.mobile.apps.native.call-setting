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

#ifndef _GUI_WIDGET_ITEM_H_
#define _GUI_WIDGET_ITEM_H_

#include "gui/GuiTypes.h"

namespace gui {

	class WidgetItem : private util::NonCopyable {
	public:
		template <typename ITEM_TYPE, typename... TYPE_ARGS>
		static ITEM_TYPE *create(TYPE_ARGS&&... args);

		template <typename ITEM_TYPE>
		static void destroy(ITEM_TYPE *widgetItem);

		Elm_Object_Item *getElmObjectItem();
		Evas_Object *getParent();
		void setDestroyHandler(NotiHandler handler);

	protected:
		WidgetItem();
		virtual ~WidgetItem();
		static bool prepare(WidgetItem *widgetItem);

	protected:
		Elm_Object_Item *m_pEOItem;

	private:
		void doDestroy();
		void onElmObjectItemDelBase(Evas_Object *obj, void *event_info);

	private:
		NotiHandler m_destroyHandler;
	};

	template <typename ITEM_TYPE, typename... TYPE_ARGS>
	ITEM_TYPE *WidgetItem::create(TYPE_ARGS&&... args)
	{
		ITEM_TYPE *instance = new ITEM_TYPE();

		if (!instance->initialize(std::forward<TYPE_ARGS>(args)...) || !prepare(instance)) {
			ERR("Failed to create WidgetItem instance instance!");
			delete instance;
			return nullptr;
		}

		return instance;
	}

	template <typename ITEM_TYPE>
	void WidgetItem::destroy(ITEM_TYPE *widgetItem)
	{
		if (widgetItem) {
			static_cast<WidgetItem *>(widgetItem)->doDestroy();
		}
	}
}

#endif /* _GUI_WIDGET_ITEM_H_ */
