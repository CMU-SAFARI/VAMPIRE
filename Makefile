SRCDIR := src
TOOLSDIR := tools
OBJDIR := obj
MAIN := $(SRCDIR)/main.cpp
TRACE_GEN := $(TOOLSDIR)/make_sample_trace.cpp

SRCS := $(filter-out $(MAIN), $(wildcard $(SRCDIR)/*.cpp))
OBJS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

CXXFLAGS += -std=c++11

.PHONY: all clean depend debug backend tests

# all: Default compilation rule, generates binary with optimzation, not suitable for debugging
all: CXXFLAGS += -O3 -march=native
all: backend

# debug: Generates a binary which is easier to debug
debug: CXXFLAGS += -O0 -g -D GLOBAL_DEBUG
debug: backend

# Runs tests from `tests/`
tests: all
	./tests/run_tests.sh

# Actual compilation is handled by function past this comment
backend: depend vampire sampletr

sampletr: traceGen

traceGen: $(TRACE_GEN)
	$(CXX) $(CXXFLAGS) $(TRACE_GEN) -o traceGen
clean:
	rm -f vampire traceGen
	rm -rf $(OBJDIR)

depend: $(OBJDIR)/.depend


$(OBJDIR)/.depend: $(SRCS)
	@mkdir -p $(OBJDIR)
	@rm -f $(OBJDIR)/.depend
	@$(foreach SRC, $(SRCS), $(CXX) $(CXXFLAGS) -MM -MT $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC)) $(SRC) >> $(OBJDIR)/.depend ;)

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJDIR)/.depend
endif


vampire: $(MAIN) $(OBJS) $(SRCDIR)/*.h | depend
	$(CXX) $(CXXFLAGS) -DVAMPIRE -o $@ $(MAIN) $(OBJS)

$(OBJS): | $(OBJDIR)

$(OBJDIR): 
	@mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
