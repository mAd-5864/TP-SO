all: motor jogoUI

motor: bot ./src/motor.c ./src/motor.h ./src/utils.h
	gcc ./src/motor.c -o ./out/motor

jogoUI: ./src/jogoUI.c ./src/jogoUI.h ./src/utils.h
	gcc ./src/jogoUI.c -o ./out/jogoUI

bot: ./src/bot.c
	gcc ./src/bot.c -o ./out/bot

clean:
	rm -f ./out/*
