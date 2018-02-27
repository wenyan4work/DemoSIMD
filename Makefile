## switches, yes or no
HAVEPVFMM = yes
HAVEEIGEN = yes
VECREPORT = yes

## change this if you need eigen. no action needed if not using eigen
EIGEN = /mnt/home/wyan/local/include/eigen3


## Possible choices of compilers and flags if not using PVFMM
## Will be replaced if pvfmm is used

## linux intel
CXX=icpc
CXXFLAGS= -std=c++11 -qopenmp -O3 -DNDEBUG -xcore-avx2

## linux gcc
# CXX= g++
# CXXFLAGS= -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 

## mac. DO NOT use mac default gcc/clang. Use gcc from macports or homebrew
## You need special settings to activate avx:
## Install a new clang, use -Wa,-q to use the clang as the assembler
# CXX= g++-mp-7
# CXXFLAGS=  -std=c++11 -fopenmp -O3 -DNDEBUG -march=core-avx2 -Wa,-q

###############################################
#   No modifications needed below this line   #
############################################### 

CXXFLAGS += -DNDEBUG

LD= $(CXX) 
LINKFLAGS= $(CXXFLAGS)

ifeq ($(HAVEPVFMM),yes)
# replace flags with CXXFLAGS_PVFMM and LDLIBS_PVFMM
-include $(PVFMM_DIR)/MakeVariables
CXX= mpicxx # the compiler you used to compile PVFMM
LD= $(CXX) 
CXXFLAGS= $(CXXFLAGS_PVFMM) -DHAVE_PVFMM -no-ipo
LINKFLAGS= $(CXXFLAGS_PVFMM) $(LDLIBS_PVFMM)
endif

ifeq ($(HAVEEIGEN),yes)
# location of EIGEN library
CXXFLAGS += -I$(EIGEN) -DHAVE_EIGEN
endif

ifeq ($(VECREPORT),yes)
# only available for intel compilers
# change 5 to 1 or 2 to generate less report
CXXFLAGS += -qopt-report=5 -qopt-report-phase=vec # or =all
endif

RM = rm -f
MKDIRS = mkdir -p

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

