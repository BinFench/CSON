default: install

deps:
	@mkdir deps && cd deps && git clone https://github.com/BinFench/CBoil > /dev/null 2>&1 && cd CBoil && make static --no-print-directory > /dev/null 2>&1
	@cp deps/CBoil/libCBoil.o src/libCBoil.o
	@cd deps/CBoil && make --no-print-directory > /dev/null

install: clean deps
	@cd src && make install --no-print-directory

install-quiet:
	@make install --no-print-directory > /dev/null

uninstall:
	@cd src && make uninstall --no-print-directory

test: install-quiet
	@cd test && make --no-print-directory

test-mem: install-quiet
	@cd test && make test-mem --no-print-directory

clean:
	@rm -rf deps
	@cd test && make clean --no-print-directory
