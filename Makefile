-include $(PVFMM_DIR)/MakeVariables

EIGEN = /mnt/home/wyan/local/include/eigen3
# PVFMMINC = /mnt/home/wyan/local/include/pvfmm
# PVFMMLIB = /mnt/home/wyan/local/lib/pvfmm
# LDFLAGS= -L$(PVFMMLIB) -lpvfmm

# linux intel
CXX=mpicxx
CXXFLAGS= -std=c++11 -qopenmp -O3 -DNDEBUG -xcore-avx2 -qopt-report=1 -qopt-report-phase=vec

# linux gcc
# CXX=g++
# CXXFLAGS= -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 

# mac
#CXX=g++-mp-7
#CXXFLAGS=  -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 -Wa,-q -I/Users/wyan/local/include/eigen3

CXXFLAGS= $(CXXFLAGS_PVFMM) -I$(EIGEN)
LD= $(CXX) 
LINKFLAGS= $(CXXFLAGS_PVFMM) $(LDLIBS_PVFMM)

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
       $(BINDIR)/rsqrt \
       $(BINDIR)/kernelsum

all : $(TARGET_BIN)

$(BINDIR)/%: $(OBJDIR)/%.o
	-@$(MKDIRS) $(dir $@)
	$(LD) $^ -o $@ $(LINKFLAGS) 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-@$(MKDIRS) $(dir $@)
	$(CXX) -c $^ -o $@  $(CXXFLAGS) -I$(INCDIR) 

.PRECIOUS: $(OBJDIR)/%.o

.PHONY: clean

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*
	$(RM) *~ */*~

