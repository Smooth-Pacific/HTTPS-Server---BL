#include <httpserver.hpp>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include "dataGen.h"

using namespace httpserver;

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

class transaction_resource : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            if (req.get_arg("arg1") == "user") {
                dataGen::gen_user();
                dataGen::signXML("files/user.xml");
                return std::shared_ptr<file_response>(new file_response("files/user.xml", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "creditcard") {
                dataGen::gen_cards();
                dataGen::signXML("files/credit_cards.xml");
                return std::shared_ptr<file_response>(new file_response("files/credit_cards.xml", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "merchants") {
                dataGen::gen_merchants();
                dataGen::signXML("files/merchants.xml");
                return std::shared_ptr<file_response>(new file_response("files/merchants.xml", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "states") {
                dataGen::gen_states();
                dataGen::signXML("files/states.xml");
                return std::shared_ptr<file_response>(new file_response("files/states.xml", 200, "application/xml"));
            } else {
                return std::shared_ptr<string_response>(new string_response("resource not available\n", 404, "text/plain"));
            }
        } else {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }

    const std::shared_ptr<http_response> render_POST(const http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            return std::shared_ptr<string_response>(new string_response("Post request, file received\n", 200, "text/plain"));
        } else {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }
};