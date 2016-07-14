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

#ifndef _GUI_EDITFIELD_LIST_ITEMS_H_
#define _GUI_EDITFIELD_LIST_ITEMS_H_

#include "gui/GenlistItem.h"
#include "gui/Layout.h"
#include "gui/Editfield.h"
#include "gui/Button.h"

namespace CallSettings { namespace View {

	/*
	 * @brief Represents Special genlist item widget for input phone number
	 * It contains from editfield and contact button
	 */
	class PhoneEditListItem : public gui::GenlistItem {
	public:
		/*
		 * @return Editfield widget instance from List item
		 */
		gui::Editfield &getEditfield();

		/*
		 * @return Contact button widget instance from List item
		 */
		gui::Button &getContactButton();

		// GenlistItem //

		virtual Elm_Genlist_Item_Class *getItemClass() override;

	private:
		friend class WidgetItem;
		PhoneEditListItem();
		virtual ~PhoneEditListItem();

		bool initialize(ItemAddMethod createItem);

		/*
		 * @brief Initialize List item layout
		 * @param[in]	parent	Widget parent
		 * @return true on success, otherwise false
		 */
		bool initItemLayout(const gui::Widget &parent);

		/*
		 * @brief Create editfield widget for phone number input with special type of SIP
		 * @return Editfield instance
		 */
		gui::Editfield *createEditFiled();

		/*
		 * @brief Create Contacts button with special style
		 * @return Contact button instance
		 */
		gui::Button *createContactButton();

		/*
		 * @brief Callback for item layout destroy
		 */
		void onItemLayoutDel();

		// GenlistItem //

		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;

	protected:
		gui::Layout *m_pItemLayout;
		gui::Editfield *m_pEditfield;
		gui::Button *m_pContactBtn;
	};

}}

#endif /* _GUI_EDITFIELD_LIST_ITEMS_H_ */
