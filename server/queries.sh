#!/bin/bash

mkdir queries
cd queries

curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/atLeastOneInsufficientBalance'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/RepeatedInsufficientBalance'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/uniqueMerchant'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/percentageFraudByYear'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/recurringTrans5'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/zipcode5'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/city5'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/transactionTypes'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/top10Transactions'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/noFraud'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/lateNight'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/bottom5'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/deposits'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/noError5'
curl -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/onlineCity'