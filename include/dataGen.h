#include <unordered_set>
#include "safeq.h"

namespace dataGen {

    std::vector<std::string> split (const std::string &s, const char delim);
    std::string quoteAndEsc(const std::string& word);
    std::string quote(const std::string& word);

    int set_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName);
    int map_set_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName);
    int map_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName);

    int initializeDB();
    int createTable();
    void readth(threadsafe_queue<std::string>& data_queue, std::ifstream& file);
    void processth(threadsafe_queue<std::string>& data_queue, std::string& sql);
    int insertVals();

    void gen_user();

    bool luhn_alg(std::string cnum);
    std::string credit_card_gen(std::unordered_set<std::string>& unique_cards);
    void gen_cards();

    std::vector<std::string> getMercFiles(std::string filename);
    void gen_merchants();

    void gen_states();
}
