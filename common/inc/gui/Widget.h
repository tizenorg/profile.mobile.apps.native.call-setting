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

#ifndef _GUI_WIDGET_H_
#define _GUI_WIDGET_H_

#include "GuiTypes.h"

namespace gui {

	/**
	 * @brief Represents Evas_Object
	 * @details Ties the lifetime of this object to the underlying Evas_Object.
	 * @note Should always be allocated with operator "new".
	 */
	class Widget : private util::NonCopyable {

	public:
		/*
		 * @brief Fabric method for creation widget.
		 * @param[in]	args	Variadic arguments which describes widget parameters
		 * @return Widget instance pointer on success, otherwise nullptr
		 */
		template <typename WIDGET_TYPE, typename... TYPE_ARGS>
		static WIDGET_TYPE *create(TYPE_ARGS&&... args);

		/*
		 * @brief	Destroy widget object
		 * @param[in]	widget	Widget to be destroyed
		 */
		static void destroy(Widget *widget);

		/*
		 * @brief Set destroy handler. Handler will be invoked when wiget object be destroyed
		 * @param[in]	handler		Destroy handler
		 */
		void setDestroyHandler(NotiHandler handler);

		/**
		 * @return Underlying Evas_Object.
		 */
		Evas_Object *getEvasObject() const;

		/*
		 * @brief Set tag data into widget object
		 * @param[in]	tag		Tag data
		 */
		void setTag(int tag);

		/*
		 * @return tag data which was set to widget object
		 */
		int getTag() const;

		/*
		 * @brief Set underlying Evas_Object into disabled state
		 * @param[in]	disabledState	disabled state
		 */
		void setDisabled(bool disabledState);

		/*
		 * @brief Set focuse to widget object
		 * @param[in]	focused		If true focus will be set to object, otherwise focus will be removed
		 */
		void setFocus(bool focused);

		/*
		 * @brief Set whether widget is allowed to grab focuse by click events
		 * @param[in]	focusAllowed	if true focus allowed, otherwise not
		 */
		void setFocusAllowed(bool focusAllowed);

		/*
		 * @brief Make widget object visible
		 */
		void show();

		/*
		 * @brief Make widget object invisible
		 */
		void hide();

		/*
		 * @brief Set text to default text part of widget object
		 * @param[in]	text	Text to be set
		 */
		void setText(const util::TString &text);

		/*
		 * @brief Set text to specific part of widget object
		 * @param[in]	partName	Part name
		 * @param[in]	text	Text to be set
		 */
		void setPartText(const char *partName, const util::TString &text);

		/*
		 * @brief Set content to default content part of widget object
		 * @param[in]	pContent	Content instance to be set
		 */
		void setContent(Widget *pContent);

		/*
		 * @brief Set content to specific part of widget object
		 * @param[in]	partName	Part name
		 * @param[in]	pContent	Content instance to be set
		 */
		void setPartContent(const char *partName, Widget *pContent);

	protected:
		Widget();
		virtual ~Widget();

		/*
		 * @brief Evas_Object delete callback. It is invoked when underlying evas object of widget marked for destroy.
		 */
		virtual void onEvasObjectDel() {};

	protected:
		Evas_Object *m_pEvasObject;

	private:
		/*
		 * @brief Setup delete callback for underlying Evas_Object to be able to handle life cycle of Evas_Object and free resources
		 */
		static bool prepare(Widget *widget);

		/*
		 * @brief Callback which will be invoked on widget child objects to notify about destroy
		 */
		void onEvasObjectDelBase(Evas *e, Evas_Object *obj, void *event_info);

	private:
		NotiHandler m_destroyHandler;
		int m_tag;
	};

	/*
	 * @brief Represents Evas_OBject wrapper.
	 * This object is not an owner of Evas_Object but it provides ability to use raw Evas_Object as Widget
	 * Destroy of WidgetWrapper does not modify underlying Evas_Object
	 */
	class WidgetWrapper : public Widget {
	public:
		WidgetWrapper(Evas_Object *object) { m_pEvasObject = object;}
		virtual ~WidgetWrapper() {}
	};

	template <typename WIDGET_TYPE, typename... TYPE_ARGS>
	WIDGET_TYPE *Widget::create(TYPE_ARGS&&... args)
	{
		WIDGET_TYPE *instance = new WIDGET_TYPE();
		if (!instance->initialize(std::forward<TYPE_ARGS>(args)...) || !prepare(instance)) {
			ERR("Failed to create Widget instance!");
			Widget::destroy(instance);
			return nullptr;
		}

		return instance;
	}
}

#endif /* _GUI_WIDGET_H_ */
