CFLAGS= -c
CTEMP=-Wall -Werror -Wextra -pedantic -Wshadow -Wconversion -Wsign-conversion
all: assembler

assembler: translator.o assembler/assembler.o assembler/data.o
	g++ translator.o assembler/assembler.o assembler/data.o -o assm 

translator.o: translator.cpp
	g++ $(CFLAGS) translator.cpp

assembler.o: assembler/assembler.cpp
	g++ $(CFLAGS) assembler/assembler.cpp

data.o: assembler/data.cpp
	g++ $(CFLAGS) assembler/data.cpp

stack.o: stack/stack.cpp
	g++ $(CFLAGS) stack/stack.cpp

log.o: stack/log.cpp
	g++ $(CFLAGS) stack/log.cpp

cpu.o: cpu/cpu.cpp
	g++ $(CFLAGS) cpu/cpu.cpp

executor.o: executor.cpp
	g++ $(CFLAGS) executor.cpp

executor: stack/stack.o stack/log.o cpu/cpu.o executor.o
	g++ executor.o stack/stack.o stack/log.o cpu/cpu.o -o exec 
clean:
	rm -rf *.o hello