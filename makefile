# server : main.cpp
# 	g++ -std=c++17 -pthread main.cpp urlRouting.cpp dataParser.cpp jsonParser.cpp log.cpp

CXX      := g++
CXXFLAGS := -std=c++20 -pthread
TARGET   := server
SOURCES  := main.cpp logger.cpp multiProcessHandler.cpp

.PHONY: all server run clean rebuild

all: server

server: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: server
	./$(TARGET)

clean:
	rm -f $(TARGET) access.log

rebuild: clean server

