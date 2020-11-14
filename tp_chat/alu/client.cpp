#include "header.h"
using namespace std;
#define PORT 5300
#define MAX_MSG_LENGTH 1024

int connection_setup(){

    int socket_fd;
    struct sockaddr_in  remote;
    
    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Creando socket");
        exit(1);
    }
    
    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT);
    
    inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr)); // Guarda la ip en remote.sin_addr como binario

    if (connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
        perror("Error conectandose");
        exit(1);
    }

    return socket_fd;
}


void rcvFromServer(int socket)
{
    char buf [1024];
    int n;
    
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        n = recv(socket, buf, sizeof(buf), 0);
        cout << buf << endl;

        if(n<0)
        {
            perror("Error enviando al servidor");
            exit(0);
        }
    }
}


int main(void){
    
    int socket_fd = connection_setup();
    
    string msg;
    
    vector<std::thread> threads_send;
    vector<std::thread> threads_recv;

    char buf[MAX_MSG_LENGTH];
    int booleano;

    
    while(1) //While del login
    {
        cout << "Ingrese el usuario:" << endl; 
        cout << "> " ;
        
        
        getline(cin, msg);
        
        if(send(socket_fd, msg.data(), msg.length(), 0) < 0)
        {
            perror("Error enviando al servidor");
            exit(1);
        }

        recv(socket_fd, buf, sizeof(buf), 0);
        
        string string_buffer(buf);
        
        if (string_buffer != "No")
        {
            cout << "Login exitoso" << endl ; 
            break;
        }
        else
        {
            cout << "El usuario ya esta en uso, pruebe con otro" << endl;
        }
        

    }
 
    while(1) // While de envio y recibir mensajes
    {

        if(threads_recv.size()  < 1)
        {
            threads_recv.push_back(thread(rcvFromServer, socket_fd));
        }

        cout << "> " ;
        getline(cin, msg);
        
        int n;

        n = send(socket_fd, msg.data(), msg.length(), 0);

        if(n<0)
        {
            perror("Error enviando al servidor");
        }
    }
    close(socket_fd);
}
