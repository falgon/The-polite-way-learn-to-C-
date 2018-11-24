shell = bash
BOOK_BRANCH = book
OUTPUTDIR = output
COMMIT_MESSAGE = "Auto upload by ghp-import"

setup_env:
	@pip install --user -r requirements.txt
	@cd tools/math_formula; npm i; npm audit fix; npm run build

fig:
	@./tools/plantuml/gen.sh

doxygen:
	@make -f ./tools/doxygen/Makefile

math_formula:
	@cd tools/math_formula; npm run run

test:
	make fig
	make doxygen
	make math_formula
	@cd tools; ./swap_mr.sh

clean:
	rm -rf ./assets/formula ./backups

book: fig 
	make doxygen
	make math_formula
	@mkdir -p $(OUTPUTDIR)
	@cp -r *.md Chap* docs assets book.json prh.yml $(OUTPUTDIR)
	ghp-import -m "$(COMMIT_MESSAGE)" -b $(BOOK_BRANCH) $(OUTPUTDIR)
	git push --quiet --force git@github.com:falgon/ThePoliteWayLearnToCpp17.git $(BOOK_BRANCH)
	@cd tools; ./swap_mr.sh
	@rm -rf $(OUTPUTDIR)

.PHONY: book setup_env test clean
