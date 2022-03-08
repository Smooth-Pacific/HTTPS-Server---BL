#include <httpserver.hpp>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>

using namespace httpserver;

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

class server_resource : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            return std::shared_ptr<string_response>(new string_response("SUCCESS, Hello World!\n", 200, "text/plain"));
        } else {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }

    const std::shared_ptr<http_response> render_POST(const http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            std::ofstream out("files/out.xml");
            out << req.get_content();
            out.close();
            return std::shared_ptr<string_response>(new string_response("post\n", 200, "text/plain"));
        } else {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }
};

class mime_resource : public http_resource {
public:
    const std::shared_ptr<http_response> render(const http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            if (req.get_arg("arg1") == "image") {
                return std::shared_ptr<file_response>(new file_response("files/mob.png", 200, "image/png"));
            } else if (req.get_arg("arg1") == "xml") {
                return std::shared_ptr<file_response>(new file_response("files/sample.xml", 200, "text/xml"));
            } else {
                return std::shared_ptr<string_response>(new string_response("resource not available\n", 200, "text/plain"));
            }
        } else {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }
};