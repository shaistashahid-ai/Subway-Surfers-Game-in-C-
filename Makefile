SRC_DIR := ProjectOOP
TARGET := ProjectOOPGame

ifeq ($(OS),Windows_NT)
	DEFAULT_SFML_DIR := C:/SFML-2.6.1
	EXE := .exe
	RPATH_FLAG :=
else
	DEFAULT_SFML_DIR := /usr/local/SFML-2.6.1
	EXE :=
	RPATH_FLAG = -Wl,-rpath,$(SFML_DIR)/lib
endif

SFML_DIR ?= $(DEFAULT_SFML_DIR)

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I$(SRC_DIR) -I$(SFML_DIR)/include
LDFLAGS := -L$(SFML_DIR)/lib $(RPATH_FLAG)
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
BINARY := $(TARGET)$(EXE)

SRCS := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/GameEngine.cpp \
	$(SRC_DIR)/Player.cpp \
	$(SRC_DIR)/TrackManager.cpp \
	$(SRC_DIR)/ConcreteObstacles.cpp \
	$(SRC_DIR)/ConcretePowerUps.cpp \
	$(SRC_DIR)/ScoreManager.cpp

OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(BINARY)
	./$(BINARY)

clean:
	$(RM) $(OBJS) $(BINARY)

