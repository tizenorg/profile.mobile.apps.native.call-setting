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

#ifndef _GUI_CHECKBOX_H_
#define _GUI_CHECKBOX_H_

#include "Widget.h"

namespace gui {
	/*
	 * @brief Represent Checkbox widget
	 * Has different styles: default square checkbox, star icon, on/off toggle button
	 */
	class Checkbox : public Widget {
	public:

		/*
		 * @brief Checkbox widget type
		 */
		typedef enum {
			CHECKBOX_DEFAULT,
			CHECKBOX_SWITCHER,
			CHECKBOX_STAR,
		} CheckboxStyle;

		/*
		 * @brief Get state of checkbox whether it is checked or not
		 * @return	true if checkbox is checked, otherwise false
		 */
		bool isChecked();

		/*
		 * @brief Set new state of checkbox
		 * @param[in]	checkedState	Checkbox new check state which will be set
		 */
		void setChecked(bool checkedState);

		/*
		 * @brief Set event handler for checkbox check event. This event will be invoked when widget was checked/unchecked by user.
		 * Handler will not be invoked in case of change check state by setChecked() method
		 * @param[in]	handler		Check event handler
		 */
		void setCheckHandler(NotiHandler handler);

		/*
		 * @brief Set checkbox type
		 * @param[in]	style		checkbox style
		 */
		void setCheckStyle(CheckboxStyle style);

		/*
		 * @brief calling this method allow/deny user interaction with checkbox
		 * @param[in]	skipEvents	if true events will be blocked so checkbox can not be checked by user, if false checkbox became interactive
		 */
		void setSkipEvents(bool skipEvents);

	private:
		friend Widget; // to be used in Widget::create

		Checkbox();
		virtual ~Checkbox();

		/**
		 * @brief Initialization of checkbox widget is provided here.
		 * @param[in]	parent			Checkbox widget parent instance
		 * @param[in]	type			Checkbox type
		 * @param[in]	skipEvents		Checkbox interactivity state, @see setSkipEvents()
		 * @return	true on success, otherwise false
		 */
		bool initialize(const Widget &parent, CheckboxStyle type = CHECKBOX_DEFAULT, bool skipEvents = false);

		/*
		 * @brief Checkbox check callback
		 * @param[in]	obj			Checkbox evas object entity
		 * @param[in]	event_info	Click callback event info data.
		 */
		void onChecked(Evas_Object *obj, void *event_info);

	private:
		NotiHandler m_checkHandler;

	};
}

#endif /* _GUI_CHECKBOX_H_ */
