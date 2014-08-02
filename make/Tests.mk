include make/Config.mk
include make/Defs.mk

ifeq (,$(wildcard $(GMOCK_LIB)))
$(error gmock not configured or built correctly, cannot build tests!)
endif

VPATH                   := $(PROJECT_ROOT)/src/Tests
TEST_BUILD_ROOT         := $(BUILD_ROOT)/Tests
TEST_SRCS               := $(notdir $(shell find $(PROJECT_ROOT)/src/Tests -name "*.cpp" | grep -v '\#'))
TEST_OBJS               := $(addprefix $(TEST_BUILD_ROOT)/,$(TEST_SRCS:.cpp=.o))

TESTS_EXECUTABLE        := $(TEST_BUILD_ROOT)/test-runner

run-tests: $(TESTS_EXECUTABLE)
ifdef COVERAGE
	$(LCOV)  -b $(PROJECT_ROOT) -d $(OBJDIR) --zerocounters --ignore-errors source
endif
	GTEST_OUTPUT="xml:tests-summary.xml" $(TESTS_EXECUTABLE)
ifdef COVERAGE
	$(LCOV) -b $(PROJECT_ROOT) -d $(OBJDIR) --capture -o tests.info --ignore-errors source
	$(LCOV) --extract tests.info '*/src/C++/*'  -o tests.info
	$(GENHTML) tests.info 
endif

tests: $(TESTS_EXECUTABLE)

# Test code (not the library) should always be built DEBUG
# so we can set breakpoints in it.
$(TEST_OBJS): CXX_OPT_FLAGS := $(CXX_OPT_FLAGS_DEBUG)

$(TEST_OBJS): $(TEST_BUILD_ROOT)/%.o : %.cpp $(CXX_LIB) $(GMOCK_INCLUDE) $(GTEST_INCLUDE)
	-mkdir -p $(TEST_BUILD_ROOT)
	$(CXX) -I $(GMOCK_INCLUDE) -I $(GTEST_INCLUDE) -c $< -o $@

$(TESTS_EXECUTABLE): $(TEST_OBJS) $(CXX_LIB) $(GMOCK_LIB) $(GTEST_LIB) $(GTEST_MAIN)
	$(CXX) --coverage $(TEST_OBJS) $(CXX_LIB) $(GMOCK_LIB) $(GTEST_LIB) $(GTEST_MAIN) -lpthread -o $@

.PHONY: run-tests tests
