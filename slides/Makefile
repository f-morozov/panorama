.DEFAULT_GOAL := slides
slides:
	pdflatex -interaction=nonstopmode slides_template.tex

.PHONY: clean
clean:
	@rm -f *.aux *.log *.nav *.out *.snm *.toc *.bcf *.xml\
		images/*converted-to*
