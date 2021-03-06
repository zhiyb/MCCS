# Author: Yubo Zhi (normanzyb@gmail.com)

PRG	= mccs
SRC	= main.cpp network.cpp handler.cpp packet.cpp status.cpp protocols.cpp \
	  client.cpp client_play.cpp player.cpp chat.cpp tick.cpp server.cpp

#CROSS	= mipsel-linux-
OPTIMIZE	= -g -O3
CXXFLAGS	= -Wall -Werror -std=c++11 \
		  -Irapidjson/include -Ispdlog/include -Ifast-cpp-csv-parser
LIBS	= -lev -lcrypto -lpthread -lm

OBJ	= $(SRC:%.cpp=%.o)

CXX	= $(CROSS)g++
LD	= $(CROSS)g++

.PHONY: all
all: $(PRG)

.PHONY: run
run: $(PRG)
	./$(PRG)

$(PRG): $(OBJ)
	$(LD) $(OPTIMIZE) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) -c $(OPTIMIZE) $(CXXFLAGS) -o $@ $<

# Dependency:
-include $(OBJ:.o=.d)
%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Clean up
.PHONY: clean
clean:
	rm -f $(OBJ) $(OBJ:%.o=%.d) $(OBJ:%.o=%.d.*) $(PRG)
