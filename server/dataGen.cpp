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
#include <algorithm>
#include <tuple>
#include "dasmig/namegen.hpp"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/xmlenc.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include "dataGen.h"

using ng = dasmig::ng;
using namespace std;

namespace dataGen {
	vector<string> split (const string &s, char delim) {
	    vector<string> result;
	    stringstream ss (s);
	    string item;
	    while (getline (ss, item, delim)) {
	        result.push_back (item);
	    }
	    return result;
	}

// void split_csv() {
// 	ifstream csvfile("files/small.csv");
// 	string line, word;
// 	getline(csvfile, line);
// 	vector<string> colnames = split(line, ",");
// 	while(getline(csvfile, line)) {
// 		vector<string> tokens = split(line, ',');
// 		if (tokens.size() > 15) {
// 			tokens.erase(tokens.begin() + 14);
// 		}
// 		for_each(tokens.begin(), tokens.end(), [] (string& s) { s = "'" + s + "'"; });
// 		stringstream ss;
// 		for_each(tokens.begin(), tokens.end(), [&ss] (const string& s) { ss << s << ","; });
// 		string myline = ss.str();
// 		myline.pop_back();
// 	}

// }

	int sign_file(const char* xml_file, const char* key_file) {
	    xmlDocPtr doc = NULL;
	    xmlNodePtr signNode = NULL;
	    xmlNodePtr refNode = NULL;
	    xmlNodePtr keyInfoNode = NULL;
	    xmlSecDSigCtxPtr dsigCtx = NULL;
	    int res = -1;
	    
	    /* load doc file */
	    doc = xmlParseFile(xml_file);
	    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)){
	        fprintf(stderr, "Error: unable to parse file \"%s\"\n", xml_file);
	        goto done;      
	    }
	    /* create signature template for RSA-SHA1 enveloped signature */
	    signNode = xmlSecTmplSignatureCreate(doc, xmlSecTransformExclC14NId, xmlSecTransformRsaSha1Id, NULL);
	    if(signNode == NULL) {
	        fprintf(stderr, "Error: failed to create signature template\n");
	        goto done;              
	    }

	    /* add <dsig:Signature/> node to the doc */
	    xmlAddChild(xmlDocGetRootElement(doc), signNode);
	    /* add reference */
	    refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha1Id, NULL, NULL, NULL);
	    if(refNode == NULL) {
	        fprintf(stderr, "Error: failed to add reference to signature template\n");
	        goto done;              
	    }
	    /* add enveloped transform */
	    if(xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformEnvelopedId) == NULL) {
	        fprintf(stderr, "Error: failed to add enveloped transform to reference\n");
	        goto done;              
	    }
	    /* add <dsig:KeyInfo/> and <dsig:KeyName/> nodes to put key name in the signed document */
	    keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo(signNode, NULL);
	    if(keyInfoNode == NULL) {
	        fprintf(stderr, "Error: failed to add key info\n");
	        goto done;              
	    }
	    if(xmlSecTmplKeyInfoAddKeyName(keyInfoNode, NULL) == NULL) {
	        fprintf(stderr, "Error: failed to add key name\n");
	        goto done;              
	    }
	    /* create signature context, we don't need keys manager in this example */
	    dsigCtx = xmlSecDSigCtxCreate(NULL);
	    if(dsigCtx == NULL) {
	        fprintf(stderr,"Error: failed to create signature context\n");
	        goto done;
	    }
	    /* load private key, assuming that there is not password */
	    dsigCtx->signKey = xmlSecCryptoAppKeyLoad(key_file, xmlSecKeyDataFormatPem, NULL, NULL, NULL);
	    if(dsigCtx->signKey == NULL) {
	        fprintf(stderr,"Error: failed to load private pem key from \"%s\"\n", key_file);
	        goto done;
	    }
	    if(xmlSecDSigCtxSign(dsigCtx, signNode) < 0) {
	        fprintf(stderr,"Error: signature failed\n");
	        goto done;
	    }
	    xmlSaveFormatFileEnc(xml_file, doc, "UTF-8", 1);
	    res = 0;
	done:    
	    /* cleanup */
	    if(dsigCtx != NULL) {
	        xmlSecDSigCtxDestroy(dsigCtx);
	    }
	    
	    if(doc != NULL) {
	        xmlFreeDoc(doc); 
	    }
	    return(res);
	}

	void gen_user() {
		set<string> records;
		string line;
	    ifstream file("files/card_transaction.csv");

		if(file.is_open()){
			getline(file, line);
			while(getline(file, line)){
				vector<string> tokens = split(line, ',');
				records.insert(tokens[0]);
			}
		} 
		file.close();

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
		}
		xmlSaveFormatFileEnc("files/user.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();

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
		unordered_map<string, set<string> > records;
		string line;
	    ifstream file("files/card_transaction.csv");

		if(file.is_open()){
			getline(file, line);
			while(getline(file, line)){
				vector<string> tokens = split(line, ',');
				records[tokens[0]].insert(tokens[1]);
			}
		}
		file.close();

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
			}
		}

		xmlSaveFormatFileEnc("files/credit_cards.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
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
		unordered_map<string, string> records;
		string line;
	    ifstream file("files/card_transaction.csv");

		if(file.is_open()){
			getline(file, line);
			while(getline(file, line)){
				vector<string> tokens = split(line, ',');
				records[tokens[8]] = tokens[12];
			}
		}
		file.close();

		vector<string> mNames = getMercFiles("./resources/merchants/merchant.names");

		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	    xmlDocSetRootElement(doc, root_node);
	    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "merchants", NULL);

	    for (auto it = records.begin(); it != records.end(); it++) {
	    	xmlNodePtr node1 = xmlNewChild(node, NULL, BAD_CAST "merchant", NULL);
	    	xmlNewProp(node1, BAD_CAST "id", BAD_CAST it->first.c_str());
	    	xmlNewChild(node1, NULL, BAD_CAST "merchantName", BAD_CAST mNames[rand() % mNames.size()].c_str());
	    	xmlNewChild(node1, NULL, BAD_CAST "mcc", BAD_CAST it->second.c_str());
		}
		xmlSaveFormatFileEnc("files/merchants.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
	}

	void gen_states() {
		unordered_map<string, set<string> > records;
		string rrow;
	    ifstream file("files/card_transaction.csv");

		if(file.is_open()){
			getline(file, rrow);
			while(getline(file, rrow)){
				vector<string> tokens = split(rrow, ',');
				records[tokens[10]].insert(tokens[11]);
			}
		}
		file.close();

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
	        }
	    }
	    xmlSaveFormatFileEnc("files/states.xml", doc, "UTF-8", 1);
	    xmlFreeDoc(doc);
	    xmlCleanupParser();
	}

	int signXML(const char* filename) {
		/* Init libxml and libxslt libraries */
	    xmlInitParser();
	    LIBXML_TEST_VERSION
	    xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
	    xmlSubstituteEntitiesDefault(1);

	    /* Init xmlsec library */
	    if(xmlSecInit() < 0) {
	        fprintf(stderr, "Error: xmlsec initialization failed.\n");
	        return(-1);
	    }

	    /* Load default crypto engine if we are supporting dynamic
	     * loading for xmlsec-crypto libraries. Use the crypto library
	     * name ("openssl", "nss", etc.) to load corresponding 
	     * xmlsec-crypto library.
	     */
	#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
	    if(xmlSecCryptoDLLoadLibrary(NULL) < 0) {
	        fprintf(stderr, "Error: unable to load default xmlsec-crypto library. Make sure\n"
	                        "that you have it installed and check shared libraries path\n"
	                        "(LD_LIBRARY_PATH and/or LTDL_LIBRARY_PATH) environment variables.\n");
	        return(-1);     
	    }
	#endif /* XMLSEC_CRYPTO_DYNAMIC_LOADING */

	    /* Init crypto library */
	    if(xmlSecCryptoAppInit(NULL) < 0) {
	        fprintf(stderr, "Error: crypto initialization failed.\n");
	        return(-1);
	    }

	    /* Init xmlsec-crypto library */
	    if(xmlSecCryptoInit() < 0) {
	        fprintf(stderr, "Error: xmlsec-crypto initialization failed.\n");
	        return(-1);
	    }

	    if(sign_file(filename, "/home/server_ca/private/document.key") < 0) {
	        return(-1);
	    }    
	    
	    /* Shutdown library */
	    xmlSecCryptoShutdown();
	    xmlSecCryptoAppShutdown();
	    xmlSecShutdown();

	    xmlCleanupParser();
	    return 0;
	}
}
