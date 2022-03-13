COMPILER = g++
COMPILER_MPI = mpic++

ifndef NOMAD_HOME
define ECHO_NOMAD
	@echo Please set NOMAD_HOME environment variable.
	@false
endef
endif

UNAME := $(shell uname)

EXE                    = categorical.exe

COMPILER_OPTIONS       = -O2 -ansi 

LIB_DIR                = $(NOMAD_HOME)/lib
LIB_NOMAD              = libnomad.so 

CXXFLAGS               =           
ifeq ($(UNAME), Linux)
CXXFLAGS              += -Wl,-rpath,’$(LIB_DIR)’
CXXFLAGS              += -ansi
CXXFLAGS              += -pthread
endif

LDLIBS                 = -lm -lnomad -lsgtelib

INCLUDE             = -I$(NOMAD_HOME)/src -I$(NOMAD_HOME)/ext/sgtelib/src -I.
COMPILE             = $(COMPILER) $(COMPILER_OPTIONS) $(INCLUDE) -c
OBJS                = categorical.o


$(EXE): $(L1) $(OBJS)
	$(ECHO_NOMAD)
	@echo "   building the parallel (pthreads) version ..."
	@echo "   exe file : "$(EXE)
	@$(COMPILER) -o  $(EXE) $(OBJS) $(LDLIBS) $(CXXFLAGS) -L$(LIB_DIR) 
ifeq ($(UNAME), Darwin)
	@install_name_tool -change $(LIB_NOMAD) $(NOMAD_HOME)/lib/$(LIB_NOMAD) $(EXE)
endif



categorical.o: categorical.cpp
	$(ECHO_NOMAD)
	@$(COMPILE) categorical.cpp

$(L1) : ;
	$(ECHO_NOMAD)


all: $(EXE) 

clean: ;
	@echo "   cleaning obj files"
	@rm -f $(OBJS) 

del: ;
	@echo "   cleaning trash files"
	@rm -f core *~
	@echo "   cleaning obj files"
	@rm -f $(OBJS) 
	@echo "   cleaning exe file"
	@rm -f $(EXE) 


