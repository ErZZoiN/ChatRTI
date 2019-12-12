#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <iostream>
using namespace std;

#define GROUP "225.1.1.0"
#define PORT 2401

int numero_question = 1;
int numero_reponse = 0;
int numero_event = 1;
const char* texte = "HELLO;";

char group[20];
int port;

void erreur(const char* err)
{
    puts(err);
    exit(0);
}

void menu()
{
    printf("\n1-Question");
    printf("\n2-Reponse");
    printf("\n3-Event");
    printf("\n\n");
}

DWORD WINAPI thread_reception(void* data)
{
    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData))
        erreur("WSAStartup");

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        erreur("Socket");

    u_int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0)
        erreur("Reusing ADDR failed");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*) & addr, sizeof(addr)) < 0)
        erreur("bind");

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
        erreur("setsockopt");

    while (1)
    {
        char message[200];
        char* token;
        char a[50], b[50], c[50];
        int addrlen = sizeof(addr);
        if (recvfrom(fd, message, 200, 0, (struct sockaddr*) & addr, &addrlen) < 0)
            erreur("recvfrom");

        //message[strlen(message)] = '\0';

        token = strtok(message, ";");
        printf("\nMessage recu :\n%s ", token);

        token = strtok(NULL, ";");

        printf("%s : ", token);

        token = strtok(NULL, ";");

        printf("%s\n\n", token);

        char * type = strtok(message,";");
        char tmp [200];

        memset(message, 0, 200);
        memset(a, 0, 50);
        memset(b, 0, 50);
        memset(c, 0, 50);
    }

    WSACleanup();

    return 0;
}

int main(int argc, char* argv[])
{
    char c;
    char t[10];
    int tag;
    char message[50];
    char nom[50];
    char tmp[100];
    srand(time(NULL));

    printf("Rentrez votre numero de client : ");
    fflush(stdin);
    std::cin >> nom;
    sprintf(tmp, "%s[%s] s'est connecte.\n", texte, nom);

    strcpy(group, GROUP);
    port = PORT;

    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData))
        erreur("WSAStartup");

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        erreur("Socket");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    //On défini la socket TCP pour se connecter au chat
    addr.sin_addr.s_addr = inet_addr(GROUP);
    addr.sin_port = htons(PORT);

    int choix = 0;
    HANDLE thread = CreateThread(NULL, 0, thread_reception, NULL, 0, NULL);
    if (thread)
    {
        /*if (sendto(fd, tmp, strlen(tmp), 0, (struct sockaddr*) & addr, sizeof(addr)) < 0)
            erreur("sendto");*/

        while (1)
        {
            fflush(stdin);
            menu();
            //fflush(stdin);
            cin >> c;

            if (c == '2')
            {
                cout << "Rentrez le tag de la question à laquelle vous voulez repondre :";
                cin >> t;
                tag = atoi(t);
                sprintf(nom, "REPONSE;%d;", tag);
            }
            else if (c == '1')
            {
                sprintf(nom, "QUESTION;%d;", numero_question);
                numero_question++;
            }
            else if (c == '3')
            {
                sprintf(nom, "EVENT;%d;", numero_event);
                numero_event++;
            }

            cout << "Entrez votre texte :";

            cin >> message;
            printf("%c\n", message[strlen(message) - 1]);

            strcpy(tmp, nom);
            strcat(tmp, message);

            if (sendto(fd, tmp, strlen(tmp) +1, 0, (struct sockaddr*) & addr, sizeof(addr)) < 0)
                erreur("sendto");

            memset(tmp, 0, 100);
            memset(message, 0, 50);
            int o;
            for (o = 0; o < 100000; o++);
        }
        WSACleanup();
    }
    return 0;
}

