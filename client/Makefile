lat:
	clang++ -std=c++17 -Wall -o latency latency.cpp -l curl 
req:
	clang++ -std=c++17 -pthread -Wall -o request request.cpp -l curl
resp:
	clang++ -std=c++17 -pthread -Wall -o response response.cpp -l curl  
clean:
	rm -f *.xml
	rm -f latency
	rm -f request
	rm -f response
