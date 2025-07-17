#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <Windows.h>
#include "uuid4/uuid4.h"
#include "sqlite/sqlite3.h"

// parameters for benchmark
#define DELETE_DATA_TEST 1
#ifndef DELETE_WITH_IN
#define DELETE_WITH_IN 1
#endif
#define USE_TRANSACTION 1
#define cycle 32000

sqlite3 *db;
sqlite3_stmt *stmt;

uuid4_t uuids[cycle];
char zsql[300000];
char uuids_buffer[cycle][256];
char sql[256];

LARGE_INTEGER frequency;
LARGE_INTEGER start_time;
LARGE_INTEGER end_time;

// DO NOT USE THIS PROFILE!
// In SQLite, this callback returns meaningless value!
int trace_callback(unsigned reason, void *data, void *p_stmt, void *nano_seconds) {
  (void)reason;
  (void)data;
  if (stmt == p_stmt) {
    printf("%lld\n", *((long long *)nano_seconds));
  }
  return 0;
}

int main(void) {
  QueryPerformanceFrequency(&frequency);

  sqlite3_open("test.db", &db);

  UUID4_STATE_T state;
  UUID4_T uuid;
  char *msg = (char *)sqlite3_malloc(1024);

  // DO NOT USE THIS PROFILE!
  // sqlite3_trace_v2(db, SQLITE_TRACE_PROFILE, trace_callback, NULL);

  uuid4_seed(&state);

  sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, &msg);
  sqlite3_exec(db, "DROP TABLE test;", NULL, NULL, &msg);

  sqlite3_exec(db, "CREATE TABLE test(id TEXT NOT NULL PRIMARY KEY, val TEXT NOT NULL)", NULL, NULL, &msg);

#if USE_TRANSACTION
  sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &msg);
#endif

  for (int i = 0; i < cycle; i++) {
    uuid4_gen(&state, &uuid);
    uuids[i] = uuid;
    uuid4_to_s(uuid, uuids_buffer[i], sizeof(uuids_buffer[i]));
    sprintf_s(sql, sizeof(sql), "INSERT INTO test VALUES('%s', '%d');", uuids_buffer[i], i);
    sqlite3_exec(db, sql, NULL, NULL, &msg);
  }

#if USE_TRANSACTION
  sqlite3_exec(db, "COMMIT;", NULL, NULL, &msg);
#endif

#if DELETE_DATA_TEST
#if DELETE_WITH_IN
  char *params = "?,";

  memcpy(zsql, "DELETE FROM test WHERE id IN (", sizeof(zsql));
  for (int i = 0; i < cycle; i++) {
    memcpy(zsql + 30 + 2 * i, params, 2);
  }
  memcpy(zsql + 30 + 2 * cycle - 1, ");", 2);

  QueryPerformanceCounter(&start_time);

  sqlite3_prepare_v2(db, zsql, (int)strlen(zsql), &stmt, NULL);

#if USE_TRANSACTION
  sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &msg);
#endif

  for (int i = 0; i < cycle; i++) {
    // When binding parameters, the buffer should be cleaned before sqlite_step()
    sqlite3_bind_text(stmt, i + 1, uuids_buffer[i], (int)strlen(uuids_buffer[i]), NULL);
  }

  sqlite3_step(stmt);

#if USE_TRANSACTION
  sqlite3_exec(db, "COMMIT;", NULL, NULL, &msg);
#endif

  QueryPerformanceCounter(&end_time);

#else
  memcpy(zsql, "DELETE FROM test WHERE id = ?;", 30);

  QueryPerformanceCounter(&start_time);

  sqlite3_prepare_v2(db, "DELETE FROM test WHERE id = ?;", (int)strlen(sql), &stmt, NULL);

#if USE_TRANSACTION
  sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &msg);
#endif

  for (int i = 0; i < cycle; i++) {
    sqlite3_bind_text(stmt, 1, uuids_buffer[i], (int)strlen(uuids_buffer[i]), NULL);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }

#if USE_TRANSACTION
  sqlite3_exec(db, "COMMIT;", NULL, NULL, &msg);
#endif

  QueryPerformanceCounter(&end_time);

#endif
  sqlite3_finalize(stmt);
#endif
  sqlite3_free(msg);
  sqlite3_close(db);

  printf("Spend %.1lfus\n", (double)(end_time.QuadPart - start_time.QuadPart) / (double)frequency.QuadPart * 1e6);
  return 0;
}
