TARGET = $(notdir $(shell pwd))

ALL_C = $(wildcard *.c) $(wildcard *.h)

all: $(ALL_C)
	cl65 -v -W error -t apple2 -Osir -Cl -C ../apple2-hgr2.cfg -o $(TARGET).bin -l $(TARGET).list $(TARGET).c ../lib/*.o

clean:
	rm -f *.bin *.list *.o
