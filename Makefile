# linux intel
# CXX=icpc
# CXXFLAGS= -std=c++11 -fopenmp -O3 -DNDEBUG -xcore-avx2 -I/mnt/home/wyan/local/include/eigen3 -qopt-report=1 -qopt-report-phase=vec

# linux gcc
 CXX=g++
 CXXFLAGS= -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 -I/mnt/home/wyan/local/include/eigen3

# mac
#CXX=g++-mp-7
#CXXFLAGS=  -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 -Wa,-q -I/Users/wyan/local/include/eigen3

LD= $(CXX)

RM = rm -f
MKDIRS = mkdir -p

# TODO: generate obj and bin folder for each compiler flag combination
BINDIR = ./bin
SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./include

INC = \
	$(INCDIR)/Timer.hpp \
	$(INCDIR)/AlignedMemory.hpp

TARGET_BIN = \
       $(BINDIR)/memcpy \
       $(BINDIR)/arrayscan \
       $(BINDIR)/gemm \

all : $(TARGET_BIN)

$(BINDIR)/%: $(OBJDIR)/%.o
	-@$(MKDIRS) $(dir $@)
	$(LD) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-@$(MKDIRS) $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $^ -o $@

.PRECIOUS: $(OBJDIR)/%.o

.PHONY: clean

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*
	$(RM) *~ */*~

