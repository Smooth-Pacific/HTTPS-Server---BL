#include <unordered_set>

namespace dataGen {

    std::vector<std::string> split (const std::string &s, char delim);

    int sign_file(const char* xml_file, const char* key_file);

    void gen_user();

    bool luhn_alg(std::string cnum);

    std::string credit_card_gen(std::unordered_set<std::string>& unique_cards);

    void gen_cards();

    std::vector<std::string> getMercFiles(std::string filename);

    void gen_merchants();

    void gen_states();

    int signXML(const char* filename);
}
