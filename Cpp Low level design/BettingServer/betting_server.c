
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "betting_game_common.h"

#define ERR_NO_NUM -1
#define ERR_NO_MEM -2

static betting_game_client_t betting_client[BETSERVER_NUM_CLIENTS];
static int total_clients;   /* number of active clients */
static fd_set read_fds;     /* socket fd master list */

static unsigned int
gen_winning_number(
    unsigned int min,
    unsigned int max)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    srand((unsigned)tv.tv_usec);

    return (rand() % (max - min + 1) + min);
}

static void
close_connection(int client_id)
{
    SERVER_TRACE_LOG("Close connection client_id: %d fd:%d\n",
                      client_id, betting_client[client_id].sockfd);

    FD_CLR(betting_client[client_id].sockfd, &read_fds);
    close(betting_client[client_id].sockfd);
    betting_client[client_id].sockfd = 0;
    betting_client[client_id].state = BETTING_GAME_MSG_NONE;
    total_clients--;
}

int
main(
    int argc,
    char *argv[])
{
    int opt = TRUE;
    int master_socket = 0, addr_len = 0, new_socket = 0;
    int max_clients = BETSERVER_NUM_CLIENTS, activity = 0, i = 0, valread = 0;
    int sock_desc = 0, max_sd = 0;
    struct sockaddr_in server_addr;
    struct timeval timeout;
    unsigned long winning_number = 0;

    /* create a master socket */
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Master socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* set master socket to allow multiple connections */
    /* setsockopt is used for losing the pesky “Address already in use” error message */
    if (setsockopt(master_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /* preparation of the socket server_addr */

    /* Configure settings of the server server_addr struct */
    /* Address family = Internet */
    server_addr.sin_family = AF_INET;

    /* Set IP server_addr to localhost */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Set port number, using htons function to use proper byte order */
    server_addr.sin_port = htons(SERV_PORT);

    /* Set all bits of the padding field to 0 */
    memset(server_addr.sin_zero, 0, sizeof server_addr.sin_zero);

    /* bind the socket to localhost port SERV_PORT */
    if (bind(master_socket,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind the socket to the port failed");
        exit(EXIT_FAILURE);
    }

    SERVER_TRACE_LOG("Betting server listening on the port: %d \n", SERV_PORT);

    /* Total no. of client connection: BETSERVER_NUM_CLIENTS, allowed on the incoming queue */
    if (listen(master_socket, BETSERVER_NUM_CLIENTS) == SOCKET_ERROR)
    {
        perror("listen call");
        exit(errno);
    }

    addr_len = sizeof(server_addr);

    winning_number = gen_winning_number(BETSERVER_NUM_MIN,
                                        BETSERVER_NUM_MAX);

    /* Clear the socket set */
    FD_ZERO(&read_fds);

    /* Add master socket to set */
    FD_SET(master_socket, &read_fds);
    max_sd = master_socket;

    betting_game_result_t     res_msg;
    betting_game_msg_accept_t accept_msg;
    betting_game_msg_bet_t    bet_msg;
    betting_game_common_hdr_t send_hdr;
    betting_game_common_hdr_t recv_hdr;

    /* Run the betting server forever and detect activity in sockets */
    while (1)
    {
        /* Clear the socket set */
        FD_ZERO(&read_fds);

        memset(&res_msg, 0, sizeof res_msg);
        memset(&accept_msg, 0, sizeof accept_msg);
        memset(&bet_msg, 0, sizeof bet_msg);
        memset(&send_hdr, 0, sizeof send_hdr);
        memset(&recv_hdr, 0, sizeof recv_hdr);

        /* Add master socket to set */
        FD_SET(master_socket, &read_fds);
        max_sd = master_socket;

        /* Add child sockets to set */
        for (i = 0; i < max_clients; i++)
        {
            /* socket descriptor */
            sock_desc = betting_client[i].sockfd;

            /* if client socket descriptor is valid, then add to read list */
            if (sock_desc > 0)
            {
                FD_SET(sock_desc, &read_fds);
            }

            /* highest file descriptor number, need it for the select function */
            if (sock_desc > max_sd)
            {
                max_sd = sock_desc;
            }
        }

        timeout.tv_sec = 15;
        timeout.tv_usec = 0;

        /* Bock until input is ready on a specified set of file descriptors, or
         * until a timer expires, whichever comes first
         */

        activity = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);

        if ((activity < 0) && (errno != EINTR))
        {
            SERVER_TRACE_LOG("select system call error - close all active connections\n");
            perror("select");

            for (i = 0; i < BETSERVER_NUM_CLIENTS; i++)
            {
                close_connection(sock_desc);
            }
        }
        else if (activity == 0) /*  Whenever the timeout expires */
        {
            for (i = 0; i < max_clients; i++)
            {
                if ((sock_desc = betting_client[i].sockfd) &&
                    (i != master_socket))
                {
                    if (betting_client[sock_desc].betting_num == -1)
                    {
                        close_connection(betting_client[sock_desc].sockfd);
                    }

                    SERVER_TRACE_LOG("Sending Result message: 0x%lX\n", winning_number);
                    res_msg.winning_number = winning_number;

                    if (betting_client[sock_desc].state == BETTING_GAME_MSG_BET)
                    {
                        if (winning_number == betting_client[sock_desc].betting_num)
                        {
                            /* if the submitted bet is equal to the winning number */
                            res_msg.result_status = 1;
                        }
                        else
                        {
                            res_msg.result_status = 0;
                        }

                        /* BETTING_GAME_MSG_RESULT message is sent to all the connected clients */
                        make_header(PROTOCOL_VERSION, BETTING_GAME_MSG_RESULT,
                                    sizeof(send_hdr) + sizeof(res_msg),
                                    sock_desc,
                                    &send_hdr);

                        if (send(sock_desc,
                                 (betting_game_common_hdr_t *)&send_hdr,
                                  sizeof(send_hdr), 0) == -1)
                        {
                            perror("Send error Common header of Result message\n\r");
                        }

                        if (send(sock_desc, (betting_game_result_t *)&res_msg, sizeof(res_msg), 0) == -1)
                        {
                            perror("Send error Result Message\n\r");
                        }

                        SERVER_TRACE_LOG("\nMsg to Client: BETTING_GAME_MSG_RESULT - |Version = %2u | Pkt Type = %2u | Pkt Length = %d | ClientID = %d |\n",
                                         send_hdr.version, send_hdr.type, send_hdr.length, sock_desc);
                    }

                    SERVER_TRACE_LOG("Bet Status: %d | Winning number: 0x%lX\n",
                           res_msg.result_status, res_msg.winning_number);
                }
            }

            winning_number = gen_winning_number(BETSERVER_NUM_MIN,
                                                BETSERVER_NUM_MAX);
        }

        /* If something happened on the master socket , */
        /* then its an incoming connection */
        if (FD_ISSET(master_socket, &read_fds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&server_addr,
                                     (socklen_t *)&addr_len)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            SERVER_TRACE_LOG("New Client Connected - Socket fd: %d, IP: %s, Port: %d\n",
                   new_socket, inet_ntoa(server_addr.sin_addr),
                   ntohs(server_addr.sin_port));

            /* add new socket to array of sockets */
            for (i = 0; i < max_clients; i++)
            {
                /* if position is empty */
                if (betting_client[i].sockfd == 0)
                {
                    betting_client[i].sockfd = new_socket;
                    SERVER_TRACE_LOG("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        /* else its some IO operation on some socket */
        for (i = 0; i < max_clients; i++)
        {
            sock_desc = betting_client[i].sockfd;

            if (FD_ISSET(sock_desc, &read_fds))
            {
                /* Check if it was for closing , and also read the incoming message */
                if ((valread = read(sock_desc, &recv_hdr, sizeof(recv_hdr))) == 0)
                {
                    /* Somebody disconnected , get the client details and display */
                    getpeername(sock_desc, (struct sockaddr *)&server_addr,
                                (socklen_t *)&addr_len);

                    SERVER_TRACE_LOG("Host disconnected: IP:%s, Port:%d\n",
                                      inet_ntoa(server_addr.sin_addr),
                                      ntohs(server_addr.sin_port));

                    /* Close the socket and mark as 0 in list for reuse */
                    close(sock_desc);
                    betting_client[i].sockfd = 0;
                }
                else /* Echo back the message that came in */
                {
                    switch (recv_hdr.type)
                    {
                        case BETTING_GAME_MSG_OPEN:
                        {
                            SERVER_TRACE_LOG("\n\nMsg from Client: BETTING_GAME_MSG_OPEN |Version = %2u | Length = %2u | Type = %d | ClientID = %d |\n",
                                   recv_hdr.version, recv_hdr.length, recv_hdr.type, recv_hdr.client_id);

                            accept_msg.lower_end_of_number = BETSERVER_NUM_MIN;
                            accept_msg.upper_end_of_num = BETSERVER_NUM_MAX;

                            betting_client[sock_desc].state = BETTING_GAME_MSG_ACCEPT;
                            total_clients++;

                            /* Header for BETTING_GAME_MSG_ACCEPT is created */
                            make_header(PROTOCOL_VERSION,
                                        BETTING_GAME_MSG_ACCEPT,
                                        sizeof(send_hdr) + sizeof(accept_msg),
                                        sock_desc,
                                        &send_hdr);

                            /* Header for the BETTING_GAME_MSG_ACCEPT is sent to the client */
                            if (send(sock_desc, (betting_game_common_hdr_t *)&send_hdr,
                                          sizeof(send_hdr), 0) == -1)
                            {
                                perror("Error in sending the message to the client");
                            }

                            /* In response BETTING_GAME_MSG_ACCEPT is sent to the client */
                            if (send(sock_desc, (betting_game_msg_accept_t *)&accept_msg,
                                     sizeof(accept_msg), 0) == -1)
                            {
                                perror("Error in sending the message to the client");
                            }

                            SERVER_TRACE_LOG("\n\nMsg to client: BETTING_GAME_MSG_ACCEPT - |Version = %2u | Pkt Type = %2u | Pkt Length = %d | ClientID = %d |\n",
                                   send_hdr.version, send_hdr.type, send_hdr.length, send_hdr.client_id);

                            SERVER_TRACE_LOG("Minimum Limit: 0x%lX | Maximum Limit: 0x%lX\n",
                                   accept_msg.lower_end_of_number, accept_msg.upper_end_of_num);
                            break;
                        }

                        case BETTING_GAME_MSG_BET:
                        {
                            SERVER_TRACE_LOG("\nMsg from Client: BETTING_GAME_MSG_BET - |Version = %2u | Length = %2u | Type = %d | ClientID = %d |\n",
                                   recv_hdr.version, recv_hdr.length, recv_hdr.type, recv_hdr.client_id);

                            if ((recv(sock_desc, &bet_msg, sizeof(bet_msg), 0)) <= 0)
                            {
                                perror("recv");
                                exit(1);
                            }

                            SERVER_TRACE_LOG("Bet received: 0x%lX\n", bet_msg.client_bet);

                            if (betting_client[sock_desc].state == BETTING_GAME_MSG_ACCEPT)
                            {
                                /* Check whether the client bet is within the specified limit
                                 *  else close the socket
                                 */
                                if (bet_msg.client_bet <= BETSERVER_NUM_MIN ||
                                    bet_msg.client_bet >= BETSERVER_NUM_MAX)
                                {
                                    SERVER_TRACE_LOG("Received Bet was not within the limit\n");
                                    betting_client[sock_desc].betting_num = -1;
                                    close_connection(betting_client[sock_desc].sockfd);
                                }
                                else
                                {
                                    SERVER_TRACE_LOG("Received a valid bet from the client\n");
                                    betting_client[sock_desc].betting_num = bet_msg.client_bet;
                                    betting_client[sock_desc].state = BETTING_GAME_MSG_BET;
                                }
                            }
                            break;
                        }

                        default:
                            SERVER_TRACE_LOG("Invalid Type received!\n\r");
                            close_connection(betting_client[sock_desc].sockfd);
                            break;
                    }
                }

                SERVER_TRACE_LOG("\n\n");
            }
        }
    }

    return 0;
}


