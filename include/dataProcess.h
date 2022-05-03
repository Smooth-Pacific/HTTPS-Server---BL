#pragma once

namespace dataProcess {

    int IBCount_callback(void *data,  int argc, char **argv, char **azColName);
    int fraudCountYear_callback(void *data, int argc, char **argv, char **azColName);
    int singleCount_callback(void *data, int argc, char **argv, char **azColName);
    int amount_callback(void *data, int argc, char **argv, char **azColName);
    int fraudCountState_callback(void *data, int argc, char **argv, char **azColName);
    int lateNight_callback(void *data, int argc, char **argv, char **azColName);
    int bottom_callback(void *data, int argc, char **argv, char **azColName);
    int noErrors_callback(void *data, int argc, char **argv, char **azColName);
    
    void atLeastOneInsufficientBalance(const char* dbfile = "transaction.db");
    void RepeatedInsufficientBalance(const char* dbfile = "transaction.db");
    void uniqueMerchant(const char* dbfile = "transaction.db");
    void percentageFraud(const char* dbfile = "transaction.db");
    void recurringTrans(const char* dbfile = "transaction.db");
    void zipTrans5(const char* dbfile = "transaction.db");
    void cityTrans5(const char* dbfile = "transaction.db");
    void transactionTypes(const char* dbfile = "transaction.db");
    void top10Transactions(const char* dbfile = "transaction.db");
    void noFraud(const char* dbfile = "transaction.db");
    void lateNight(const char* dbfile = "transaction.db");
    void bottom5(const char* dbfile = "transaction.db");
    void deposits(const char* dbfile = "transaction.db");
    void noError5(const char* dbfile = "transaction.db");
}
