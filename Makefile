bot: bot.cc gpio.cc
	gcc -o bot -l rt bot.cc gpio.cc /usr/local/lib/libbcm2835.a
