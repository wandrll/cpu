CFLAGS= -c 
CTEMP=-Wall -Werror -Wextra -pedantic -Wshadow -Wconversion -Wsign-conversion
all: assembler executor

assembler: translator.o assembler/assembler.o assembler/data.o assembler/disassembler.o
	g++ translator.o assembler/assembler.o assembler/data.o assembler/disassembler.o -o assm 

translator.o: translator.cpp assembler/assembler.h constants.h
	g++ $(CFLAGS) translator.cpp

assembler.o: assembler/assembler.cpp constants.h
	g++ $(CFLAGS) assembler/assembler.cpp

disassembler.o: assembler/disassembler.cpp constants.h
	g++ $(CFLAGS) assembler/disassembler.cpp

data.o: assembler/data.cpp
	g++ $(CFLAGS) assembler/data.cpp

stack.o: stack/stack.cpp
	g++ $(CFLAGS) stack/stack.cpp

log.o: stack/log.cpp
	g++ $(CFLAGS) stack/log.cpp

cpu.o: cpu/cpu.cpp constants.h
	g++ $(CFLAGS) cpu/cpu.cpp

executor.o: executor.cpp constants.h
	g++ $(CFLAGS) executor.cpp

executor: stack/stack.o stack/log.o cpu/cpu.o executor.o constants.h
	g++ executor.o stack/stack.o stack/log.o cpu/cpu.o -o exec 
clean:
	rm -rf *.o assm exec stack/*.o cpu/*.o assembler/*.o 