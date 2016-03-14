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

#ifndef BASE_CONTROLLER_H_
#define BASE_CONTROLLER_H_

#include "App/AppCore.h"
#include "App/SystemEventListener.h"
#include "View/ViewManager/ViewEventListener.h"

namespace Controller {

	class BaseController :
		public App::SystemEventListener,
		public View::ViewEventListener
	{
	public:
		BaseController(App::AppCore &core);
		BaseController(App::AppCore &core, BaseController *parent);
		virtual ~BaseController();
		virtual bool createView() = 0;

		virtual void onPause() final;
		virtual void onResume() final;
		virtual void onUpdate(int updateFlag) final;
		virtual void onViewChangeBegin() final;
		virtual void onViewChangeEnd() final;

		/*Function makes request to parent controller to destroy child*/
		void makeDestroyReqeuest();
		/*Function invoke onDestroyRequestCb on child controller to notify him that death is coming*/
		bool destroyChild(BaseController *child);
		/*Callback function called when parent approve child destroy request*/
		virtual void onDestroyRequestCb();

		BaseController() = delete;
		BaseController(const BaseController &obj) = delete;
		BaseController(BaseController &&obj) = delete;

	protected:
		virtual void upadateView(int updateFlag) = 0;

	private:
		typedef enum {
			VIEW_STATE_SHOWN,
			VIEW_STATE_ACTIVATED,
			VIEW_STATE_DEACTIVATED,
			VIEW_STATE_HIDDEN,
		} ViewState;

		App::AppCore &m_Core;
		BaseController *m_pParent;
		View::BaseView *m_pView;
		ViewState m_ViewState;
		bool m_IsUpdateNeeded;
		bool m_IsDestroying;
		int m_UpdateFlag;
	};
}

#endif /* BASE_CONTROLLER_H_ */
