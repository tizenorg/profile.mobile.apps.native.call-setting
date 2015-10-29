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

#ifndef INTL_DIAL_H_
#define INTL_DIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int		intl_type;
	char	intl_name[64];
	char 	intl_prefix[22];
} intl_dial_s;

typedef enum {
	INTL_DIAL_TYPE_CODE = 1,
	INTL_DIAL_TYPE_PREFIX = 2,
	INTL_DIAL_TYPE_MAX,
}intl_dial_type_e;;

typedef void (*pt2fn_callback_fn)(void *data);

#if 0  /*don't use yet but we will someday....*/
typedef enum {
	INTL_DIAL_COLUMN_INTL_TYPE = 0,
	INTL_DIAL_COLUMN_INTL_NAME,
	INTL_DIAL_COLUMN_INTL_CODE,
	INTL_DIAL_COLUMN_MAX,
}intl_dial_intl_column_e;
#endif

typedef enum {
	INTL_DIAL_ERROR_NONE = 0,
	INTL_DIAL_ERROR_SQLITE_FAIL = -100,
	INTL_DIAL_ERROR_INVALID_PARAMETER = INTL_DIAL_ERROR_SQLITE_FAIL | 0x01,
	INTL_DIAL_ERROR_INTERNAL = INTL_DIAL_ERROR_SQLITE_FAIL | 0x02,
	INTL_DIAL_ERROR_UNKNOWN = INTL_DIAL_ERROR_SQLITE_FAIL | 0x03,
} intl_dial_error_e;


int intl_dial_insert(const int intl_type, const char *intl_name, const char *intl_prefix);
int intl_dial_delete(const int intl_type, const char *intl_name);
int intl_dial_update(const int intl_type, const char *old_intl_name, const char *intl_name, const char *intl_prefix);
int intl_db_add_notify(pt2fn_callback_fn cb, void *data);
int intl_db_del_notify(void *data, pt2fn_callback_fn cb);

// GET
int intl_dial_get_list_count(const short intl_type, int *list_count);
int intl_dial_get_list(const short intl_type, int *list_count, intl_dial_s **intl_dial);
void intl_dial_free_list(int list_count, intl_dial_s *intl_dial_list);

int intl_dial_reset(void);

#ifdef __cplusplus
}
#endif
#endif				/* INTL_DIAL_H_ */
