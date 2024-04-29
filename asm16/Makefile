#
#
#
.SUFFIXES:

.SUFFIXES:	.c .o

.c.o:
	gcc -c -Wall -O2 -o $*.o $*.c
#
#
files	= main.o asm16.o hash.o debug.o opcode.o
#
#
#
#
asm16 : $(files)
	gcc -O2 -s $(files) -o $@
#	cp asm16.exe cina16
#
test:
	./cina16.exe inv.c
	cp  inv.bin ../r16sim/testdata.bin
	cat inv.lst
#
test1:
	./cina16.exe nr.c
	cp  nr.bin ../r16sim/testdata.bin
	cat nr.lst
#
test2:
	./cina16.exe infile.c
	cp  infile.bin ../r16sim/testdata.bin
	cat infile.lst
#
testasm:
	./asm16.exe -l infile.asm
	cp  infile.bin ../r16sim/testdata.bin
	cat infile.lst
#
#
#
clean:
	-rm *.o
	-rm asm16.exe
	-rm main
#
#
