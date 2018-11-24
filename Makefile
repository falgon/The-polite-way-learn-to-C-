shell = bash
BOOK_BRANCH = book
OUTPUTDIR = output

fig:
	@./tools/plantuml/gen.sh

doxygen:
	@make -f ./tools/doxygen/Makefile

math_formula:
	@cd tools/math_formula; npm run run

book: fig 
	make doxygen
	#make math_formula
	mkdir -p $(OUTPUTDIR)
	cp -r *.md Chap* docs assets book.json prh.yml $(OUTPUTDIR)
	ghp-import -m "Auto upload by ghp-import" -b $(BOOK_BRANCH) $(OUTPUTDIR)
	git push origin $(BOOK_BRANCH)
	rm -rf $(OUTPUTDIR)

.PHONY: book fig doxygen math_formula
