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

#include <app_i18n.h>

#include "View/MainView/RejectMsgOption.h"

namespace MainView {

	Elm_Genlist_Item_Class *RejectMsgOption::getItemClass()
	{
		static Elm_Genlist_Item_Class itc = createItemClass("multiline");
		return &itc;
	}

	char *RejectMsgOption::getText(const char *part)
	{
		if (strcmp(part, "elm.text") == 0) {
			return strdup(_("IDS_CST_HEADER_CALL_REJECT_MESSAGES_ABB"));
		} else if (strcmp(part, "elm.text.multiline") == 0) {
			return strdup(_("IDS_CST_SBODY_COMPOSE_OR_EDIT_RESPONSE_MESSAGES_TO_SEND_WHEN_REJECTING_INCOMING_CALLS"));
		}

		return nullptr;

	}

}
