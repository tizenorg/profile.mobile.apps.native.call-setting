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

#include "gui/Widgets/GenlistItem.h"
#include "gui/Widgets/Editfield.h"
#include "gui/Widgets/Button.h"

namespace CallSettings { namespace View {

	class PhoneEditListItem : public gui::GenlistItem {
	public:
		gui::Editfield &getEditfield();

		// GenlistItem //

		virtual Elm_Genlist_Item_Class *getItemClass() override;

	private:
		friend class WidgetItem;
		PhoneEditListItem();
		virtual ~PhoneEditListItem();

		bool initialize(ItemAddMethod createItem);

		bool initEditFiled(const gui::Widget &parent);
		gui::Button *createContactButton(gui::Widget &parent);

		// GenlistItem //

		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;
		virtual void onUnrealized() override;

	protected:
		gui::Editfield *m_pEditfield;
	};

}}

#endif /* _GUI_EDITFIELD_LIST_ITEMS_H_ */
