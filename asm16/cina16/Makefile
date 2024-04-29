#################################################################
#	コンパイラは MinGw gccを使用します。
#	試してはいませんがcygwinでも大丈夫なはずです。
#	(cygwinでは、コンパイルオプション -mno-cygwinを付けます)
#	DEBUGの時には、-gをつけ、-sを外します。
#################################################################
# REMOVE DEFAULT MAKE RULE
MAKEFLAGS = -r

.SUFFIXES:

.SUFFIXES:	.c .o


#============================
# DOSかどうかチェック.
 ifdef ComSpec
MSDOS=1
 endif

 ifdef COMSPEC
MSDOS=1
 endif
#============================

 ifdef MSDOS
DOSFLAGS = -D_MSDOS_
EXE = .exe
WIN32LIB= -lkernel32 -luser32 -lgdi32 -lsetupapi 
 else
DOSFLAGS = -D_LINUX_
EXE =
WIN32LIB= 
 endif

#CFLAGS	= -g $(DOSFLAGS) $(CDEFS) -O2 -Wall
CFLAGS	= -s $(DOSFLAGS) $(CDEFS) -O2 -Wall
CFLAGS	+= -Wno-multichar
LIBS	=

TARGET  = cina16$(EXE)
#
#
#

CC = gcc
RC = windres

.c.o:
	$(CC) $(CFLAGS) -c $<
#
#
files	= cina16.o
#files	= main.o opcode.o disasm.o gr.o led.o debug.o linux.o
#
#
#
#
$(TARGET) : $(files)
	$(CC) $(CFLAGS) -o $@ $(files) $(WIN32LIB) -lm $(LIBS)
#
test:
	./cina16$(EXE) nr.c
	cat nr.lst

#	./cina16$(EXE) -S nr.c
#	cat nr.asm
#
test2:
	./cina16$(EXE) -S infile.c
	cat infile.asm
	./cina16$(EXE) infile.c
	cat infile.lst
#
#
clean:
	-rm *.o
	-rm $(TARGET)
#
#
