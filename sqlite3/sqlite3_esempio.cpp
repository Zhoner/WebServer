#include "sqlite3.h"
#include <stdio.h>

//http://zetcode.com/db/sqlitec/

//schema: create delle tabelle
/*
int main(int argc, char* argv[]) {

  sqlite3 *db; //connessione al db
  sqlite3_stmt *res; //statement

  int rc = sqlite3_open(":memory:", &db); //return code, lo apre in ram, poi db per referenza(indirizzo del puntatore)

  if (rc != SQLITE_OK) { //se ha dato errori

    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); //stampa l'errore, sulla console
    sqlite3_close(db);

    return 1;
  }

  rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0); //passo il db, preparo la query, flag, l'indirizzo del risultato

  if (rc != SQLITE_OK) { //errore

    fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);

    return 1;
  }

  rc = sqlite3_step(res);

  if (rc == SQLITE_ROW) {
    printf("%s\n", sqlite3_column_text(res, 0));
  }

  sqlite3_finalize(res);
  sqlite3_close(db);

  return 0;
}
*/
/*********************************************************/



#include <stdio.h>

int callback(void *, int, char **, char **);

int main(void) {

  sqlite3 *db;
  char *err_msg = 0;

  int rc = sqlite3_open("mydb.db", &db);

  if (rc != SQLITE_OK) {

    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);

    return 1;
  }

  char *sql = "SELECT * FROM Personaggi"; //query

  rc = sqlite3_exec(db, sql, callback, 0, &err_msg); //gli passo la funzione di callback

  if (rc != SQLITE_OK ) {

    fprintf(stderr, "Failed to select data\n");
    fprintf(stderr, "SQL error: %s\n", err_msg);

    sqlite3_free(err_msg);
    sqlite3_close(db);

    return 1;
  }

  sqlite3_close(db);

  return 0;
}


/*
 * funzione di callback, restituisce un intero che è la return dell'exec, la usiamo per leggere il db
 * Notused -> non è usato
 * argc -> numero argomenti, numero delle colonne
 * argv -> argomenti, ci sarà la riga del DB
 * azColName -> nomi delle colonne
 */
int callback(void *NotUsed, int argc, char **argv, char **azColName) {

  NotUsed = 0;

  for (int i = 0; i < argc; i++) { //scorre la riga

    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  printf("\n");

  return 0;
}
