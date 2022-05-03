#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include "../include/DSign.h"
#include "../include/dataGen.h"
#include "../include/dataProcess.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

using namespace std;


void parseStory (xmlDocPtr doc, xmlNodePtr cur, const char* keyword, vector<string>& records) {
	xmlChar *key;
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
	   	if ((!xmlStrcmp(cur->name, (const xmlChar *) keyword))) {
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		    string str(reinterpret_cast<char*>(key));
		    records.push_back(str);
		    xmlFree(key);
 	    }
	cur = cur->next;
	}
    return;
}

// Test transaction types query
int testTransactionTypes() {
	dataProcess::transactionTypes("../server/transaction.db");

	xmlDocPtr doc;
	xmlNodePtr cur;
	vector<string> records;

	doc = xmlParseFile("files/transactionTypes.xml");
	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return 0;
	}
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return 0;
	}
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"transactionTypes"))){
			parseStory(doc, cur, "type", records);
		}
		 
	cur = cur->next;
	}
      
	xmlFreeDoc(doc);
	assert(records.size() == 3);
	assert(records[0] == "Chip Transaction");
	assert(records[1] == "Online Transaction");
	assert(records[2] == "Swipe Transaction");
	return 1;
}

// Test top 5 transactions grouped by city query
int testcityTrans5() {
	dataProcess::cityTrans5("../server/transaction.db");

	xmlDocPtr doc;
	xmlNodePtr cur;
	vector<string> records;

	doc = xmlParseFile("files/city5.xml");
	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return 0;
	}
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return 0;
	}
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"city5"))){
			parseStory(doc, cur, "Top5", records);
		}
		 
	cur = cur->next;
	}
      
	xmlFreeDoc(doc);
	assert(records.size() == 5);
	assert(records[0] == "Houston");
	assert(records[1] == "Los Angeles");
	assert(records[2] == "Miami");
	assert(records[3] == "Brooklyn");
	assert(records[4] == "Chicago");
	return 1;
}

// Test signing and verifying XML
int sign_and_verify() {
	int r = 0;
	r = DSign::signXML("files/transactionTypes.xml", "files/transactionTypes-s.xml");
	assert(r == 0);
	r = DSign::verifyXML("files/transactionTypes-s.xml");
	assert(r == 0);
	return 1;
}


int main() {
	int count = 0;
	count += testTransactionTypes();
	count += testcityTrans5();
	count += sign_and_verify();

	cout << count << " tests passed" << "\n";
	return 0;
}