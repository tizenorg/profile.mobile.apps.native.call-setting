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

#ifndef WIDGET_H_
#define WIDGET_H_

#include <Elementary.h>
#include <utility>

#include "Utils/Common.h"

namespace Widgets {
	class Widget : private NonCopyable {

	public:
		template <typename WIDGET_TYPE, typename... TYPE_ARGS>
		static WIDGET_TYPE *create(TYPE_ARGS&&... args);

		static void destroy(Widget *widget);
		void setDestroyHandler(NotifyHandler handler);
		Evas_Object *getEvasObject() const;

	protected:
		Widget();
		virtual ~Widget();
		virtual void onEvasObjectDel() {};

	protected:
		Evas_Object *m_pEvasObject;

	private:
		static void prepare(Widget *widget);
		void onEvasObjectDelBase(Evas *e, Evas_Object *obj, void *event_info);

	private:
		NotifyHandler m_destroyHandler;
		bool m_isDestroying;
		bool m_isOwner;
	};

	class WidgetWrapper : public Widget {
	public:
		WidgetWrapper(Evas_Object *object) { m_pEvasObject = object;}
		virtual ~WidgetWrapper() {}
	};

	template <typename WIDGET_TYPE, typename... TYPE_ARGS>
	WIDGET_TYPE *Widget::create(TYPE_ARGS&&... args)
	{
		WIDGET_TYPE *instance = new WIDGET_TYPE();
		if (!instance->initialize(std::forward<TYPE_ARGS>(args)...) || !instance->m_pEvasObject) {
			ERR("Failed to create Widget instance!");
			Widget::destroy(instance);
			return nullptr;
		}

		prepare(instance);
		return instance;
	}
}

#endif /* WIDGET_H_ */
