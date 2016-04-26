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

#ifndef _UTIL_LOGGER_H_
#define _UTIL_LOGGER_H_

#include <dlog.h>

#define _LOG_OUT(level, fmt, ...) dlog_print(level, LOG_TAG, "%s; %s(%d); " fmt, \
		strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define INF(fmt, arg...) _LOG_OUT(DLOG_INFO, fmt, ##arg)
#define DBG(fmt, arg...) _LOG_OUT(DLOG_DEBUG, fmt, ##arg)
#define WARN(fmt, arg...) _LOG_OUT(DLOG_WARN, fmt, ##arg)
#define ERR(fmt, arg...) _LOG_OUT(DLOG_ERROR, fmt, ##arg)

#define RETM_IF(expr, fmt, arg...) \
{ \
	if (expr) { \
		ERR(fmt, ##arg); \
		return; \
	} \
}

#define RETVM_IF(expr, val, fmt, arg...) \
{ \
	if (expr) { \
		ERR(fmt, ##arg); \
		return (val); \
	} \
}

#endif /* _UTIL_LOGGER_H_ */
