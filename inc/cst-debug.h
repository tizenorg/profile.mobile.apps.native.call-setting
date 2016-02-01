/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <dlog.h>
#include <stdio.h>
#ifndef CST_LOG_TAG
#define CST_LOG_TAG "CST"
#endif

#define ERR(fmt, arg...) \
	do { \
		LOG(LOG_ERROR, CST_LOG_TAG, fmt, ##arg); \
	} while(0)

#define WARN(fmt, arg...) \
			do { \
				LOG(LOG_WARN, CST_LOG_TAG, fmt, ##arg); \
			} while(0)

#define DBG(fmt, arg...) \
	do { \
		LOG(LOG_DEBUG, CST_LOG_TAG, fmt, ##arg); \
	} while(0)

#define ENTER(arg...) \
	do {\
		LOG(LOG_DEBUG, CST_LOG_TAG, "Enter func=%p", ##arg);\
	}while(0)

#define LEAVE() \
	do {\
		LOG(LOG_DEBUG, CST_LOG_TAG, "Leave func");\
	} while(0)

#define SEC_DBG(fmt, arg...) \
	do { \
		SECURE_LOGD(CST_LOG_TAG, fmt, ##arg); \
	} while(0)

#define SEC_ERR(fmt, arg...) \
	do { \
		SECURE_LOGE(CST_LOG_TAG, fmt, ##arg); \
	} while(0)

#define ret_if(expr) do { \
		if(expr) { \
			ERR("(%s) return", #expr); \
			return; \
		} \
	} while (0)

#define retv_if(expr, val) do { \
		if(expr) { \
			ERR("(%s) return", #expr); \
			return (val); \
		} \
	} while (0)

#define retm_if(expr, fmt, arg...) do { \
		if(expr) { \
			ERR(fmt, ##arg); \
			ERR("(%s) return", #expr); \
			return; \
		} \
	} while (0)

#define retvm_if(expr, val, fmt, arg...) do { \
		if(expr) { \
			ERR(fmt, ##arg); \
			ERR("(%s) return", #expr); \
			return (val); \
		} \
	} while (0)
