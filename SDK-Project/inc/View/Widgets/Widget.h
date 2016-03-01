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

#ifndef VIEW_WIDGETS_WIDGET_H_
#define VIEW_WIDGETS_WIDGET_H_

#include <Elementary.h>
#include <functional>
#include "Utils/Logger.h"
namespace View { namespace Widgets {
	class Widget {
	public:

		template <typename WIDGET_TYPE, typename... TYPE_ARGS>
		static WIDGET_TYPE *create(Widget *parent, TYPE_ARGS... ARGS);

		static void destroy(Widget *widget);
		bool setDestroyCallback(std::function<void()> callback);
		void unsetDestroyCallback();
		Evas_Object *getEvasObject();

		Widget(const Widget &widget) = delete;
		Widget(Widget &&widget) = delete;

		Widget &operator=(const Widget &widget) = delete;
		Widget &operator=(Widget &&widget) = delete;

	protected:
		Widget();
		virtual ~Widget();
		virtual void onEvasObjectDel() {};

	protected:
		Evas_Object *m_pEvasObject;

	private:
		bool m_isDestroying;
		bool m_isOwner;
		std::function<void()> m_pOnDestroyCb;

	private:
		void onEvasObjectDelBase();
	};

	class WidgetWrapper : public Widget {
	public:
		WidgetWrapper(Evas_Object *object) { m_pEvasObject = object;}
		WidgetWrapper() = delete;
		virtual ~WidgetWrapper() {}
	};

	template <typename WIDGET_TYPE, typename... TYPE_ARGS>
	WIDGET_TYPE* Widget::create(Widget *parent, TYPE_ARGS... ARGS)
	{
		RETVM_IF (!parent || !parent->getEvasObject(), nullptr, "Parent is invalid!");

		WidgetClass *instance = new WidgetClass();
		if (!instance->initialize(parent, ARGS...) || !instance->m_pEvasObject) {
			Widget::destroy(instance);
			return nullptr;
		}

		evas_object_event_callback_add(instance->m_pEvasObject, EVAS_CALLBACK_DEL,
			[](void *data, Evas *e, Evas_Object *obj, void *event_info) {
				return static_cast<Widget*>(data)->onEvasObjectDelBase();
			}, instance);

		return instance;
	}

} }

#endif /* VIEW_WIDGETS_WIDGET_H_ */
