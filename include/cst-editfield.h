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

#ifndef _CST_EDITFIELD_H_
#define _CST_EDITFIELD_H_

#include <Evas.h>
#include <eina_types.h>

typedef enum
{
	ET_SINGLELINE,
	ET_MULTILINE,
	ET_PASSWORD
} Editfield_Type;

Evas_Object *editfield_create(Evas_Object *parent, Editfield_Type type, const char *guide_text);

void editfield_clear_button_disabled_set(Evas_Object *editfield, Eina_Bool set);

Evas_Object *editfield_get_entry(Evas_Object *editfield);

#endif //_CST_EDITFIELD_H_
