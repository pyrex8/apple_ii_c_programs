SRC = $(wildcard *.c)
EXE :=$(SRC:.c=)

all: $(SRC)
	cl65  -v -W error -t apple2 -Osir -Cl -C ../apple2-hgr2.cfg -c $(SRC)

clean:
	rm -f *.bin *.list *.o
