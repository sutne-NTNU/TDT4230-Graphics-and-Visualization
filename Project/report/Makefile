PDVARS += --highlight-style=pygments # the default theme
PDVARS += --filter pandoc-crossref # Reference figures etc.
PDVARS += --listings # Code
PDVARS += --citeproc # Use bibliography

SETUP = -s

%.pdf: %.md Makefile
	pandoc ${SETUP} -i $< ${PDVARS} -o $@ --template template.tex --pdf-engine=pdflatex

create:
	@make report.pdf

.PHONY: create