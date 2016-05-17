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
			m_pItemLayout(nullptr),
			m_pEditfield(nullptr),
			m_pContactBtn(nullptr)
	{
	}

	PhoneEditListItem::~PhoneEditListItem()
	{
		if (m_pItemLayout) {
			evas_object_unref(m_pItemLayout->getEvasObject());
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

		return initItemLayout(WidgetWrapper(getParent()));
	}

	bool PhoneEditListItem::initItemLayout(const gui::Widget &parent)
	{
		if (!m_pItemLayout) {
			m_pItemLayout = Widget::create<Layout>(parent, "layout", "call_setting", "phone_edit.list_item");
			RETVM_IF(!m_pItemLayout, false, "Failed to create Item Layout!");

			if (!m_pEditfield) {
				m_pEditfield = createEditFiled();
				RETVM_IF(!m_pEditfield, false, "Failed to create Editfield!");
			}

			if (!m_pContactBtn) {
				m_pContactBtn = createContactButton();
				RETVM_IF(!m_pContactBtn, false, "Failed to create Contact Button!");
			}

			m_pItemLayout->setPartContent("elm.swallow.editfield_ly", m_pEditfield);
			m_pItemLayout->setPartContent("elm.swallow.contact_btn", m_pContactBtn);

			evas_object_ref(m_pItemLayout->getEvasObject());
		}
		return true;
	}

	Editfield *PhoneEditListItem::createEditFiled()
	{
		Editfield *editfield = Widget::create<Editfield>(*m_pItemLayout, Editfield::EF_CLEAR_BTN, "IDS_ST_BODY_PHONE_NUMBER");
		RETVM_IF(!editfield, nullptr, "Failed to create Editfield!");

		editfield->sipTypeSet(Editfield::SIP_TYPE_PHONENUMBER);
		editfield->sipReturnKeyTypeSet(Editfield::SIP_RETURN_KEY_DONE);

		return editfield;
	}

	Button *PhoneEditListItem::createContactButton()
	{
		Button *btn = Widget::create<Button>(*m_pItemLayout, "phone_contact_button");
		RETVM_IF(!btn, nullptr, "Failed to create button!");

		return btn;
	}

	Editfield &PhoneEditListItem::getEditfield()
	{
		return *m_pEditfield;
	}

	Button &PhoneEditListItem::getContactButton()
	{
		return *m_pContactBtn;
	}

	Evas_Object *PhoneEditListItem::getContent(Evas_Object *genlist, const char *part)
	{
		if (!initItemLayout(WidgetWrapper(genlist))) {
			return nullptr;
		}

		m_pItemLayout->show();
		evas_object_smart_calculate(m_pEditfield->getEvasObject());
		return m_pItemLayout->getEvasObject();
	}

	void PhoneEditListItem::onUnrealized()
	{
		if (m_pItemLayout) {
			m_pItemLayout->hide();
		}
	}

}}
