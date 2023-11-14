CC := gcc
CFLAGS := -Wall -Wextra -std=c99
SRC_DIR := ./src
OUT_DIR := ./out

all: motor jogoUI bot

motor: $(SRC_DIR)/motor.c $(SRC_DIR)/motor.h $(SRC_DIR)/utils.h bot
	$(CC) $(CFLAGS) $< -o $(OUT_DIR)/$@

jogoUI: $(SRC_DIR)/jogoUI.c $(SRC_DIR)/jogoUI.h $(SRC_DIR)/utils.h
	$(CC) $(CFLAGS) $< -o $(OUT_DIR)/$@

bot: $(SRC_DIR)/bot.c
	$(CC) $(CFLAGS) $< -o $(OUT_DIR)/$@

clean:
	rm -f $(OUT_DIR)/*
