
CC 		:= g++
INCLUDE := -Iinclude/ -Isrc/
LINK 	:= -Lsrc/ -Llib/ -lglfw3 -lglad -lm -lrt -ldl -lX11 -lpthread
SRC 	:= src/*.cpp

all: chess 

chess: 
	$(CC) $(INCLUDE) $(SRC) main.cpp -o main $(LINK) 

ui_test: 
	$(CC) $(INCLUDE) $(SRC) ui_test.cpp -o ui_test $(LINK) 

clean:
	rm main ui_test
.PHONY: all chess ui_test
