BUILD_DIR := build
SRC_DIR := src
TEST_DIR := test

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/test/%.o,$(TEST_SRCS))

.PHONY: debug

debug:
	echo $(SRCS)
	echo $(OBJS)
	echo $(TEST_SRCS)
	echo $(TEST_OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	g++ -c $< -o $@

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.cpp #$(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/test
	g++ -c $< -o $@

$(BUILD_DIR)/test_main: $(TEST_OBJS) $(OBJS)
	@mkdir -p $(BUILD_DIR)
	g++ -o $(BUILD_DIR)/test_main $^ -lCppUTest

test: $(BUILD_DIR)/test_main
	$(BUILD_DIR)/test_main
