#include <httpserver.hpp>
#include "DSign.h"

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
                DSign::signXML("files/user.xml", "files/user-s.xml");
                return std::shared_ptr<file_response>(new file_response("files/user-s.xml", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "creditcard") {
                DSign::signXML("files/credit_cards.xml", "files/credit_cards-s.xml");
                return std::shared_ptr<file_response>(new file_response("files/credit_cards-s.xml", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "merchants") {
                DSign::signXML("files/merchants.xml", "files/merchants-s.xml");
                return std::shared_ptr<file_response>(new file_response("files/merchants-s.xml", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "states") {
                DSign::signXML("files/states.xml", "files/states-s.xml");
                return std::shared_ptr<file_response>(new file_response("files/states-s.xml", 200, "application/xml"));
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

class process_transaction_resource : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            if (req.get_arg("arg1") == "0") {
                return std::shared_ptr<string_response>(new string_response("File stored locally\n", 200, "text/plain"));
            } else if (req.get_arg("arg1") == "2") {
                return std::shared_ptr<file_response>(new file_response("AWS not implemented yet", 200, "application/xml"));
            } else if (req.get_arg("arg1") == "1") {
                std::string path = "files/" + req.get_arg("arg2") + ".xml";
                std::string outfile = "files/" + req.get_arg("arg2") + "-s.xml";
                int s = DSign::signXML(path.c_str(), outfile.c_str());
                if (s < 0) {
                    return std::shared_ptr<string_response>(new string_response("resource not available\n", 404, "text/plain"));
                } else {
                    return std::shared_ptr<file_response>(new file_response(outfile, 200, "application/xml"));
                }
            } else {
                return std::shared_ptr<string_response>(new string_response("resource not available\n", 404, "text/plain"));
            }
        } else {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }
};