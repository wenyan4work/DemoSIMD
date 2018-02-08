CXX=g++-mp-7

CXXFLAGS=  -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 -Wa,-q -I/Users/wyan/local/include/eigen3
LDLIBS= $(CXXFLAGS)

RM = rm -f
MKDIRS = mkdir -p

# TODO: generate obj and bin folder for each compiler flag combination
BINDIR = ./bin
SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./include

INC = \
	$(INCDIR)/Timer.hpp

TARGET_BIN = \
       $(BINDIR)/memcpy \
       $(BINDIR)/arrayscan \
       $(BINDIR)/gemm \

all : $(TARGET_BIN)

$(BINDIR)/%: $(OBJDIR)/%.o
	-@$(MKDIRS) $(dir $@)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-@$(MKDIRS) $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $^ -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*
	$(RM) *~ */*~

