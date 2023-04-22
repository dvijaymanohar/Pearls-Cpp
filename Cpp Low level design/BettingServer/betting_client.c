
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "betting_game_common.h"

int
main(
    int argc,
    char **argv)
{
    int sock_fd = 0, port_num = 0, n = 0;
    struct sockaddr_in server_addr;
    struct hostent *server = NULL;
    char *host_name = NULL;
    unsigned int betting_num = 0;

    /* basic check of the arguments */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s  <Server IP address> <port no>\n", argv[0]);
        exit(1);
    }

    host_name = argv[1];
    port_num = atoi(argv[2]);

    /* socket: create the socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0)
    {
        error("ERROR opening socket");
    }

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(host_name);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host as %s\n", host_name);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port_num);

    /* connect: create a connection with the server */
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error("ERROR connecting");
    }

    /* get message line from the user */
    betting_game_common_hdr_t send_hr;
    betting_game_common_hdr_t receive_hdr;

    make_header(PROTOCOL_VERSION, BETTING_GAME_MSG_OPEN, sizeof(send_hr), 0, &send_hr);

    if (send(sock_fd, (betting_game_common_hdr_t *) &send_hr, sizeof(send_hr), 0) == -1)
    {
        perror("send");
    }

    CLIENT_TRACE_LOG("Client Msg:\nBETTING_GAME_MSG_OPEN - |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d|\n",
                      send_hr.version, send_hr.type, send_hr.length, send_hr.client_id);

    int result_received = 0;

    betting_game_result_t result_msg;
    betting_game_msg_accept_t accept_msg;
    betting_game_msg_bet_t bet_msg;

    while (1)
    {
        n = read(sock_fd, &receive_hdr, sizeof(receive_hdr));

        if (n <= 0)
        {
            perror("recv invalid Bet\n\r");
            CLIENT_TRACE_LOG("Closing Socket..: %d", sock_fd);
            close(sock_fd);
            exit(1);
        }
        else
        {
            switch (receive_hdr.type)
            {
                case BETTING_GAME_MSG_ACCEPT:
                {
                    CLIENT_TRACE_LOG("\n\nMsg from Server: BETTING_GAME_MSG_ACCEPT - |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d |\n",
                                     receive_hdr.version, receive_hdr.type, receive_hdr.length, receive_hdr.client_id);

                    if ((read(sock_fd, &accept_msg, sizeof(accept_msg))) <= 0)
                    {
                        perror("recv");
                        exit(1);
                    }

                    CLIENT_TRACE_LOG("|Minimum Limit: 0x%lX | Maximum Limit: 0x%lX\n",
                                     accept_msg.lower_end_of_number, accept_msg.upper_end_of_num);

                    betting_num = gen_betting_number(BETSERVER_NUM_MIN, BETSERVER_NUM_MAX);

                    CLIENT_TRACE_LOG("Bet number = 0x%X\n\r", betting_num);
                    bet_msg.client_bet = betting_num;

                    make_header(PROTOCOL_VERSION, BETTING_GAME_MSG_BET,
                                sizeof(send_hr) + sizeof(bet_msg),
                                receive_hdr.client_id, &send_hr);

                    if (send(sock_fd, (betting_game_common_hdr_t *) &send_hr, sizeof(send_hr), 0) == -1)
                    {
                        perror("send");
                    }

                    if (send(sock_fd, (betting_game_msg_bet_t *) &bet_msg, sizeof(bet_msg), 0) == -1)
                    {
                        perror("send");
                    }

                    CLIENT_TRACE_LOG("\n\nMsg to Server: BETTING_GAME_MSG_BET - |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d\n",
                                     send_hr.version, send_hr.type, send_hr.length, send_hr.client_id);
                    CLIENT_TRACE_LOG("Bet submitted: 0x%lX\n", bet_msg.client_bet);
                    break;
                }

                case BETTING_GAME_MSG_RESULT:
                {
                    CLIENT_TRACE_LOG("\n\nMsg from Server: BETTING_GAME_MSG_RESULT - |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d\n",
                                     receive_hdr.version, receive_hdr.type, receive_hdr.length,
                                     receive_hdr.client_id);

                    if ((recv(sock_fd, &result_msg, sizeof(result_msg), 0)) <= 0)
                    {
                        perror("recv");
                        exit(1);
                    }

                    CLIENT_TRACE_LOG("Bet Status: %d | Winning number:0x%lX\n",
                                     result_msg.result_status, result_msg.winning_number);

                    if (result_msg.result_status == 1)
                    {
                        CLIENT_TRACE_LOG("This client won the bet\n");
                    }
                    else
                    {
                        CLIENT_TRACE_LOG("This client lost the bet\n");
                    }

                    result_received = 1;
                    break;
                }
            }

            if (result_received == 1) break;
        }
    }

    close(sock_fd);

    return 0;
}


