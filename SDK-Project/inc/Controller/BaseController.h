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

namespace Controller {
	class AppCore;
	class BaseController :
		public SystemEventListener,
		public ViewEventListener
	{
	public:
		BaseController(const AppCore &core);
		BaseController(const AppCore &core, const BaseController *m_pParent);
		virtual ~BaseController() {};
		/*Function makes request to parent controller to destroy child*/
		void makeDestroyReqeuest();
		BaseController() = delete;
		BaseController(const BaseController &obj) = delete;
		BaseController(BaseController &&obj) = delete;
		virtual void onPause() override;
		virtual void onResume() override;
		virtual void onUpdate(int updateFlag) override;
		virtual void onViewChangeBegin() override;
		virtual void onViewChangeEnd() override;
		/*Function invoke onDestroyRequestCb on child controller to notify him that death is coming*/
		bool destroyChild(BaseController *child);
		/*Callback function called when parent approve child destroy request*/
		virtual void onDestroyRequestCb();
	private:
		const AppCore &m_Core;
		const BaseController *m_pParent;
	};
}

#endif /* BASE_CONTROLLER_H_ */
