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

#ifndef _GUI_VIEW_CONTROLLER_H_
#define _GUI_VIEW_CONTROLLER_H_

#include "GuiTypes.h"
#include "appfw/BaseApplication.h"

namespace gui {

	/*
	 * @brief Represents base class of controller Naviframe page view
	 * All View Controllers are inherited from ViewController
	 */
	class ViewController : private util::NonCopyable {
	public:
		/*
		 * @brief	Fabric method for creation View controller of type CONTROLLER_T
		 * @param[in]	app			Application core instance
		 * @param[in]	handler		Controller destroy handler. It is invoked when controller is destroying to notify parent controller
		 * @param[in]	args		Variadic arguments for controller instance
		 * @return	Pointer to new controller instance on success, otherwise false.
		 */
		template <typename CONTROLLER_T, typename APP_T, typename ...TYPE_ARGS>
		static CONTROLLER_T *create(APP_T &app, NotiHandler handler, TYPE_ARGS&&... args);

		virtual ~ViewController();

		/*
		 * @brief Add custom flags which will be passed to updateView function.
		 * Thus each controller can add custom update flag which will be passed when controller update will be invoked.
		 * @param[in]	flags	custom update flags
		 * @see updateView()
		 */
		void addCustomUpdateFlags(int flags);
		/*
		 * @brief Remove custom update flags which was added.
		 * @param[in]	flags	cusotm falgs
		 */
		void removeCustomUpdateFlags(int flags);

		/*
		 * @brief Check whether update flag is set
		 * @param[in]	flags	flag to be checked
		 * @return true if falg is set, otherwise false
		 */
		bool isUpdateFlagsSet(int flags) const;

	protected:
		/*
		 * @brief Update flags. This flags are determine events which took place so that controller can provide needed update when updateView() is called
		 */
		typedef enum {
			UF_INITIAL = 1,
			UF_LANGUAGE = 2,
			UF_REGION_FORMAT = 4,
			UF_ORIENTATION = 8,
			UF_WAS_PAUSED = 16,
			UF_IS_POPPING = 32,

			//Keep in sync with base flags
			UF_CUSTOM_SHIFT = 6,
			UF_BASE_FLAGS = ((1 << UF_CUSTOM_SHIFT) - 1)
		} UpdateFlag;

		ViewController (appfw::BaseApplication &app, NotiHandler destroyHandler);
		/*
		 * @brief Called when controller initiate the process of destroy. Calling this method notify parent for deleting current controller
		 * Then parent controller can destroy it's child controller and destructor of current controller will be invoked
		 */
		void makeDestroyReqeuest();

		/*
		 * @brief Callback which is invoked when View becomes visible.
		 * When this callback is called view is most commonly in transition state
		 */
		virtual void onShow() {}

		/*
		 * @brief Callback which is invoked when View becomes invisible.
		 */
		virtual void onHide() {}

		/*
		 * @brief Callback which is invoked when View needs update. This can be by different reason.
		 * Update invokes only when view became visible.
		 */
		virtual void updateView(int updateFlag) {}

		/*
		 * @brief Callback which s invoked when View is ready for user interaction.
		 * It is invoked when View finished transition and is ready for work
		 */
		virtual void onActivate() {}

		/*
		 * @brief Callback which s invoked when View becomes not ready for interaction but it is still visible.
		 * It is invoked when current view became popping so that view is visible but not active any more.
		 */
		virtual void onDeactivate() {}

		/*
		 * @brief Callback for more menu hardware key press
		 */
		virtual void onMenuKeyPressed() {}

		/*
		 * @brief Callback for back hardware key press
		 */
		virtual void onBackKeyPressed();

		/*
		 * @brief Initialize View controller
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief Set view to belongs to Controller
		 * @param[in]	view	View instance which will be attached to controller
		 */
		void setBaseView(BaseView *view);

	protected:
		appfw::BaseApplication &m_Core;
		NotiHandler m_destroyRequestHandler;
		bool m_isActivated;
		bool m_isVisible;
		bool m_isDestroying;
		int m_updateFlag;

	private:
		void onSystemEvent(appfw::SystemEvent event);
		void onViewEvent(ViewEvent event);

		void handleViewChangeBegin();
		void handleViewChangeEnd();
		void handleMenuKeyPress();
		void handleBackKeyPress();
		void handlePause();
		void handleResume();

		void doUpdate();
		void doActivate();
		void doDeactivate();
		void doShow();
		void doHide();

	private:
		BaseView *m_pView;

	};

	template <typename CONTROLLER_T, typename APP_T, typename ...TYPE_ARGS>
	CONTROLLER_T *ViewController::create(APP_T &app, NotiHandler handler, TYPE_ARGS&&... args)
	{
		CONTROLLER_T *instance = new CONTROLLER_T(app, handler);
		if (!instance->initialize(std::forward<TYPE_ARGS>(args)...) && !instance->m_pView) {
			ERR("Failed to create controller instance!");
			delete instance;
			return nullptr;
		}

		return instance;
	}
}

#endif /* _GUI_VIEW_CONTROLLER_H_ */
