PROJECT = rom
OUTDIR = out
SRC = $(wildcard src/*.c)

all:
	@echo "=== Building SGDK project ==="
	@mkdir -p $(OUTDIR)
	@make -f /opt/sgdk/makefile.gen PROJECT=$(PROJECT) SRC="$(SRC)" OUTDIR=$(OUTDIR)

clean:
	rm -rf $(OUTDIR)
