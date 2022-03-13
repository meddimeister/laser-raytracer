COMPILER = g++
COMPILER_MPI = mpic++

ifndef NOMAD_HOME
define ECHO_NOMAD
	@echo Please set NOMAD_HOME environment variable.
	@false
endef
endif

UNAME := $(shell uname)

EXE                             = restart.exe
EXE_MPI                         = restart_MPI.exe

COMPILER_OPTIONS                = -g -O2 -ansi 
COMPILER_OPTIONS_MPI            = $(COMPILER_OPTIONS) -DUSE_MPI

LIB_DIR                         = $(NOMAD_HOME)/lib
LIB_NOMAD                       = libnomad.so 
LIB_NOMAD_MPI                   = libnomad.MPI.so

CXXFLAGS                        =           
ifeq ($(UNAME), Linux)
CXXFLAGS                       += -Wl,-rpath,'$(LIB_DIR)'
CXXFLAGS                       += -ansi
endif
CXXFLAGS_MPI                    = $(CXXFLAGS) -DUSE_MPI

LDLIBS                          = -lm -lnomad
LDLIBS_MPI                      = -lm -lmpi -lnomad.MPI

INCLUDE                         = -I$(NOMAD_HOME)/src -I$(NOMAD_HOME)/ext/sgtelib/src -I.
COMPILE                         = $(COMPILER) $(COMPILER_OPTIONS) $(INCLUDE) -c
COMPILE_MPI                     = $(COMPILER_MPI) $(COMPILER_OPTIONS_MPI) $(INCLUDE) -c
OBJS                            = restart.o
OBJS_MPI                        = restart_MPI.o


$(EXE): $(OBJS)
	$(ECHO_NOMAD)
	@echo "   building the scalar version ..."
	@echo "   exe file : "$(EXE)
	@$(COMPILER) -o $(EXE) $(OBJS) $(LDLIBS) $(CXXFLAGS) -L$(LIB_DIR) 
ifeq ($(UNAME), Darwin)
	@install_name_tool -change $(LIB_NOMAD) $(NOMAD_HOME)/lib/$(LIB_NOMAD) $(EXE)
endif


$(EXE_MPI): $(OBJS_MPI) 
	$(ECHO_NOMAD)
	@echo "   building the MPI version ..."
	@echo "   exe file : "$(EXE_MPI)
	@$(COMPILER_MPI) -o  $(EXE_MPI) $(OBJS_MPI) $(LDLIBS_MPI) $(CXXFLAGS_MPI) -L$(LIB_DIR)
ifeq ($(UNAME), Darwin)
	@install_name_tool -change $(LIB_NOMAD_MPI) $(NOMAD_HOME)/lib/$(LIB_NOMAD_MPI) $(EXE_MPI)
endif


restart.o: restart.cpp
	$(ECHO_NOMAD)
	@$(COMPILE) restart.cpp

restart_MPI.o: restart.cpp 
	$(ECHO_NOMAD)
	@$(COMPILE_MPI) restart.cpp -o restart_MPI.o

mpi: $(EXE_MPI)

all: $(EXE) $(EXE_MPI)

clean: ;
	@echo "   cleaning obj files"
	@rm -f $(OBJS) $(OBJS_MPI)

del: ;
	@echo "   cleaning trash files"
	@rm -f core *~
	@echo "   cleaning obj files"
	@rm -f $(OBJS) $(OBJS_MPI)
	@echo "   cleaning exe file"
	@rm -f $(EXE) $(EXE_MPI)


