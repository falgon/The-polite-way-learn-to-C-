shell = bash

BOOK_BRANCH = book
BOOK_COMMIT_MESSAGE = "Auto upload by ghp-import"
BOOK_OUTPUT = book_output

DOXY_BRANCH = gh-pages
DOXY_COMMIT_MESSAGE = "Auto upload by ghp-import"
DOXY_OUTPUT = docs

setup_env:
	@pip install --user -r requirements.txt
	@cd tools/math_formula; npm i; npm audit fix; npm run build

fig:
	@./tools/plantuml/gen.sh

doxygen:
	@cd tools/doxygen; make

math_formula:
	@cd tools/math_formula; npm run run

test:
	make fig
	make doxygen
	make math_formula
	@cd tools; ./swap_mr.sh

clean:
	rm -rf ./assets/formula ./backups $(BOOK_OUTPUT) $(DOXY_OUTPUT)

push_doxy:
	make doxygen
	@mkdir -p $(DOXY_OUTPUT)
	ghp-import -m "$(DOXY_COMMIT_MESSAGE)" -b $(DOXY_BRANCH) $(DOXY_OUTPUT)
	git push --quiet --force git@github.com:falgon/ThePoliteWayLearnToCpp17.git $(DOXY_BRANCH)
	@rm -rf $(DOXY_OUTPUT)

book: 
	make fig 
	make math_formula
	@mkdir -p $(BOOK_OUTPUT)
	@cp -r *.md Chap* assets book.json prh.yml $(BOOK_OUTPUT)
	ghp-import -m "$(BOOK_COMMIT_MESSAGE)" -b $(BOOK_BRANCH) $(BOOK_OUTPUT)
	git push --quiet --force git@github.com:falgon/ThePoliteWayLearnToCpp17.git $(BOOK_BRANCH)
	@cd tools; ./swap_mr.sh
	@rm -rf $(BOOK_OUTPUT)

.PHONY: book push_doxy setup_env test clean
