CXX = g++
CXXFLAGS = -Wall -std=c++17

SRC = p2p.cpp
EXEC = peer

all: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXEC)

.PHONY: all clean