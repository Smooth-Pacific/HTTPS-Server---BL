#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <tuple>
#include "dasmig/namegen.hpp"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include "dataGen.h"
#include <chrono>
#include "safeq.h"
#include <thread>
#include <sqlite3.h> 


using ng = dasmig::ng;
using namespace std;

namespace dataGen {
	// Split line by delimiter
	vector<string> split (const string &s, const char delim) {
	    vector<string> result;
	    bool quoteFlag = false;  // Checks to escape delim inside quotes
	    string element = "";
	    for (auto& ch: s) {
	    	if (ch == '"') {
	    		quoteFlag = !quoteFlag;
	    	} else if ( ch == delim && !quoteFlag) {
	    		result.push_back(element);
	    		element = "";
	    	} else {
	    		element += ch;
	    	}
	    }
	    result.push_back(element);
	    return result;
	}

	// Function to escape and quote strings
	string quoteAndEsc(const string& word) {
		std::string esc;
		const char * src = word.c_str();
		while (char c = *src++) {
			if (c == '\'') {
			  esc += "\\";
			}
			esc += c;
		}
		return "'" + esc + "'";
	}

	string quote(const string& word) {
		return "'" + word + "'";
	}

	int set_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
		typedef unordered_set<string> table_type;
		table_type* table = static_cast<table_type*>(data);
		table->insert(argv[0]);
		return 0;
	}

	int map_set_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
		typedef map<string, set<string> > table_type;
		table_type* table = static_cast<table_type*>(data);
		(*table)[argv[0]].insert(argv[1]);
		return 0;
	}

	int map_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
		typedef map<string, string> table_type;
		table_type* table = static_cast<table_type*>(data);
		(*table)[argv[0]] = argv[1];
		return 0;
	}


	int initializeDB() {
		sqlite3 *db;
	   	int rc;

	   	rc = sqlite3_open("transaction.db", &db);

	   	if( rc ) {
	   		cerr << "Can't open database: " << sqlite3_errmsg(db) << '\n';
	      	return(0);
	   	} else {
	   		cout << "Opened database successfully\n";
	   	}
	   	sqlite3_close(db);
	   	return 0;
	}


	int createTable() {
		sqlite3 *db;
		char *zErrMsg = 0;
		int rc;
		string sql;

		/* Open database */
		rc = sqlite3_open("transaction.db", &db);

		if( rc ) {
			cerr << "Can't open database: " << sqlite3_errmsg(db) << '\n';
			return(0);
		} else {
			cout << "Opened database successfully\n";
		}

		ifstream file("files/card_transaction.csv");
		if (!file.is_open()) {
			cerr << "File not opened\n";
		}

		string line;
		getline(file, line);
		vector<string> headers = split(line, ',');
		file.close();

		/* Create SQL statement */
		sql = "CREATE TABLE transactions (";
		for (auto& str: headers) {
			string quoted = quoteAndEsc(str);
			sql += quoted + " TEXT,";
		}
		sql.pop_back();
		sql += ");";

		/* Execute SQL statement */
		rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

		if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		} else {
			fprintf(stdout, "Table created successfully\n");
		}
		sqlite3_close(db);
  	 	return 0;
	}

	void readth(threadsafe_queue<string>& data_queue, ifstream& file) {
		string line;
		unsigned long counter = 0;
		// Read rows
		while(getline(file, line)) {
			data_queue.push(line);

			// If queue gets too big 
			if (data_queue.size() > 50000) {
				sleep(1);
			}
			// Batch size per insert
			if (counter == 800000) {
				break;
			}
			counter++;
		}
		// Indicator for each thread to stop
		string end = "END";
		data_queue.push(end);
		data_queue.push(end);
	}

	void processth(threadsafe_queue<string>& data_queue, string& sql) {
		while (true) {
	        string line;
	        if (data_queue.try_pop(line)) {
	        	if (line == "END") {
	        		break;
	        	}
	        	vector<string> row = split(line, ',');
	        	string temp = " (";
				for (auto& str: row) {
					string quoted = quoteAndEsc(str);
					temp += quoted + ",";
				}
				temp.pop_back();
				temp += "),";
				sql += temp;
	        }
	    }
	}

	int insertVals() {
		sqlite3 *db;
		char *zErrMsg = 0;
		int rc;

		/* Open database */
		rc = sqlite3_open("transaction.db", &db);
	   
	   	if( rc ) {
	    	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	    	return(0);
	   } else {
	    	fprintf(stderr, "Opened database successfully\n");
	   }

	   ifstream file("files/card_transaction.csv");
		if (!file.is_open()) {
			cerr << "File not opened\n";
			return 0;
		}

		string line;
		getline(file, line);
		vector<string> headers = split(line, ',');

   		/* Create SQL statement */
   		string insertStr;
   		insertStr = "INSERT INTO transactions (";
   		for (auto& str: headers) {
			string quoted = quoteAndEsc(str);
			insertStr += quoted + ",";
		}
		insertStr.pop_back();
		insertStr += ") VALUES";


		// Insert in batch to avoid reading whole file in at once
		for (int i = 0; i < 30; i++) {
			string sql = insertStr;
			threadsafe_queue<string> data_queue;
			thread t1(readth, ref(data_queue), ref(file));

			string query1;
			string query2;

			thread t2(processth, ref(data_queue), ref(query1));
			thread t3(processth, ref(data_queue), ref(query2));

			t1.join();
			t2.join();
			t3.join();

			sql += query1 + query2;
			sql.pop_back();
			sql += ";";

			cout << "batch " << i << endl;

			/* Execute SQL statement */
			rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
	   
		   if( rc != SQLITE_OK ){
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
		   } else {
		    	fprintf(stdout, "Records created successfully\n");
		   }
		   
		}

		// Final batch for leftovers
		string sql = insertStr;

		while(getline(file, line)) {
			vector<string> row = split(line, ',');
			string temp = " (";
			for (auto& str: row) {
				string quoted = quoteAndEsc(str);
				temp += quoted + ",";
			}
			temp.pop_back();
			temp += "),";
			sql += temp;
		}
		sql.pop_back();
		sql += ";";


	   rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
	   } else {
	    	fprintf(stdout, "Records created successfully\n");
	   }
	   sqlite3_close(db);
	   return 0;
	}


	void gen_user() {
		unordered_set<string> records;

		sqlite3 *db;
   		char *zErrMsg = 0;
   		int rc;
   		string sql;

   		/* Open database */
   		rc = sqlite3_open("transaction.db", &db);
   
   		if( rc ) {
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		return;
   		} else {
      		fprintf(stderr, "Opened database successfully\n");
   		}

   		/* Create user table */
		sql = "CREATE TABLE users (User, firstname, lastname, email );";

		rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   string userq = "INSERT INTO users ( User, firstname, lastname, email ) VALUES ";



	   /* Get user column */
	   sql = "SELECT User from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), set_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }


		unordered_set<string> unique_names;

		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "users", NULL);

	    for (auto itr = records.begin(); itr != records.end(); itr++) {
			string fn, ln;
			while (true) {
				wstring first = ng::instance().get_name(ng::gender::any, ng::culture::american);
				transform(first.begin(), first.end(), back_inserter(fn), [] (wchar_t c) { return (char)c; } );
				wstring last = ng::instance().get_name(ng::gender::any, ng::culture::american);
				transform(last.begin(), last.end(), back_inserter(ln), [] (wchar_t c) { return (char)c; } );
				if (unique_names.find(fn + " " + ln) == unique_names.end()) {
					unique_names.insert(fn + " " + ln);
					break;
				}
			}
	    	xmlNodePtr node1 = xmlNewChild(node, NULL, BAD_CAST "user", NULL);
	    	xmlNewProp(node1, BAD_CAST "id", BAD_CAST (*itr).c_str());
	    	xmlNewChild(node1, NULL, BAD_CAST "firstName", BAD_CAST fn.c_str());
	    	xmlNewChild(node1, NULL, BAD_CAST "lastName", BAD_CAST ln.c_str());
	    	string em = fn + "." + ln + "@smoothceeplusplus.com";
	    	xmlNewChild(node1, NULL, BAD_CAST "email", BAD_CAST em.c_str());

	    	userq += " (" + quote((*itr)) + "," + quote(fn) + "," + quote(ln) + "," + quote(em) + "),";
		}
		xmlSaveFormatFileEnc("files/user.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();

	    /* Create table for users */
	    userq.pop_back();
		userq += ";";
		rc = sqlite3_exec(db, userq.c_str(), 0, 0, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

	    sqlite3_close(db);

	}

	bool luhn_alg(string cnum) {
		int nDigits = cnum.length();

	    int nSum = 0, isSecond = false;
	    for (int i = nDigits - 1; i >= 0; i--) {
	        int d = cnum[i] - '0';
	        if (isSecond == true) {
	            d = d * 2;
	        }
	        nSum += d / 10;
	        nSum += d % 10;
	        isSecond = !isSecond;
	    }
	    return (nSum % 10 == 0);
	}

	string credit_card_gen(unordered_set<string>& unique_cards) {
		string card_num;
		while (true) {
			card_num = "";
			for (int i = 0; i < 16; i++) {
				card_num += to_string(rand() % 10);
			}
			if (luhn_alg(card_num)) {
				if (unique_cards.find(card_num) == unique_cards.end()) {
					unique_cards.insert(card_num);
					break;
				}
			}
		}
		return card_num;
	}


	void gen_cards() {
		map<string, set<string>> records;

		sqlite3 *db;
   		char *zErrMsg = 0;
   		int rc;
   		string sql;

   		/* Open database */
   		rc = sqlite3_open("transaction.db", &db);
   
   		if( rc ) {
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		return;
   		} else {
      		fprintf(stderr, "Opened database successfully\n");
   		}

   		/* Create cards table */
		sql = "CREATE TABLE cards (User, Card, cardNum);";

		rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   string cardq = "INSERT INTO cards ( User, Card, cardNum ) VALUES ";



	   /* Get user, card column */
	   sql = "SELECT User, Card from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), map_set_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

		unordered_set<string> unique_cards;
		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "users", NULL);

	    for (auto it = records.begin(); it != records.end(); it++) {
	    	xmlNodePtr node1 = xmlNewChild(node, NULL, BAD_CAST "user", NULL);
	    	xmlNewProp(node1, BAD_CAST "id", BAD_CAST it->first.c_str());
	    	xmlNodePtr node2 = xmlNewChild(node1, NULL, BAD_CAST "cards", NULL);

			set<string> st = it->second;
			for (auto itr = st.begin(); itr != st.end(); itr++) {
				string credit_card = credit_card_gen(unique_cards);
				xmlNodePtr card = xmlNewChild(node2, NULL, BAD_CAST "card", BAD_CAST credit_card.c_str());
				xmlNewProp(card, BAD_CAST "id", BAD_CAST (*itr).c_str());

				cardq += " (" + quote(it->first) + "," + quote((*itr)) + "," + quote(credit_card) + "),";
			}
		}

		xmlSaveFormatFileEnc("files/credit_cards.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();

	    cardq.pop_back();
		cardq += ";";
		rc = sqlite3_exec(db, cardq.c_str(), 0, 0, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

	    sqlite3_close(db);
	}

	vector<string> getMercFiles(string filename) {
		ifstream namesfile(filename);
		if (!namesfile.is_open()) {
			throw std::runtime_error("No such file");
		}

		string name;
		vector<string> nvec;
		while (getline(namesfile, name)) {
			nvec.push_back(name);
		}
		namesfile.close();
		return nvec;
	}


	void gen_merchants() {
		map<string, string> records;

		sqlite3 *db;
   		char *zErrMsg = 0;
   		int rc;
   		string sql;

   		/* Open database */
   		rc = sqlite3_open("transaction.db", &db);
   
   		if( rc ) {
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		return;
   		} else {
      		fprintf(stderr, "Opened database successfully\n");
   		}

   		/* Create merchants table */
		sql = "CREATE TABLE merchants (mid, mname, MCC);";

		rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   string merchq = "INSERT INTO merchants ( mid, mname, MCC) VALUES ";



	   /* Get user, card column */
	   sql = "SELECT `Merchant Name`, MCC from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), map_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

		vector<string> mNames = getMercFiles("./resources/merchants/merchant.names");

		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "merchants", NULL);

	    for (auto it = records.begin(); it != records.end(); it++) {
	    	xmlNodePtr node1 = xmlNewChild(node, NULL, BAD_CAST "merchant", NULL);
	    	xmlNewProp(node1, BAD_CAST "id", BAD_CAST it->first.c_str());
	    	string mname = mNames[rand() % mNames.size()];
	    	xmlNewChild(node1, NULL, BAD_CAST "merchantName", BAD_CAST mname.c_str());
	    	xmlNewChild(node1, NULL, BAD_CAST "mcc", BAD_CAST it->second.c_str());

	    	merchq += " (" + quote(it->first) + "," + quote(mname) + "," + quote(it->second) + "),";
		}
		xmlSaveFormatFileEnc("files/merchants.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();

	    merchq.pop_back();
		merchq += ";";
		rc = sqlite3_exec(db, merchq.c_str(), 0, 0, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

	    sqlite3_close(db);
	}

	void gen_states() {
		map<string, set<string> > records;

		sqlite3 *db;
   		char *zErrMsg = 0;
   		int rc;
   		string sql;

   		/* Open database */
   		rc = sqlite3_open("transaction.db", &db);
   
   		if( rc ) {
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		return;
   		} else {
      		fprintf(stderr, "Opened database successfully\n");
   		}

   		/* Create cards table */
		sql = "CREATE TABLE states (sid, state, capital, zipcode);";

		rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   string stateq = "INSERT INTO states ( sid, state, capital, zipcode ) VALUES ";



	   /* Get user, card column */
	   sql = "SELECT `Merchant State`, Zip from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), map_set_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

		

		vector<vector<string>> states;
		vector<string> row;
		string line, word;
		ifstream file2("files/states.csv");
		if (!file2.is_open()) {
			throw std::runtime_error("No such file");
		}
		getline(file2, line);
		while(getline(file2, line)) {
			row.clear();
			stringstream ss(line);
			while(getline(ss, word, ',')) {
				row.push_back(word);
			}
			states.push_back(row);
		}
		file2.close();

		
		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "states", NULL);

	    for (unsigned long i = 0; i < states.size(); i++) {
	    	set<string> st = records[states[i][2]];
	    	xmlNodePtr stateNode = xmlNewChild(node, NULL, BAD_CAST "state", NULL);
	    	xmlNewProp(stateNode, BAD_CAST "id", BAD_CAST states[i][2].c_str());
	    	xmlNewChild(stateNode, NULL, BAD_CAST "name", BAD_CAST states[i][1].c_str());
	    	xmlNewChild(stateNode, NULL, BAD_CAST "capital", BAD_CAST states[i][3].c_str());
	    	xmlNodePtr zipNode = xmlNewChild(stateNode, NULL, BAD_CAST "zipcodes", NULL);
	        for (auto itr = st.begin(); itr != st.end(); itr++) {
	        	xmlNewChild(zipNode, NULL, BAD_CAST "zipcode", BAD_CAST (*itr).c_str());

	        	stateq += " (" + quote(states[i][2]) + "," + quote(states[i][1]) + "," + quote(states[i][3]) + "," + quote((*itr)) + "),";
	        }
	    }
	    xmlSaveFormatFileEnc("files/states.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();

	    stateq.pop_back();
		stateq += ";";
		rc = sqlite3_exec(db, stateq.c_str(), 0, 0, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }

	    sqlite3_close(db);
	}

}
