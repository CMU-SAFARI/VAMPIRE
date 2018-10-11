SRCDIR := src
TOOLSDIR := tools
OBJDIR := obj
MAIN := $(SRCDIR)/main.cpp
TRACE_GEN := $(TOOLSDIR)/make_sample_trace.cpp

SRCS := $(filter-out $(MAIN), $(wildcard $(SRCDIR)/*.cpp))
OBJS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

CXX_COMMON_FLAGS := -std=c++11
CXXFLAGS := -Og -g -Wall $(CXX_COMMON_FLAGS)

.PHONY: all clean depend

all: depend vampire sampletr

debug: all
ifeq ($(CXX), clang++)
CXXFLAGS := -O0 -g $(CXX_COMMON_FLAGS)
else
CXXFLAGS := -Og -g $(CXX_COMMON_FLAGS)
endif

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
	@$(foreach SRC, $(SRCS), $(CXX) $(CXXFLAGS) -DVAMPIRE -MM -MT $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC)) $(SRC) >> $(OBJDIR)/.depend ;)

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJDIR)/.depend
endif


vampire: $(MAIN) $(OBJS) $(SRCDIR)/*.h | depend
	$(CXX) $(CXXFLAGS) -DVAMPIRE -o $@ $(MAIN) $(OBJS)

$(OBJS): | $(OBJDIR)

$(OBJDIR): 
	@mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -DVAMPIRE -c -o $@ $<
