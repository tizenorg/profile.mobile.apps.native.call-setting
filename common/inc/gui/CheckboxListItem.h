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

#ifndef _GUI_CHECK_ITEM_H_
#define _GUI_CHECK_ITEM_H_

#include "SimpleListItem.h"
#include "Checkbox.h"

namespace gui {
	/*
	 * @brief Represent Genlist item with single text line and checkbox widget
	 */
	class CheckboxListItem : public SimpleListItem {

	public:
		/*
		 * @brief	Check mode. This mode determines behavior of checkbox widget in list item
		 */
		typedef enum {
				NORMAL,			//In this mode checkbox widget click events and list item click events are independent
				AUTO_CHECK,		//In this mode click by list item automatically perform click by checkbox and check event handler will be invoked
				SKIP_EVENTS,	//In this mode checkbox does not handle click events, only list item is interactive
				PENDING,		//In this mode pending circle is shown instead checkbox. This mode used when checkbox handle tooks too much time.
				HIDDEN,			//In this mode checkbox does not shown
		} CheckMode;

		/*
		 * @brief Set new state of checkbox
		 * @param[in]	checkedState	Checkbox new check state which will be setted
		 */
		void setCheckState(bool state);

		/*
		 * @brief Get state of checkbox whether it is checked or not
		 * @return	true if checkbox is checked, otherwise false
		 */
		bool getCheckState();

		/*
		 * @brief Set check mode for list item
		 * @param[in]	mode	New check mode
		 */
		void setCheckMode(CheckMode mode);

		/*
		 * @return Get current check mode
		 */
		CheckMode getCheckMode();

		/*
		 * @brief Set checkbox style
		 * @param[in]	checkStyle		checkbox style
		 */
		void setCheckboxStyle(Checkbox::CheckboxStyle checkStyle);

		/*
		 * @brief Set event handler for checkbox check event.
		 * @param[in]	handler		Check event handler
		 */
		void setCheckHandler(ItemNotiHandler handler);

	protected:
		CheckboxListItem();
		virtual ~CheckboxListItem();

		/*
		 * @brief Callback called when list item will be displayed on screen. List item content will be created here
		 * @param[in]	genlist		Genlist object
		 * @param[in]	part		Part name of list item
		 * @return	Widget which will be placed in mentioned part in list item
		 */
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;

		/*
		 * @brief Callback for checkbox check event
		 */
		void onCheckChanged();

		/*
		 * @brief Callback for checkbox destroy event
		 */
		void onCheckboxDestroyed();

		/*
		 * @brief Callback for list item select event
		 */
		virtual void onSelected() override;

	protected:
		Checkbox *m_pCheckbox;
		CheckMode m_checkMode;
		Checkbox::CheckboxStyle m_checkStyle;
		bool m_checkState;
		ItemNotiHandler m_checkHandler;

	private:
		friend class WidgetItem;
	};
}

#endif /* _GUI_CHECK_OPTION_ITEM_H_ */
