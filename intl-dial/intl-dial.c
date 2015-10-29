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

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>
#include <sqlite3.h>
#include <db-util.h>
#include <vconf.h>
#include "cst-debug.h"
#include "intl-dial.h"
#include <sys/inotify.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/version.h>
#include <stdio.h>

#define INTL_DIAL_QUERY_MAX_LEN				1024
#define INTL_DIAL_PATH_FILE					".intl-dial.db"
#define INTL_DIAL_DBDIR						DBDIR
#define INTL_DIAL_PATH_FULL					INTL_DIAL_DBDIR"/"INTL_DIAL_PATH_FILE
#define INTL_DIAL_RO_PATH_FULL				RO_DBDIR"/"INTL_DIAL_PATH_FILE

#define INTL_DIAL_DEFAULT_COUNTRY_PREFIX	"+81"
#define INTL_DIAL_DEFAULT_PREFIX_NAME		"WORLD CALL"
#define INTL_DIAL_DEFAULT_PREFIX_CODE		"009130010"

#define DB_OK                 0
#define DB_ERROR              -1
#define ERR_LEN 		128
#define INOTY_EVENT_MASK   (IN_MODIFY)

static int _db_inoti_fd = -1;
static int wd = 0;

struct noti_node {
	pt2fn_callback_fn cb;
	void *cb_data;
};
typedef struct noti_node noti_node_s;
static GSList *g_notilist;

static gint _db_inoti_comp_with_cb_cbdata(gconstpointer a, gconstpointer b)
{
	ENTER(_db_inoti_comp_with_cb_cbdata);
	int r;

	noti_node_s *key1 = (noti_node_s *) a;
	noti_node_s *key2 = (noti_node_s *) b;

	r = (int)(key1->cb - key2->cb);

	if (r != 0)
		return r;

	r = (int)(key1->cb_data - (key2->cb_data));
	return r;
}

static gboolean _db_gio_cb(GIOChannel *src, GIOCondition cond, gpointer data)
{
	ENTER(_db_gio_cb);
	struct inotify_event ie;
	int fd, r;
	fd = g_io_channel_unix_get_fd(src);
	r = read(fd, &ie, sizeof(ie));
	while (r > 0) {
		if (ie.mask & INOTY_EVENT_MASK) {
			if (g_notilist) {
				struct noti_node *t = NULL;
				GSList *noti_list = NULL;
				noti_list = g_slist_nth(g_notilist, 0);

				while (noti_list) {
					t = noti_list->data;
					if ((t) && (wd == ie.wd))
						t->cb(t->cb_data);

					noti_list = g_slist_next(noti_list);
				}
			}
		}
		if (ie.len > 0)
			(void) lseek(fd, ie.len, SEEK_CUR);

		r = read(fd, &ie, sizeof(ie));
	}
	LEAVE();
	return TRUE;
}

static int _db_noti_init(pt2fn_callback_fn cb, void *data)
{
	ENTER(_db_noti_init);
	GIOChannel *gio;
	int ret = 0;

	if (0 < _db_inoti_fd) {
		ERR("Error: invalid _db_inoti_fd");
		return DB_ERROR;
	}

	_db_inoti_fd = inotify_init();
	if (_db_inoti_fd == -1) {
		char err_buf[100] = { 0, };
		strerror_r(errno, err_buf, sizeof(err_buf));
		ERR("inotify init: %s", err_buf);
		return DB_ERROR;
	}

	ret = fcntl(_db_inoti_fd, F_SETFD, FD_CLOEXEC);
	if (ret < 0) {
		char err_buf[100] = { 0, };
		strerror_r(errno, err_buf, sizeof(err_buf));
		ERR("inotify init: %s", err_buf);
		return DB_ERROR;
	}

	ret = fcntl(_db_inoti_fd, F_SETFL, O_NONBLOCK);
	if (ret < 0) {
		char err_buf[100] = { 0, };
		strerror_r(errno, err_buf, sizeof(err_buf));
		ERR("inotify init: %s", err_buf);
		return DB_ERROR;
	}

	gio = g_io_channel_unix_new(_db_inoti_fd);
	retvm_if(gio == NULL, -1, "Error: create a new GIOChannel");

	g_io_channel_set_flags(gio, G_IO_FLAG_NONBLOCK, NULL);
	g_io_add_watch(gio, G_IO_IN | G_IO_OUT, _db_gio_cb, NULL);
	g_io_channel_unref(gio);
	return DB_OK;
}

EXPORT_API int intl_db_add_notify(pt2fn_callback_fn cb, void *data)
{
	ENTER(intl_db_add_notify);
	struct noti_node t, *n;
	GSList *list = NULL;
	char err_buf[ERR_LEN] = { 0, };
	int func_ret = DB_OK;

	if (_db_inoti_fd < 0) {
		if (_db_noti_init(cb, data))
			return DB_ERROR;
	}

	wd = inotify_add_watch(_db_inoti_fd, INTL_DIAL_PATH_FULL, INOTY_EVENT_MASK);
	if (wd == -1) {
		strerror_r(errno, err_buf, sizeof(err_buf));
		ERR("_db_add_notify : add noti(%s)", err_buf);
		return DB_ERROR;
	}


	t.cb = cb;
	t.cb_data = data;
	list = g_slist_find_custom(g_notilist, &t, _db_inoti_comp_with_cb_cbdata);
	if (list) {
		errno = EALREADY;
		func_ret = DB_ERROR;
		goto out_func;
	}

	n = g_new0(noti_node_s, 1);
	if (n == NULL) {
		ERR("_db_add_notify : g_new0 failed");
		func_ret = DB_ERROR;
		goto out_func;
	}

	n->cb_data = data;
	n->cb = cb;
	g_notilist = g_slist_append(g_notilist, n);
	if (!g_notilist) {
		g_free(n);
		ERR("g_slist_append fail");
	}

out_func:
	return func_ret;
}

EXPORT_API int intl_db_del_notify(void *data, pt2fn_callback_fn cb)
{
	ENTER(intl_db_del_notify);

	struct noti_node *n = NULL;
	struct noti_node t;
	int del = 0;
	GSList *noti_list;
	int r = 0;
	char err_buf[ERR_LEN] = { 0, };
	int func_ret = DB_OK;

	retvm_if(_db_inoti_fd <= 0, DB_ERROR, "Invalid operation: not exist anything for inotify");
	if (wd == -1) {
		strerror_r(errno, err_buf, sizeof(err_buf));
		ERR("Error: inotify_add_watch() [%s]: %s", INTL_DIAL_PATH_FULL, err_buf);
		return DB_ERROR;
	}

	t.cb = cb;
	t.cb_data = data;

	noti_list = g_slist_find_custom(g_notilist, &t, _db_inoti_comp_with_cb_cbdata);

	if (noti_list) {
		del++;
		n = noti_list->data;
		g_notilist = g_slist_remove(g_notilist, n);
		g_free(n);
	}
	noti_list = NULL;

	if (g_slist_length(g_notilist) == 0) {
		r = inotify_rm_watch(_db_inoti_fd, wd);
		if (r == -1) {
			DBG("inotify_rm_watch NOT SUCCESSFUL");
			strerror_r(errno, err_buf, sizeof(err_buf));
			ERR("Error: inotify_rm_watch %s", err_buf);
			func_ret = DB_ERROR;
		} else
			DBG("inotify_rm_watch SUCCESSFUL");

		_db_inoti_fd = -1;
		g_slist_free(g_notilist);
		g_notilist = NULL;
	}

	if (del == 0) {
		ERR("Error: nothing deleted");
		errno = ENOENT;
		func_ret = DB_ERROR;
	}
	return func_ret;
}

static int _xsystem(const char *argv[])
{
        int status = 0;
        pid_t pid;
        pid = fork();
        switch (pid) {
        case -1:
                perror("fork failed");
                return -1;
        case 0:
                /* child */
                execvp(argv[0], (char *const *)argv);
                _exit(-1);
        default:
                /* parent */
                break;
        }
        if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid failed");
                return -1;
        }
        if (WIFSIGNALED(status)) {
                perror("signal");
                return -1;
        }
        if (!WIFEXITED(status)) {
                /* shouldn't happen */
                perror("should not happen");
                return -1;
        }
        return WEXITSTATUS(status);
}

static void __intl_dial_close(sqlite3 *handle)
{
	if (handle == NULL) {
		return ;
	}
	db_util_close(handle);
}

static int __intl_dial_open_readonly(sqlite3 **handle)
{
	int rc = SQLITE_OK;
	if (handle == NULL) {
		ERR("intl_dial_open & close\n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	rc = db_util_open_with_options(INTL_DIAL_PATH_FULL, handle, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READONLY, NULL);
	if (SQLITE_OK != rc) {
		ERR("Failed to open : Error[%s]\n", sqlite3_errmsg(*handle));
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}

	return INTL_DIAL_ERROR_NONE;
}

static int __intl_dial_open_readwrite(sqlite3 **handle)
{
	int rc = SQLITE_OK;

	if (handle == NULL) {
		ERR("intl_dial_open & close\n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	rc = db_util_open_with_options(INTL_DIAL_PATH_FULL, handle, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE, NULL);
	if (SQLITE_OK != rc) {
		/* DB Recovery */
		if (SQLITE_CANTOPEN == rc) {
			/* copy original db from ro to rw */
			rc = db_util_open_with_options(INTL_DIAL_PATH_FULL, handle, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE, NULL);
			if (SQLITE_OK != rc) {
				ERR("Failed to open : Error[%s]\n", sqlite3_errmsg(*handle));
				return INTL_DIAL_ERROR_SQLITE_FAIL;
			}
		} else {
			ERR("Failed to open : Error[%s]\n", sqlite3_errmsg(*handle));
			return INTL_DIAL_ERROR_SQLITE_FAIL;
		}
	}

	return INTL_DIAL_ERROR_NONE;
}

EXPORT_API int intl_dial_insert(const int intl_type, const char *intl_name, const char *intl_prefix)
{
	sqlite3 *handle = NULL;
	int rc = SQLITE_OK;
	char *query = NULL;
	char *errmsg = NULL;

	if (intl_type < INTL_DIAL_TYPE_CODE || intl_type > INTL_DIAL_TYPE_MAX) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (NULL == intl_name || NULL == intl_prefix) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (__intl_dial_open_readwrite(&handle) != INTL_DIAL_ERROR_NONE)
		return INTL_DIAL_ERROR_SQLITE_FAIL;

	query = sqlite3_mprintf("INSERT INTO InternationalDial (intl_type, intl_name, intl_prefix)"
				   " VALUES (%d, '%q', '%q')", intl_type, intl_name, intl_prefix);

	if (!query) {
		__intl_dial_close(handle);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	DBG("query = %s", query);

	rc = sqlite3_exec(handle, query, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK) {
		ERR("SQL error: %d(%s)", rc, errmsg);
		sqlite3_free(errmsg);
		__intl_dial_close(handle);
		sqlite3_free(query);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	sqlite3_free(query);

	__intl_dial_close(handle);

	return INTL_DIAL_ERROR_NONE;
}

EXPORT_API int intl_dial_delete(const int intl_type, const char *intl_name)
{
	sqlite3 *handle = NULL;
	int rc = SQLITE_OK;
	char *query = NULL;
	char *errmsg = NULL;

	if (intl_type < INTL_DIAL_TYPE_CODE || intl_type > INTL_DIAL_TYPE_MAX) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (NULL == intl_name) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (__intl_dial_open_readwrite(&handle) != INTL_DIAL_ERROR_NONE)
		return INTL_DIAL_ERROR_SQLITE_FAIL;

	query = sqlite3_mprintf("DELETE from InternationalDial WHERE intl_type = %d and intl_name = '%q'", intl_type, intl_name);
	if (NULL == query) {
		ERR("Query message create failed \n");
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}

	DBG("query = %s", query);

	rc = sqlite3_exec(handle, query, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK) {
		ERR("SQL error: %d(%s)", rc, errmsg);
		sqlite3_free(errmsg);
		__intl_dial_close(handle);
		sqlite3_free(query);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	sqlite3_free(query);

	__intl_dial_close(handle);

	return INTL_DIAL_ERROR_NONE;
}

EXPORT_API int intl_dial_update(const int intl_type, const char *old_intl_name, const char *intl_name, const char *intl_prefix)
{
	sqlite3 *handle = NULL;
	int rc = SQLITE_OK;
	char *query = NULL;
	char *errmsg = NULL;

	if (intl_type < INTL_DIAL_TYPE_CODE || intl_type > INTL_DIAL_TYPE_MAX) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (NULL == intl_name || NULL == intl_prefix) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (__intl_dial_open_readwrite(&handle) != INTL_DIAL_ERROR_NONE)
		return INTL_DIAL_ERROR_SQLITE_FAIL;

	query = sqlite3_mprintf("UPDATE InternationalDial SET intl_type = %d, intl_name = '%q', intl_prefix = '%q' WHERE intl_name = '%q'", intl_type, intl_name, intl_prefix, old_intl_name);
	if (NULL == query) {
		__intl_dial_close(handle);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}

	DBG("query = %s", query);

	rc = sqlite3_exec(handle, query, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK) {
		ERR("SQL error: %d(%s)", rc, errmsg);
		sqlite3_free(errmsg);
		__intl_dial_close(handle);
		sqlite3_free(query);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	 sqlite3_free(query);

	__intl_dial_close(handle);

	return INTL_DIAL_ERROR_NONE;
}

EXPORT_API int intl_dial_get_list_count(const short intl_type, int *list_count)
{
	sqlite3_stmt *state;
	sqlite3 *handle = NULL;
	int rc = SQLITE_OK;
	int count = 0;
	const char *tail;
	char *query = NULL;

	if (intl_type < INTL_DIAL_TYPE_CODE || intl_type > INTL_DIAL_TYPE_MAX || NULL == list_count) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (__intl_dial_open_readonly(&handle) != INTL_DIAL_ERROR_NONE)
		return INTL_DIAL_ERROR_SQLITE_FAIL;

	query = sqlite3_mprintf("SELECT count(*) FROM InternationalDial where intl_type = %d", intl_type);
	if (!query) {
		__intl_dial_close(handle);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_prepare_v2(handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		ERR("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(handle));
		__intl_dial_close(handle);
		sqlite3_free(query);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	sqlite3_free(query);

	rc = sqlite3_step(state);
	if (SQLITE_ROW == rc) {
		count = sqlite3_column_int(state, 0);
	}

	DBG("count [%d]", count);
	*list_count = count;

	sqlite3_finalize(state);
	__intl_dial_close(handle);

	return INTL_DIAL_ERROR_NONE;
}

EXPORT_API int intl_dial_get_list(const short intl_type, int *list_count, intl_dial_s **intl_dial_list)
{
	sqlite3_stmt *state;
	sqlite3 *handle = NULL;
	int rc = SQLITE_OK;
	int column_index = 0;
	int i = 0;
	int count = 0;
	const char *tail;
	char *query = NULL;

	if (intl_type < INTL_DIAL_TYPE_CODE || intl_type > INTL_DIAL_TYPE_MAX || NULL == list_count || NULL == intl_dial_list) {
		ERR("invalid param \n");
		return INTL_DIAL_ERROR_INVALID_PARAMETER;
	}

	if (__intl_dial_open_readonly(&handle) != INTL_DIAL_ERROR_NONE) {
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}

	query = sqlite3_mprintf("SELECT count(*) FROM InternationalDial where intl_type = %d", intl_type);
	if (!query) {
		__intl_dial_close(handle);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	rc = sqlite3_prepare_v2(handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		ERR("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(handle));
		__intl_dial_close(handle);
		sqlite3_free(query);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	sqlite3_free(query);

	rc = sqlite3_step(state);
	if (SQLITE_ROW == rc) {
		count = sqlite3_column_int(state, 0);
	}

	sqlite3_reset(state);

	query = sqlite3_mprintf("SELECT intl_type, intl_name, intl_prefix FROM InternationalDial where intl_type = %d", intl_type);
	if (!query) {
		__intl_dial_close(handle);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	rc = sqlite3_prepare_v2(handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		ERR("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(handle));
		*list_count = 0;
		sqlite3_free(query);
		return INTL_DIAL_ERROR_SQLITE_FAIL;
	}
	sqlite3_free(query);

	DBG("count [%d]", count);
	if (0 != count) {
		*intl_dial_list = g_try_new0(intl_dial_s, count);
		if (NULL == *intl_dial_list) {
			__intl_dial_close(handle);
			*list_count = 0;
			return INTL_DIAL_ERROR_SQLITE_FAIL;
		}

		for (i = 0; i < count; i++) {
			rc = sqlite3_step(state);
			(*intl_dial_list + i)->intl_type = sqlite3_column_int(state, column_index++);
			g_strlcpy((*intl_dial_list + i)->intl_name, (char *)sqlite3_column_text(state, column_index++), sizeof((*intl_dial_list + i)->intl_name));
			g_strlcpy((*intl_dial_list + i)->intl_prefix, (char *)sqlite3_column_text(state, column_index++), sizeof((*intl_dial_list + i)->intl_prefix));
			column_index = 0;

			if (SQLITE_ROW != rc) {
				break;
			}
		}

		DBG("list_count [%d]", i);
		*list_count = i;
	}
	sqlite3_finalize(state);
	__intl_dial_close(handle);

	return INTL_DIAL_ERROR_NONE;
}

EXPORT_API void intl_dial_free_list(int list_count, intl_dial_s *intl_dial_list)
{
	if (intl_dial_list == NULL) {
		ERR("Invalid Param");
		return;
	}

	g_free(intl_dial_list);
}

EXPORT_API int intl_dial_reset(void)
{
	/* copy original db file from read-only to INTL_DIAL_PATH_DBSPACE */
	char *command[4];
	command[0] = "cp";
	command[1] = INTL_DIAL_RO_PATH_FULL;
	command[2] = INTL_DIAL_PATH_FULL;
	command[3] = NULL;
	DBG("reset command: %s %s %s", command[0], command[1], command[2]);
	if (0 != _xsystem((const char **)command)) {
		ERR("Fatal : Recovery failed !! ");
		return INTL_DIAL_ERROR_INTERNAL;
	}

	return INTL_DIAL_ERROR_NONE;
}
