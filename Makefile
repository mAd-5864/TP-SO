CC := gcc
CFLAGS := 
LDFLAGS := -lncurses
SRC_DIR := ./src
OUT_DIR := ./out

all: motor jogoUI bot

motor: 
	$(CC) $(CFLAGS) $(SRC_DIR)/motor.c -o $(OUT_DIR)/$@ $(LDFLAGS)

jogoUI: 
	$(CC) $(CFLAGS) $(SRC_DIR)/jogoUI.c -o $(OUT_DIR)/$@ $(LDFLAGS)

bot: 
	$(CC) $(CFLAGS) $(SRC_DIR)/bot.c -o $(OUT_DIR)/$@

clean:
	rm -f $(OUT_DIR)/*
