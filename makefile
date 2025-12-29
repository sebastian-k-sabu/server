server : main.cpp
	g++ -std=c++17 -pthread main.cpp urlRouting.cpp dataParser.cpp jsonParser.cpp log.cpp

run :
	./a.out

clean :
	rm a.out