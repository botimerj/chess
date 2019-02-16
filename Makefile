
CC 		:= g++
INCLUDE := -Iinclude/ -Isrc/ 
INCLUDE += -I/usr/include/
LINK 	:= -Lsrc/ -Llib/ -lfreetype -lglfw3 -lglad -lm -lrt -ldl -lX11 -lpthread
LINK	+= -L/usr/lib/x86_64-linux-gnu -lz -lpng -lharfbuzz
SRC 	:= src/*.cpp



all: chess 

chess: 
	$(CC) $(INCLUDE) $(SRC) main.cpp -o main $(LINK) 

ui_test: 
	$(CC) $(INCLUDE) $(SRC) ui_test.cpp -o ui_test $(LINK) 

clean:
	rm main ui_test
.PHONY: all chess ui_test
