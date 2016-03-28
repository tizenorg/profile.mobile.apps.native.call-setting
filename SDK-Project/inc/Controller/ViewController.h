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

#ifndef VIEW_CONTROLLER_H_
#define VIEW_CONTROLLER_H_

#include "App/AppCore.h"
#include "Utils/Common.h"

namespace Controller {
	using namespace App;
	using namespace View;

	class ViewController;
	typedef Delegate<void(ViewController *)> DestroyRequestHandler;

	class ViewController : private NonCopyable {
	public:
		template <class ViewController>
		static ViewController *create(App::AppCore &core, DestroyRequestHandler handler);
		virtual ~ViewController();
		void makeDestroyReqeuest();

	protected:
		typedef enum {
			UPDATE_INITIAL = 1,
			UPDATE_LANGUAGE = 2,
			UPDATE_REGION_FORMAT = 4,
			UPDATE_ORIENTATION = 8,
			UPDATE_WAS_PAUSED = 16,
		} UpdateFlag;

		ViewController (AppCore &core, DestroyRequestHandler handler);
		bool initialize();
		void finalize();
		virtual bool onInitizlize() { return true;}
		virtual BaseView *createView() = 0;
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void onUpdate(int updateFlag) {}
		virtual void onActivate() {}
		virtual void onDeactivate() {}
		virtual void onMenuKeyPressed() {}
		virtual void onBackKeyPressed() {}

	protected:
		App::AppCore &m_Core;
		DestroyRequestHandler m_destroyRequestHandler;

	private:
		bool m_isActivated;
		bool m_isVisible;
		bool m_isDestroying;
		int m_updateFlag;
		View::BaseView *m_pView;

	private:
		void onSystemEvent(SystemEvent event);
		void onViewEvent(ViewEvent event);

		void handleViewChangeBegin();
		void handleViewChangeEnd();
		void handleMenuKeyPress();
		void handleBackKeyPress();

		void doPause();
		void doResume();
		void doUpdate();
		void doActivate();
		void doDeactivate();
		void doShow();
		void doHide();
	};

	template <class CONTROLLER_TYPE>
	CONTROLLER_TYPE *ViewController::create(AppCore &core, DestroyRequestHandler handler)
	{
		CONTROLLER_TYPE *instance = new CONTROLLER_TYPE(core, handler);
		if (!instance->initialize()) {
			ERR("Failed to create controller instance!");
			delete instance;
			return nullptr;
		}

		return instance;
	}
}

#endif /* VIEW_CONTROLLER_H_ */
