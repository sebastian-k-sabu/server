server : main.cpp
	g++ -std=c++17 -pthread main.cpp urlRouting.cpp dataParser.cpp jsonParser.cpp log.cpp

clean :
	rm a.out