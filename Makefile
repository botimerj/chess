
CC 		:= g++
INCLUDE := -Iinclude/ -Isrc/ 
INCLUDE += -I/usr/include/
LINK 	:= -Lsrc/ -Llib/ -L/usr/lib/x86_64-linux-gnu/ -lfreetype -lglfw3 -lglad -lm -lrt -ldl -lX11 -lpthread
LINK	+= -lz -lpng -lharfbuzz
SRC 	:= src/*.cpp



all: chess 

chess: 
	$(CC) $(INCLUDE) $(SRC) main.cpp -o main $(LINK) 

ui_test: 
	$(CC) $(INCLUDE) $(SRC) ui_test.cpp -o ui_test $(LINK) 

game_test: 
	$(CC) $(INCLUDE) src/game.cpp game_test.cpp -o game_test 

agent0:
	$(CC) $(INCLUDE) src/game.cpp agent0.cpp -o agent0 $(LINK) 

clean:
	rm main ui_test
.PHONY: all chess ui_test game_test agent0
