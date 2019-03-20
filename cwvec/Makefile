SRC=src
INC=include
BUILD=build
OBJ=object

CWVEC = $(BUILD)/cwvec
PRINT_OVERFLOW  = $(BUILD)/print_overflow

CC = gcc
CFLAGS = -Wall -O3 -fPIC -I$(INC)
#CFLAGS = -Wall -fPIC -I$(INC) -g
LIBS = -lm
DEFINE = #-D_USE_LINKED_LIST_

all: $(OUT_DIR) $(CWVEC) $(PRINT_OVERFLOW)

OBJECTS = $(OBJ)/str_norm.o $(OBJ)/hash.o $(OBJ)/matrix.o $(OBJ)/tools.o $(OBJ)/vocab_count.o $(OBJ)/contextual_matrix.o $(OBJ)/options.o $(OBJ)/cwvec.o

SUFFIXES = .c .h

$(CWVEC): $(OBJECTS)
	$(CC) -o $(BUILD)/cwvec $(CFLAGS) $(LIBS) $(OBJECTS) $(DEFINE)

$(OBJ)/str_norm.o: $(SRC)/str_norm.c $(INC)/str_norm.h
	$(CC) -c -o $(OBJ)/str_norm.o $(CFLAGS) $(SRC)/str_norm.c $(DEFINE)

$(OBJ)/hash.o: $(SRC)/hash.c $(INC)/hash.h
	$(CC) -c -o $(OBJ)/hash.o $(CFLAGS) $(SRC)/hash.c $(DEFINE)

$(OBJ)/matrix.o: $(SRC)/matrix.c $(INC)/matrix.h
	$(CC) -c -o $(OBJ)/matrix.o $(CFLAGS) $(SRC)/matrix.c $(DEFINE)

$(OBJ)/tools.o: $(SRC)/tools.c $(INC)/tools.h
	$(CC) -c -o $(OBJ)/tools.o $(CFLAGS) $(SRC)/tools.c $(DEFINE)

$(OBJ)/vocab_count.o: $(SRC)/vocab_count.c $(INC)/vocab_count.h
	$(CC) -c -o $(OBJ)/vocab_count.o $(CFLAGS) $(SRC)/vocab_count.c $(DEFINE)

$(OBJ)/contextual_matrix.o: $(SRC)/contextual_matrix.c $(INC)/contextual_matrix.h
	$(CC) -c -o $(OBJ)/contextual_matrix.o $(CFLAGS) $(SRC)/contextual_matrix.c $(DEFINE)

$(OBJ)/options.o: $(SRC)/options.c $(INC)/options.h
	$(CC) -c -o $(OBJ)/options.o $(CFLAGS) $(SRC)/options.c $(DEFINE)

$(OBJ)/cwvec.o: $(SRC)/cwvec.c
	$(CC) -c -o $(OBJ)/cwvec.o $(CFLAGS) $(SRC)/cwvec.c $(DEFINE)

$(PRINT_OVERFLOW): $(OBJ)/print_overflow.o
	$(CC) -o $(BUILD)/print_overflow $(CFLAGS) $(OBJ)/print_overflow.o

$(OBJ)/print_overflow.o: $(SRC)/print_overflow.c
	$(CC) -c -o $(OBJ)/print_overflow.o $(CFLAGS) $(SRC)/print_overflow.c


clean: 
	rm -f $(CWVEC) $(OBJECTS) $(OBJ)/print_overflow.o

