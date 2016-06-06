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

#ifndef APPCONFIG_H_
#define APPCONFIG_H_

#ifdef	UI_BASE_SCALE
#undef	UI_BASE_SCALE
#endif
#define	UI_BASE_SCALE 2.6

#ifdef	LOG_TAG
#undef	LOG_TAG
#endif
#define	LOG_TAG "CALL-SETTING"

#ifdef	APP_EDJE_THEME_PATH
#undef	APP_EDJE_THEME_PATH
#endif
#define	APP_EDJE_THEME_PATH "edje/call-setting-theme.edj"

#endif /* APPCONFIG_H_ */
