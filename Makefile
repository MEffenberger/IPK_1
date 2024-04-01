CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

PROGRAM = ipk24chat-client
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(PROGRAM) $(OBJECTS)