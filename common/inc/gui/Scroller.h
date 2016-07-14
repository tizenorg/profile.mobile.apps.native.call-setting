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

#ifndef _GUI_SCROLLER_H_
#define _GUI_SCROLLER_H_

#include "Widget.h"

namespace gui {

	/*
	 * @brief Scrollbar policy type
	 */
	typedef enum {
		SCROLLBAR_POLICY_AUTO,
		SCROLLBAR_POLICY_ON,
		SCROLLBAR_POLICY_OFF
	} ScrollerPolicy;

	class Scroller : public Widget {
	public:
		/*
		 * @brief Set content into scroller
		 * @param[in]	content		Content which will be placed into scroller
		 */
		void setContent(const Widget &content);

		/*
		 * @brief Set scrollbar dispaly policy
		 * @param[in]	widthScroll			Scrollbar policy for horizontal scroller
		 * @param[in]	heightScroll		Scrollbar policy for vertical scroller
		 */
		void setScrollbarPolicy(ScrollerPolicy widthScroll, ScrollerPolicy heightScroll);

		/*
		 * @brief Show a specific virtual region within the scroller content object.
		 * @param[in]	x		X coordinate of the region
		 * @param[in]	y		Y coordinate of the region
		 * @param[in]	width	Width of the region
		 * @param[in]	height	Height of the region
		 */
		void bringIn(int x, int y, int width, int height);

		/**
		 * @brief Show a specific virtual region within the scroller content object
		 * @param[in]	x		X coordinate of the region
		 * @param[in]	y		Y coordinate of the region
		 * @param[in]	width	Width of the region
		 * @param[in]	height	Height of the region
		 */
		void regionShow(int x, int y, int width, int height);

		/**
		 * @brief Set bouncing behavior
		 * @param[in]	bounceHorizont	Allow bounce horizontally
		 * @param[in]	bounceVertical	Allow bounce vertically
		 *
		 * @note When scrolling, the scroller may "bounce" when reaching an edge of the
		 * content object. This is a visual way to indicate the end has been reached.
		 * This is enabled by default for both axis. This API will set if it is enabled
		 * for the given axis with the boolean parameters for each axis.
		 */
		void setBounceEffect(bool bounceHorizont, bool bounceVertical);

	private:
		friend Widget; // to be used in Widget::create

		Scroller() {};
		virtual ~Scroller() {};

		/**
		 * @brief Initialization of scroller is provided here.
		 * @param[in]	parent		Scroller widget parent instance
		 * @return	true on success, otherwise false
		 */
		bool initialize(const Widget &parent);

		/*
		 * @brief convert scroller policy types
		 */
		static Elm_Scroller_Policy toElmSollerPolicy(ScrollerPolicy policy);

	};
}

#endif /* _GUI_SCROLLER_H_ */
