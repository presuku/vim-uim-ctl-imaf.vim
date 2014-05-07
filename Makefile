
.PHONY: all archive clean

all: plugin/uim-ctl.so plugin/uim-ctl-pipe

plugin/uim-ctl.so: plugin/uim-ctl.c
	gcc -Wall -shared -o plugin/uim-ctl.so -I/usr/local/include plugin/uim-ctl.c -luim -fPIC

plugin/uim-ctl-pipe: plugin/uim-ctl-pipe.c
	gcc -Wall -o plugin/uim-ctl-pipe -I/usr/local/include plugin/uim-ctl-pipe.c -luim -lpthread -fPIC

archive:
	mkdir uim-ctl
	mkdir uim-ctl/plugin
	cp Makefile README uim-ctl
	cp plugin/uim-ctl.c plugin/uim-ctl.vim plugin/uim-ctl-pipe.c uim-ctl/plugin
	zip -r uim-ctl.zip uim-ctl
	rm -r uim-ctl

clean:
	rm -f plugin/uim-ctl.so
	rm -f plugin/uim-ctl-pipe

