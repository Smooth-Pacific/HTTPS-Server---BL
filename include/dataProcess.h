#pragma once

namespace dataProcess {

    int IBCount_callback(void *data, __attribute__((unused)) int argc, char **argv, __attribute__((unused)) char **azColName);
    
    void atLeastOneInsufficientBalance();
    void RepeatedInsufficientBalance();
    void uniqueMerchant();
}
