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

namespace View { namespace Widgets {
	class Widget {
	public:
		Widget();
		virtual ~Widget();
		bool create(Evas_Object *parent);
		Evas_Object *getEvasObject() const;
		static Widget *getWidget(Evas_Object *object);
		void show();
		void hide();

	protected:
		/*Evas_Object is created here. Function returns EvasOject pointer or Null in case of Fail*/
		virtual Evas_Object *onCreate(Evas_Object *parent) = 0;
		/*Called when Evas_Object is created. Here can be added up callbacks for Widget, setting focus, etc*/
		virtual void onCreated() {}
		/*Called when Evas_Object del callback is invoked*/
		virtual void onDestroy() {}
		/*Called when Evas_Object free callback is invoked*/
		virtual void onDestroed() {}
	private:
		Evas_Object *m_pEvasObject;

		/*Setup del and free callbacks here and add Widget pointer to Evas_Object data*/
		void setEvasObject(Evas_Object *object);
		/*Remove del and free callbacks here,  remove Widget pointer from Evas_Object data*/
		Evas_Object *resetEvasObject();
		void destroyWidget();
		static void onFreeEvasObject(void *data, Evas *e, Evas_Object *obj, void *event_info);
		static void onDelEvasObject(void *data, Evas *e, Evas_Object *obj, void *event_info);

		Widget(const Widget &that) = delete;
		Widget(Widget &&that) = delete;
		Widget & operator=(const Widget &that) = delete;
		Widget & operator=(Widget &&that) = delete;
	};
} }

#endif /* WIDGET_H_ */
