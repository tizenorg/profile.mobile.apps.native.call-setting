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

#ifndef _GUI_BASE_VIEW_H_
#define _GUI_BASE_VIEW_H_

#include "GuiTypes.h"
#include "Layout.h"
#include "NaviItem.h"

namespace gui {

	/**
	 * @brief Represents content of a Naviframe page
	 */
	class BaseView {
	public:
		/*
		 * @brief Fabric method for creation view instance
		 * @param[in]	naviItem	Naviframe item instance which will be container for created view
		 * @return		Pointer for newly created view instance on success, otherwise nullptr
		 */
		template <class ViewClass>
		static ViewClass *create(NaviItem *naviItem);

		/*
		 * @brief Static method for destroy view instance
		 * @param[in]	view	View instance pointer to be destroyed
		 */
		static void destroy(BaseView *view);

		/*
		 * @brief Setup subscription to be notified when View destroy happened
		 * @param[in]	handler		View destroy handler which will be invoked in process of View destroy
		 */
		void setDestroyHandler(ViewDestroyHandler handler);

		/*
		 * @brief Calling this method allows user interaction with View.
		 * Most commonly uses after interaction was blocked by disableInputEvents() method
		 * By default View allows user interaction after creation.
		 * @see disableInputEvents()
		 */
		void enableInputEvents();

		/*
		 * @brief Calling this method disable user interaction with View.
		 * All input event for GUI elements which belongs to View will be ignored.
		 * @see disableInputEvents()
		 */
		void disableInputEvents();

		/*
		 * @return	instance of root layout which is placed as content into naviframe page
		 */
		const Widget &getContent() const;

		/*
		 * @return	instance of NaviframeItem which belongs to current View instance
		 */
		NaviItem &getItem();

	protected:
		BaseView(NaviItem *naviItem);
		virtual ~BaseView();

		/*
		 * @brief	Initialize View. Creation of root view layout and setting it as NaviframePage content is provided here.
		 * @return true on success, otherwise false.
		 */
		bool initialize();

		/*
		 * @brief	Set content into View root layout which will be displayed in Naviframe page.
		 * @param[in]	content		Instance of View content widget
		 * @return true on success, otherwise false.
		 */
		bool setViewContent(Widget &content);

	protected:
		NaviItem *m_pNaviItem;
		Layout *m_pViewLayout;
		ViewDestroyHandler m_destroyHandler;
		bool m_isDestroying;

	private:
		/*
		 * @brief Handler for View root layout destroying. Invokes when view content is destroying for some reason.
		 * In this handler initiates a process of View instance destroy.
		 */
		void onViewLayoutDestroy();
		/*
		 * @brief Handler for View naviframe item destroying. Usually calls when naviframe page is popped.
		 * In this handler initiates a process of View instance destroy.
		 */
		void onNaviItemDestroy();
	};

	template <class ViewClass>
	ViewClass *BaseView::create(NaviItem *naviItem)
	{
		RETVM_IF(!naviItem, nullptr, "Invalid arguments!");

		ViewClass *viewInstance = new ViewClass(naviItem);

		if (!viewInstance->initialize()) {
			ERR("Failed to create View instance");
			delete viewInstance;
			return nullptr;
		}

		return viewInstance;
	}
}

#endif /* _GUI_BASE_VIEW_H_ */
