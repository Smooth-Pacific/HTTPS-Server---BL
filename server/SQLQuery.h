#pragma once

#include <sqlite3.h> 
#include <iostream> 
#include <functional>

class SQLQuery {
private:
	sqlite3 *db;
public:
	SQLQuery(const char* dbfile): db(nullptr) {
		open(dbfile);
	}
	~SQLQuery() {
		sqlite3_close(db);
	}

	/* Open database */
	void open(const char* dbfile) {
		int rc = sqlite3_open(dbfile, &db);
   
   		if( rc ) {
      		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
      		return;
   		} else {
      		std::cout << "Opened database successfully\n";
   		}
	}

	template<typename T>
	void exec(std::string sql, int (*callback)(void*, int, char**, char**), T& records) {
		char *zErrMsg = 0;
		int rc = sqlite3_exec(db, sql.c_str(), callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      std::cerr << "Sql error: " << zErrMsg << "\n";
	      sqlite3_free(zErrMsg);
	   } else {
	      std::cout << "Operation done successfully\n";
	   }
	}

};
