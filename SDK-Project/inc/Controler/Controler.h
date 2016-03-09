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

#ifndef ICONTROLER_H_
#define ICONTROLER_H_

namespace Controler {
	class AppCore;
	class Controler :
		public SystemEventListener,
		public ViewEventListener
	{
	public:
		Controler(const AppCore &core);
		Controler(const AppCore &core, const Controler &parent);
		virtual ~Controler() {};
		virtual void onPause() override;
		virtual void onResume() override;
		virtual void onUpdate(int updateFlag) override;
		virtual void onViewChangeBegin() override;
		virtual void onViewChangeEnd() override;
		bool makeDestroyReqeuest(const Controler &parent);
	};
}

#endif /* ICONTROLER_H_ */
