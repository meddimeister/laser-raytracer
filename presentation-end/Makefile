################################################################################
# Settings
################################################################################

COMPILER := pdflatex
BIBTEXER := biber

DOCUMENT = presentation
REFERENCES = references.bib

TEXINPUTS := ./themefau/:$(TEXINPUTS)


################################################################################
# Variables
################################################################################

COMPILER := TEXINPUTS=$(TEXINPUTS) $(COMPILER)

ifdef VERBOSE
PIPE :=
else
PIPE := 1>/dev/null 2>/dev/null
COMPILER := $(COMPILER) -interaction=nonstopmode
endif

ifdef REFERENCES
BIBDEP := %.bbl
else
BIBDEP :=
endif


################################################################################
# Functions
################################################################################

check_error = \
  if [ ! $(1) -eq 0 ]; then \
    cat $(2).log | perl -0777 -ne 'print m/\n! .*?\nl\.\d.*?\n.*?(?=\n)/gs'; \
    exit 1; \
  fi

compile = $(COMPILER) $(1).tex $(PIPE) || $(call check_error, $$?, $(1))


################################################################################
# Targets
################################################################################

.PHONY: all presentation clean distclean
.PRECIOUS: %.pdf %.bcf %.bbl

all: presentation

presentation: $(DOCUMENT).build

%.status:
	@# Echo status message
	@echo "Building $*.pdf"

%.pdf %.bcf: %.status %.tex
	@# Initial compile
	@echo "  Compiling $*.tex"
	@$(call compile, $*)

%.bbl: %.status %.bcf $(REFERENCES) 
	@# Bibliography
	@echo "  Running $(BIBTEXER)"
	-@TEXINPUTS=$(TEXINPUTS) $(BIBTEXER) $* $(PIPE)
	@# Update references
	@echo "  Updating references"
	@$(call compile, $*)
	@# Reset time stamp of .bbl to newer than .bcf
	@touch $@

%.build: %.status %.pdf $(BIBDEP)
	@# Fill in missing references
	@if test -e $*.log && \
	    ( grep -q "There were undefined references." $*.log || \
	      grep -q "Rerun to get outlines right" $*.log ); then  \
	  echo "  Filling in missing references"; \
	  $(call compile, $*); \
	fi
	@# Fix cross-references
	@while test -e $*.log && \
	    grep -q "Rerun to get cross-references right." $*.log; do \
	  echo "  Fixing cross-references"; \
	  $(call compile, $*); \
	done

clean:
	rm -rf *.aux *.ind *.idx *.toc *.out *.log *.ilg *.dvi *.bbl *.blg *.syg \
		*.syi *.synctex *.slg *.lol *.lof *.ist *.gls *.glo *.gli *.glg \
		*.alg *.acr *.acn *.ps *.defn *.nlo *.satz *.nav *.snm *.xml \
		*.synctex.gz *.synctex *.vrb *.bcf *.makefile *.figlist

distclean: clean
	rm -f *.pdf

