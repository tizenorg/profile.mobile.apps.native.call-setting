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

#include "Controller/RejectMsgUtils.h"

namespace CallSettings { namespace Controller {

	Model::StringKey convertToStringKey(RejectMsgId id) {
		switch (id) {
		case REJECT_MSG_1:
			return Model::STRING_KEY_REJECT_MSG_ITEM_1;
		case REJECT_MSG_2:
			return Model::STRING_KEY_REJECT_MSG_ITEM_2;
		case REJECT_MSG_3:
			return Model::STRING_KEY_REJECT_MSG_ITEM_3;
		case REJECT_MSG_4:
			return Model::STRING_KEY_REJECT_MSG_ITEM_4;
		case REJECT_MSG_5:
			return Model::STRING_KEY_REJECT_MSG_ITEM_5;
		case REJECT_MSG_6:
			return Model::STRING_KEY_REJECT_MSG_ITEM_6;
		}
	}

	RejectMsgId convertToMsgId(Model::StringKey key) {
		switch (key) {
		case Model::STRING_KEY_REJECT_MSG_ITEM_1:
			return REJECT_MSG_1;
		case Model::STRING_KEY_REJECT_MSG_ITEM_2:
			return REJECT_MSG_2;
		case Model::STRING_KEY_REJECT_MSG_ITEM_3:
			return REJECT_MSG_3;
		case Model::STRING_KEY_REJECT_MSG_ITEM_4:
			return REJECT_MSG_4;
		case Model::STRING_KEY_REJECT_MSG_ITEM_5:
			return REJECT_MSG_5;
		case Model::STRING_KEY_REJECT_MSG_ITEM_6:
			return REJECT_MSG_6;
		}
	}
} }
