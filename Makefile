CXX = g++
CC = gcc
CXXFLAGS = -std=c++11 -Iinclude
CFLAGS = -Iinclude

TARGET = ATMSimulator

SRCS_CXX = src/main.cpp src/Account.cpp src/DatabaseManager.cpp src/System.cpp src/UI.cpp src/sha256.cpp
SRCS_C = src/sqlite3.c

OBJS_CXX = $(SRCS_CXX:.cpp=.o)
OBJS_C = $(SRCS_C:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS_CXX) $(OBJS_C)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS_CXX) $(OBJS_C)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_CXX) $(OBJS_C) $(TARGET)
