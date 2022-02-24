#include <httpserver.hpp>
#include <cstdlib>

using namespace httpserver;

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

class server_resource : public http_resource {
public:
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req) {
        std::string user = getenv("USERNAME") ? static_cast<std::string>(getenv("USERNAME")) : std::string("");
        std::string pass = getenv("PASSWORD") ? static_cast<std::string>(getenv("PASSWORD")) : std::string("");
        bool reload_nonce = false;

        if (req.get_digested_user() == user && req.check_digest_auth("MyRealm", pass, 300, &reload_nonce)) {
            return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("SUCCESS, Hello World!\n", 200, "text/plain"));
        } else {
            return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }
};