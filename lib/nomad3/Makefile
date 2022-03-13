
ifndef NOMAD_HOME
define ECHO_NOMAD
	@echo Please set NOMAD_HOME environment variable.
	@false
endef
endif

ifeq ("$(wildcard $(NOMAD_HOME)/src/Makefile)","") 
$(info Did not find file $(NOMAD_HOME)/src/Makefile)
$(error Please run configure first)
endif

all:
	$(ECHO_NOMAD)
	cd $(NOMAD_HOME)/src && $(MAKE) all

nompi:
	$(ECHO_NOMAD)
	cd $(NOMAD_HOME)/src && $(MAKE) nompi

mpi:
	$(ECHO_NOMAD)
	cd $(NOMAD_HOME)/src && $(MAKE) mpi

clean:
	cd $(NOMAD_HOME)/src && $(MAKE) clean

del:
	cd $(NOMAD_HOME)/src && $(MAKE) del
