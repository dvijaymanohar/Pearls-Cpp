create: betting_server.c betting_client.c
	gcc -o betting_server betting_server.c betting_game_common.c -I. -Wall -g
	gcc -o betting_client betting_client.c betting_game_common.c -I. -Wall -g

clean:
	rm betting_server betting_client
