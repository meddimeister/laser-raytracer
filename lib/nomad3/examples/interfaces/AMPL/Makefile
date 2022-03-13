COMPILER = g++
COMPILER_MPI = mpic++

ifndef NOMAD_HOME
define ECHO_NOMAD
	@echo Please set NOMAD_HOME environment variable.
	@false
endef
endif

LIBAMPL_DIR = $(LIBAMPLDIR)

CXX = gcc -std=c99
INCLUDE_DIRS = $(LIBAMPL_DIR)/Src/solvers
CXX_OPTS = $(addprefix -I,$(INCLUDE_DIRS))
LIBS = -L$(LIBAMPL_DIR)/Lib -lampl -lfuncadd0 -lm -ldl

%.o: %.c
	$(ECHO_NOMAD)
	$(CXX) -c $(CXX_OPTS) $?

bb.exe: bb.o
	$(ECHO_NOMAD)
	$(CXX) -o $@ $? $(LIBS)

clean:
	rm *.o miniampl
