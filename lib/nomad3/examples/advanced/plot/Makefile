COMPILER = g++
COMPILER_MPI = mpic++

ifndef NOMAD_HOME
define ECHO_NOMAD
	@echo Please set NOMAD_HOME environment variable.
	@false
endef
endif

UNAME := $(shell uname)

EXE                    = nomad_plot.exe
COMPILER_OPTIONS       = -ansi -O2

LIB_DIR                = $(NOMAD_HOME)/lib
LIB_NOMAD              = libnomad.so 

CXXFLAGS               =           
ifeq ($(UNAME), Linux)
CXXFLAGS              += -Wl,-rpath,'$(LIB_DIR)'
CXXFLAGS              += -ansi
endif

LDLIBS                 = -lm -lnomad


INCLUDE                = -I $(NOMAD_HOME)/src -I $(NOMAD_HOME)/ext/sgtelib/src -I.
COMPILE                = $(COMPILER) $(COMPILER_OPTIONS) $(INCLUDE) -c
OBJS                   = nomad_plot.o

$(EXE): $(OBJS)
	$(ECHO_NOMAD)
	@echo "   building the scalar version ..."
	@echo "   exe file : "$(EXE)
	@$(COMPILER) -o $(EXE) $(OBJS) $(LDLIBS) $(CXXFLAGS) -L$(LIB_DIR) 
ifeq ($(UNAME), Darwin)
	@install_name_tool -change $(LIB_NOMAD) $(LIB_DIR)/$(LIB_NOMAD) $(EXE)
endif


nomad_plot.o: nomad_plot.cpp
	$(ECHO_NOMAD)
	$(COMPILE) nomad_plot.cpp

clean:
	@echo "   cleaning obj files"
	@rm -f $(OBJS)

del:
	@echo "   cleaning trash files"
	@rm -f core *~
	@echo "   cleaning obj files"
	@rm -f $(OBJS)
	@echo "   cleaning exe file"
	@rm -f $(EXE)
