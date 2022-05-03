#include <iostream>
#include <fstream>
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
#include <math.h>
#include <algorithm>
#include <tuple>
#include <utility>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include "dataGen.h"
#include "dataProcess.h"
#include "SQLQuery.h"
#include <chrono>
#include "safeq.h"
#include <thread>
#include <sqlite3.h> 
#include <execution>


using namespace std;

int dataProcess::IBCount_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
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

int dataProcess::fraudCountYear_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef unordered_map<string, pair<long, long> > table_type;
	table_type* table = static_cast<table_type*>(data);
	string str = argv[1];
	if (str == "Yes") {
		(*table)[argv[0]].first++;
	} 
	(*table)[argv[0]].second++;
	return 0;
}

int dataProcess::fraudCountState_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef unordered_map<string, long> table_type;
	table_type* table = static_cast<table_type*>(data);
	string str = argv[1];
	if (str == "No") {
		(*table)[argv[0]]++;
	} else {
		(*table)[argv[0]] += 0;
	} 
	return 0;
}

int dataProcess::singleCount_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef unordered_map<string, long> table_type;
	table_type* table = static_cast<table_type*>(data);
	(*table)[argv[0]]++;
	return 0;
}

int dataProcess::bottom_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef unordered_map<string, long> table_type;
	table_type* table = static_cast<table_type*>(data);
	string online = argv[1];
	if (online == " ONLINE") {
		(*table)[argv[0]]++;
	}
	return 0;
}

int dataProcess::lateNight_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef unordered_map<string, long> table_type;
	table_type* table = static_cast<table_type*>(data);
	string time = argv[0];
	int hour = stoi(time.substr(0, 2));
	string amount = argv[1];
	double amt = stod(amount.substr(1));
	string online = argv[2];
	if ((online == " ONLINE") && (amt > 100.00) && (hour >= 20 || hour < 5)) {
		(*table)[argv[3]]++;
	}
	return 0;
}

int dataProcess::amount_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef vector<double> table_type;
	table_type* table = static_cast<table_type*>(data);
	string s_amt = argv[0];
	double amt = stod(s_amt.substr(1));
	table->push_back(amt);
	return 0;
}

int dataProcess::noErrors_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName) {
	typedef unordered_map<string, long> table_type;
	table_type* table = static_cast<table_type*>(data);
	string error = argv[1];
	if (error == "Insufficient Balance,") {
		(*table)[argv[0]]++;
	}
	return 0;
}


void dataProcess::atLeastOneInsufficientBalance(const char* dbfile) {
	unordered_map<string, int> records;

	SQLQuery lateNight(dbfile);
	string sql = "SELECT User, `Errors?` from transactions";
	lateNight.exec(sql, IBCount_callback, records);

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

void dataProcess::RepeatedInsufficientBalance(const char* dbfile) {
	unordered_map<string, int> records;

	SQLQuery lateNight(dbfile);
	string sql = "SELECT User, `Errors?` from transactions";
	lateNight.exec(sql, IBCount_callback, records);

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

void dataProcess::uniqueMerchant(const char* dbfile) {
	unordered_set<string> records;

	SQLQuery lateNight(dbfile);
	string sql = "SELECT `Merchant Name` from transactions";
	lateNight.exec(sql, dataGen::set_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "uniqueMerchant", NULL);
    xmlNewChild(node, NULL, BAD_CAST "TotalAmount", BAD_CAST to_string(records.size()).c_str());
	xmlSaveFormatFileEnc("files/uniqueMerchant.xml", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void dataProcess::percentageFraud(const char* dbfile) {
	unordered_map<string, pair<long, long>> records;

	SQLQuery lateNight(dbfile);
	string sql = "SELECT Year, `Is Fraud?` from transactions";
	lateNight.exec(sql, fraudCountYear_callback, records);
 
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

void dataProcess::recurringTrans(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery query(dbfile);
	string sql = "SELECT `Merchant Name` from transactions";
	query.exec(sql, singleCount_callback, records);

   // Transfer to vector to sort by value
   vector<pair<string, long>> map_vec;
   for (auto& it : records) {
    	map_vec.push_back(it);
	}

	sort(std::execution::par, map_vec.begin(), map_vec.end(), [](pair<string, long> a, pair<string, long> b) {
		return a.second > b.second;
	});

 
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "recurringTrans5", NULL);

    for (int i = 0; i < 5; i++) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "Top5", BAD_CAST map_vec[i].first.c_str());
    	xmlNewProp(rtnode, BAD_CAST "count", BAD_CAST to_string(map_vec[i].second).c_str());
	}

	xmlSaveFormatFileEnc("files/recurringTrans5.xml", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void dataProcess::zipTrans5(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery query(dbfile);
	string sql = "SELECT Zip from transactions";
	query.exec(sql, singleCount_callback, records);

   // Transfer to vector to sort by value
   vector<pair<string, long>> map_vec;
   for (auto& it : records) {
    	map_vec.push_back(it);
	}

	sort(std::execution::par, map_vec.begin(), map_vec.end(), [](pair<string, long> a, pair<string, long> b) {
		return a.second > b.second;
	});

 
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "zipcode5", NULL);

    // Offset to account for online
    for (int i = 1; i < 6; i++) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "Top5", BAD_CAST map_vec[i].first.c_str());
    	xmlNewProp(rtnode, BAD_CAST "count", BAD_CAST to_string(map_vec[i].second).c_str());
	}

	xmlSaveFormatFileEnc("files/zipcode5.xml", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void dataProcess::cityTrans5(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery query(dbfile);
	string sql = "SELECT `Merchant City` from transactions";
	query.exec(sql, singleCount_callback, records);

   // Transfer to vector to sort by value
   vector<pair<string, long>> map_vec;
   for (auto& it : records) {
    	map_vec.push_back(it);
	}

	sort(std::execution::par, map_vec.begin(), map_vec.end(), [](pair<string, long> a, pair<string, long> b) {
		return a.second > b.second;
	});

 
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "city5", NULL);

    // Offset to account for online 
    for (int i = 1; i < 6; i++) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "Top5", BAD_CAST map_vec[i].first.c_str());
    	xmlNewProp(rtnode, BAD_CAST "count", BAD_CAST to_string(map_vec[i].second).c_str());
	}

	xmlSaveFormatFileEnc("files/city5.xml", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void dataProcess::transactionTypes(const char* dbfile) {
	unordered_set<string> records;

	SQLQuery query(dbfile);
	string sql = "SELECT `Use Chip` from transactions";
	query.exec(sql, dataGen::set_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "transactionTypes", NULL);

   for (auto& itr: records) {
   	xmlNewChild(node, NULL, BAD_CAST "type", BAD_CAST itr.c_str());
	}

	xmlSaveFormatFileEnc("files/transactionTypes.xml", doc, "UTF-8", 1);
   xmlFreeDoc(doc);
   xmlCleanupParser();
}

void dataProcess::top10Transactions(const char* dbfile) {
	vector<double> records;

	SQLQuery query(dbfile);
	string sql = "SELECT Amount from transactions";
	query.exec(sql, amount_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
   xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
   xmlDocSetRootElement(doc, root_node);
   xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "top10Transactions", NULL);

   sort(std::execution::par, records.begin(), records.end(), [](double a, double b) {
		return a > b;
	});

   for (int i = 0; i < 10; i++) {
    	string amt = to_string(records[i]);
    	amt.erase(amt.end() - 4, amt.end());
    	xmlNewChild(node, NULL, BAD_CAST "amount", BAD_CAST amt.c_str());
	}

	xmlSaveFormatFileEnc("files/top10Transactions.xml", doc, "UTF-8", 1);
   xmlFreeDoc(doc);
   xmlCleanupParser();
}

void dataProcess::noFraud(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery noFraud(dbfile);
	string sql = "SELECT `Merchant State`, `Is Fraud?` from transactions";
	noFraud.exec(sql, fraudCountState_callback, records);

   set<string> states;
   ifstream file("files/states.csv");
   string line;
   while (getline(file, line)) {
   		vector<string> row = dataGen::split(line, ',');
   		states.insert(row[2]);
   }

   
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "noFraud", NULL);

    for (auto& itr: records) {
    	if (states.find(itr.first) != states.end()) {
    		xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "TotalNoFraud", BAD_CAST to_string(itr.second).c_str());
    		xmlNewProp(rtnode, BAD_CAST "state", BAD_CAST itr.first.c_str());
    	}
	}


	xmlSaveFormatFileEnc("files/noFraud.xml", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void dataProcess::lateNight(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery lateNight(dbfile);
	string sql = "SELECT Time, Amount, `Merchant City`, Zip from transactions";
	lateNight.exec(sql, lateNight_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "lateNight", NULL);

	for (auto& itr: records) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "LateNightTrans", BAD_CAST to_string(itr.second).c_str());
    	xmlNewProp(rtnode, BAD_CAST "zipcode", BAD_CAST "Online");
	}

	xmlSaveFormatFileEnc("files/lateNight.xml", doc, "UTF-8", 1);
   xmlFreeDoc(doc);
   xmlCleanupParser();
}

void dataProcess::bottom5(const char* dbfile) {
	unordered_map<string, long> records;
	vector<string> months = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	SQLQuery lateNight(dbfile);
	string sql = "SELECT Month, `Merchant City` from transactions";
	lateNight.exec(sql, bottom_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "bottom5", NULL);

	// Transfer to vector to sort by value
   vector<pair<string, long>> map_vec;
   for (auto& it : records) {
    	map_vec.push_back(it);
	}

	sort(std::execution::par, map_vec.begin(), map_vec.end(), [](pair<string, long> a, pair<string, long> b) {
		return a.second < b.second;
	});

	for (int i = 0; i < 5; i++) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "bottom5", BAD_CAST to_string(map_vec[i].second).c_str());
    	int m = stoi(map_vec[i].first);
    	xmlNewProp(rtnode, BAD_CAST "month", BAD_CAST months[m].c_str());
	}

	xmlSaveFormatFileEnc("files/bottom5.xml", doc, "UTF-8", 1);
   xmlFreeDoc(doc);
   xmlCleanupParser();
}

void dataProcess::deposits(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery database(dbfile);
	string sql = "SELECT User from transactions";
	database.exec(sql, singleCount_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "deposits", NULL);

	for (auto& itr: records) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "numTrans", BAD_CAST to_string(itr.second).c_str());
    	xmlNewProp(rtnode, BAD_CAST "userid", BAD_CAST itr.first.c_str());
	}

	xmlSaveFormatFileEnc("files/deposits.xml", doc, "UTF-8", 1);
   xmlFreeDoc(doc);
   xmlCleanupParser();
}

void dataProcess::noError5(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery database(dbfile);
	string sql = "SELECT `Merchant Name`, `Errors?` from transactions";
	database.exec(sql, noErrors_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "noError5", NULL);

	// Transfer to vector to sort by value
   	vector<pair<string, long>> map_vec;
   	for (auto& it : records) {
    	map_vec.push_back(it);
	}

	sort(execution::par, map_vec.begin(), map_vec.end(), [](pair<string, long> a, pair<string, long> b) {
		return a.second > b.second;
	});

	for (int i = 0; i < 5; i++) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "merchantID", BAD_CAST (map_vec[i].first).c_str());
    	xmlNewProp(rtnode, BAD_CAST "count", BAD_CAST to_string(map_vec[i].second).c_str());
	}

	xmlSaveFormatFileEnc("files/noError5.xml", doc, "UTF-8", 1);
   	xmlFreeDoc(doc);
   	xmlCleanupParser();
}

void dataProcess::onlineCity(const char* dbfile) {
	unordered_map<string, long> records;

	SQLQuery database(dbfile);
	string sql = "SELECT `Merchant Name`, `Errors?` from transactions";
	database.exec(sql, noErrors_callback, records);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "onlineCity", NULL);

	// Transfer to vector to sort by value
   	vector<pair<string, long>> map_vec;
   	for (auto& it : records) {
    	map_vec.push_back(it);
	}

	sort(execution::par, map_vec.begin(), map_vec.end(), [](pair<string, long> a, pair<string, long> b) {
		return a.second > b.second;
	});

	for (int i = 0; i < 5; i++) {
    	xmlNodePtr rtnode = xmlNewChild(node, NULL, BAD_CAST "merchantID", BAD_CAST (map_vec[i].first).c_str());
    	xmlNewProp(rtnode, BAD_CAST "count", BAD_CAST to_string(map_vec[i].second).c_str());
	}

	xmlSaveFormatFileEnc("files/onlineCity.xml", doc, "UTF-8", 1);
   	xmlFreeDoc(doc);
   	xmlCleanupParser();
}

// void dataProcess::genXMLs() {
// 	thread t1(atLeastOneInsufficientBalance);
// 	thread t2(gen_cards);
// 	thread t3(gen_merchants);
// 	thread t4(gen_states);

// 	t1.join();
// 	t2.join();
// 	t3.join();
// 	t4.join();
// }


int main() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	// dataProcess::noError5();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;
	return 0;
}
