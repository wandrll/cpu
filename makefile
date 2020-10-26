CFLAGS= -c -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr
LDFLAGS= -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr
TMP = -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr
CTEMP=-Wall -Werror -Wextra -pedantic -Wshadow -Wconversion -Wsign-conversion
all: assembler executor

assembler: translator.o assembler/assembler.o assembler/data.o assembler/disassembler.o constants.h
	g++ $(LDFLAGS) translator.o assembler/assembler.o assembler/data.o assembler/disassembler.o -o assm 

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
	g++ $(CFLAGS) cpu/cpu.cpp -lsfml-graphics -lsfml-window -lsfml-system

executor.o: executor.cpp constants.h
	g++ $(CFLAGS) executor.cpp -lsfml-graphics -lsfml-window -lsfml-system

executor: stack/stack.o stack/log.o cpu/cpu.o executor.o constants.h
	g++ $(LDFLAGS) executor.o stack/stack.o stack/log.o cpu/cpu.o -o exec -lsfml-graphics -lsfml-window -lsfml-system
clean:
	rm -rf *.o assm exec stack/*.o cpu/*.o assembler/*.o 