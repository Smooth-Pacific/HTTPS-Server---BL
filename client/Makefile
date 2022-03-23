all: lat req resp
.PHONY : all clean

lat:
	clang++ -std=c++17 -Wall -o latency latency.cpp -l curl 
req:
	clang++ -std=c++17 -pthread -Wall -o request request.cpp -l curl
resp:
	clang++ -std=c++17 -pthread -Wall -o response response.cpp -l curl 

test:
	echo "latency:"  >> summary.txt
	./latency >> summary.txt
	echo "\nrequest:"  >> summary.txt
	./request >> summary.txt
	echo "\nresponse:"  >> summary.txt
	./response >> summary.txt

clean:
	rm -f *.xml
	rm -f latency
	rm -f request
	rm -f response
	rm -f summary.txt
