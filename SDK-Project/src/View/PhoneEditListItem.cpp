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

#include "appfw/Path.h"
#include "gui/Widgets/Layout.h"

#include "View/PhoneEditListItem.h"

namespace CallSettings { namespace View {

	using namespace gui;

	PhoneEditListItem::PhoneEditListItem() :
			m_pEditfield(nullptr)
	{
	}

	PhoneEditListItem::~PhoneEditListItem()
	{
		if (m_pEditfield) {
			evas_object_unref(m_pEditfield->getEvasObject());
		}
	}

	Elm_Genlist_Item_Class *PhoneEditListItem::getItemClass()
	{
		static Elm_Genlist_Item_Class itc = createItemClass("full");
		return &itc;
	}

	bool PhoneEditListItem::initialize(ItemAddMethod createItem)
	{
		if (!GenlistItem::initialize(createItem, GENLIST_ITEM_SELECT_MODE_NONE)) {
			return false;
		}

		return initEditFiled(WidgetWrapper(getParent()));
	}

	bool PhoneEditListItem::initEditFiled(const gui::Widget &parent)
	{
		if (!m_pEditfield) {
			m_pEditfield = Widget::create<Editfield>(parent, Editfield::EF_CLEAR_BTN, "IDS_ST_BODY_PHONE_NUMBER");
			RETVM_IF(!m_pEditfield, false, "Failed to create Editfield!");
			evas_object_ref(m_pEditfield->getEvasObject());

			m_pEditfield->sipTypeSet(Editfield::SIP_TYPE_PHONENUMBER);
			m_pEditfield->sipReturnKeyTypeSet(Editfield::SIP_RETURN_KEY_DONE);

			m_pEditfield->hide();
		}
		return true;
	}

	Button *PhoneEditListItem::createContactButton(Widget &parent)
	{
		Button *btn = Widget::create<Button>(parent, "transparent");
		RETVM_IF(!btn, nullptr, "Failed to create button!");

		Layout *icon = Widget::create<Layout>(*btn,
				appfw::getResourcePath(APP_EDJE_THEME_PATH).c_str(), "CONTACT_ICON");
		if (!icon) {
			ERR("Failed to create icon!");
			Widget::destroy(btn);
			return nullptr;
		}

		btn->setContent(icon);

		return btn;
	}

	Editfield &PhoneEditListItem::getEditfield()
	{
		return *m_pEditfield;
	}

	Evas_Object *PhoneEditListItem::getContent(Evas_Object *genlist, const char *part)
	{
		if (!initEditFiled(WidgetWrapper(genlist))) {
			return nullptr;
		}

		m_pEditfield->show();

		elm_object_focus_set(m_pEditfield->getEvasObject(), EINA_TRUE);

		return m_pEditfield->getEvasObject();
	}

	void PhoneEditListItem::onUnrealized()
	{
		if (m_pEditfield) {
			m_pEditfield->hide();
		}
	}

}}
