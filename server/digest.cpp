#include <httpserver.hpp>

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

using namespace httpserver;

class digest_resource : public httpserver::http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& req) {
        if (req.get_digested_user() == "") {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "test@example.com", MY_OPAQUE, true));
        }
        else {
            bool reload_nonce = false;
            if(!req.check_digest_auth("test@example.com", "mypass", 300, &reload_nonce)) {
                return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "test@example.com", MY_OPAQUE, reload_nonce));
            }
        }
        bool reload_nonce = false;
        if (req.check_digest_auth("test@example.com", "mypass", 300, &reload_nonce)) {
            std::shared_ptr<string_response>(new string_response("password verified\n", 200, "text/plain"));
        }
        return std::shared_ptr<string_response>(new string_response(req.get_digested_user() + "SUCCESS\n", 200, "text/plain"));
    }
};

int main(int argc, char** argv) {
    webserver ws = create_webserver(8080).digest_auth();

    digest_resource hwr;
    ws.register_resource("/hello", &hwr);
    ws.start(true);

    return 0;
}