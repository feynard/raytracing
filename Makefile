# Config
TARGET := render
CXX := g++
CXXFLAGS := -std=c++20 -Wall -pedantic -O3
BUILD_DIR := build
SRC_DIR := src

# Make
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(SRCS))
OBJS := $(patsubst %.cpp, %.o, $(OBJS))
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP


all: $(TARGET)


# Application
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# C++
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@ mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

-include $(OBJS:.o=.d)
