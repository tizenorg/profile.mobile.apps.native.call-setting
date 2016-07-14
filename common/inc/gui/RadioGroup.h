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

#ifndef _GUI_RADIO_GROUP_H_
#define _GUI_RADIO_GROUP_H_

#include "Genlist.h"

namespace gui {

	/**
	 * @brief RadioGroup click callback
	 * @param[in] itemIndex	itemIndex of radio button which was clicked
	 */
	typedef util::Delegate <void(int itemIndex)> RadioSelectHandler;

	/*
	 * @brief Representation of Radiogroup widget. This widget contains of genlist with RadioOtionItem elements
	 */
	class RadioGroup : public Widget {
	public:
		/*
		 * @brief	Set specific radio button in group selected
		 * @param[in]	itemIndex	index of radio button to be selected
		 */
		void selectRadioItem(int itemIndex);

		/*
		 * @brief	Set radio button select callback
		 * @param[in]	handler	radio button select event handler
		 */
		void setSelectedCallback(RadioSelectHandler handler) {m_radioSelectHandler = handler;}

		/*
		 * @brief Add new radio option item into list
		 * @param[in]	text	Option item text
		 * @param[in]	itemIndex	Option item index
		 * @return	true on success, otherwise false
		 */
		bool addItem(util::TString text, int itemIndex);

	protected:
		RadioGroup();
		~RadioGroup() {};

		/**
		 * @brief Initialization of RadioGroup widget
		 * @param[in]	parent		Button widget parent instance
		 * @return	true on success, otherwise false
		 */
		bool initialize(const Widget &parent);

		/*
		 * @brief Creation of genlist for RadioGroup widget
		 * @return	true on success, otherwise false
		 */
		bool createRadioList();

		/*
		 * @brief RadioItem select callback
		 * @param[in]	item	List item which dispatch select event
		 */
		void onRadioItemSelected(gui::WidgetItem &item);

	private:
		friend class Widget;

		Genlist *m_pGenlist;
		Evas_Object *m_pRadioGroup;
		RadioSelectHandler m_radioSelectHandler;
	};

}
#endif /* _GUI_RADIO_GROUP_H_ */
