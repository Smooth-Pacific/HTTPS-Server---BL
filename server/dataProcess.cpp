#include <iostream>
#include <vector>
#include <string>
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
#include <utility>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include "dataGen.h"
#include <chrono>
#include "safeq.h"
#include <thread>
#include <sqlite3.h> 


using namespace std;

namespace dataProcess {

	int IBCount_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
		typedef unordered_map<string, int> table_type;
		table_type* table = static_cast<table_type*>(data);
		string str = argv[1];
		if (str.find("Insufficient Balance")) {
			(*table)[argv[0]]++;
		} else {
			(*table)[argv[0]] += 0;
		}
		return 0;
	}

	int fraudCountYear_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
		typedef unordered_map<string, pair<long, long> > table_type;
		table_type* table = static_cast<table_type*>(data);
		string str = argv[1];
		if (str == "Yes") {
			(*table)[argv[0]].first++;
		} 
		(*table)[argv[0]].second++;
		return 0;
	}

	int map_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
		typedef map<string, string> table_type;
		table_type* table = static_cast<table_type*>(data);
		(*table)[argv[0]] = argv[1];
		return 0;
	}


	void atLeastOneInsufficientBalance() {
		unordered_map<string, int> records;

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


	   /* Get user, error column */
	   sql = "SELECT User, `Errors?` from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), IBCount_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   sqlite3_close(db);

	   long count = 0;
	   for (auto itr = records.begin(); itr != records.end(); itr++) {
	   		if (itr->second > 0) {
	   			count++;
	   		}
		}

		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "Insufficient_Balance", NULL);
	    double perc = count/records.size();
	    xmlNewChild(node, NULL, BAD_CAST "percent", BAD_CAST to_string(perc).c_str());
		xmlSaveFormatFileEnc("files/atLeastOneInsufficientBalance.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
	}

	void RepeatedInsufficientBalance() {
		unordered_map<string, int> records;

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


	   /* Get user, error column */
	   sql = "SELECT User, `Errors?` from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), IBCount_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   sqlite3_close(db);

	   long count = 0;
	   for (auto itr = records.begin(); itr != records.end(); itr++) {
	   		if (itr->second > 1) {
	   			count++;
	   		}
		}

		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "RepeatedInsufficientBalance", NULL);
	    double perc = count/records.size();
	    xmlNewChild(node, NULL, BAD_CAST "percent", BAD_CAST to_string(perc).c_str());
		xmlSaveFormatFileEnc("files/RepeatedInsufficientBalance.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
	}

	void uniqueMerchant() {
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


	   /* Get merchant name column */
	   sql = "SELECT `Merchant Name` from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), dataGen::set_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   sqlite3_close(db);


		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "uniqueMerchant", NULL);
	    xmlNewChild(node, NULL, BAD_CAST "TotalAmount", BAD_CAST to_string(records.size()).c_str());
		xmlSaveFormatFileEnc("files/uniqueMerchant.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
	}

	void percentageFraud() {
		unordered_map<string, pair<long, long>> records;

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

	   /* Get year, fraud column */
	   sql = "SELECT Year, `Is Fraud?` from transactions";

	   rc = sqlite3_exec(db, sql.c_str(), fraudCountYear_callback, &records, &zErrMsg);
	   
	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   sqlite3_close(db);

	 
		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "percentageFraudByYear", NULL);

	    for (auto it = records.begin(); it != records.end(); it++) {
	    	double fraud = it->second.first * 1.0;
	    	long tot = it->second.second;
	    	double perc = fraud / tot;
	    	xmlNodePtr yearnode = xmlNewChild(node, NULL, BAD_CAST "Year", BAD_CAST to_string(perc).c_str());
	    	xmlNewProp(yearnode, BAD_CAST "y", BAD_CAST it->first.c_str());
		}

		xmlSaveFormatFileEnc("files/percentageFraudByYear.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
	}


}

int main() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	dataProcess::percentageFraud();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;
	return 0;
}
