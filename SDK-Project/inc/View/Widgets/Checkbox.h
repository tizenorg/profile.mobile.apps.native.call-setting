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

#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include "View/Widgets/Widget.h"
#include "Utils/Common.h"

namespace Widgets {

	class Checkbox : public Widget {
	public:
		typedef enum {
			CHECKBOX_DEFAULT,
			CHECKBOX_SWITCHER,
			CHECKBOX_STAR,
		} CheckboxType;

		bool isChecked();
		void setChecked(bool checkedState);
		void setCheckHandler(NotifyHandler handler);

	private:
		friend Widget; // to be used in Widget::create

		Checkbox();
		virtual ~Checkbox();
		bool initialize(const Widget &parent, CheckboxType type);
		void onChecked(Evas_Object *obj, void *event_info);

	private:
		NotifyHandler m_checkHandler;

	};
}

#endif /* CHECKBOX_H_ */
