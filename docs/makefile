main.pdf: $(shell find . -name '*.tex')
	mkdir -p build &&\
	xelatex --output-dir=build --shell-escape main.tex &&\
	cp build/main.pdf .

clean:
	rm -rf build
