#include "header.h"
using namespace std;

#define MAX_MSG_LENGTH 1024
#define PORT 5300


void addClient(int s, const string& nickname, vector<Client> &listaClientes) //Crea un nuevo cliente, modifica sus campos nombre y socket lo agrega a la lista clientes 
{
    Client cliente;
    cliente.nombre = nickname;
    cliente.socket = s;
    listaClientes.push_back(cliente);
}

void deleteClient(Client& c) //Dado un cliente, lo elimina del sistema y cierra su socket adecuadamanete(ver shutdown())
{
   //Falta
}


Client* getClient(const string& nick, vector<Client> &listaClientes) // Dado un nick, devuelve un puntero al cliente encontrado con dicho nickname. En caso de no existir, el puntero es null
{
    cout << "Cantidad de clientes en la lista: " << listaClientes.size() << endl;
    
    for(int i=0; i < listaClientes.size(); i++)
    {
        cout << listaClientes[i].nombre << endl ;
        
        if(nick == listaClientes[i].nombre) 
        {
            return &listaClientes[i]; 
        }    
    }
    return NULL;
}


void sendToClient(Client* c, const string& msg) // Dado un cliente y un mensaje, envía dicho mensaje a traves del socket asociado al cliente
{
    send(c->socket, msg.data(), msg.length(), 0);
}



void connection_handler(int socketConectado, vector<Client> &listaClientes) //Funcion que ejecutan los threads
{    
    
    char buf[MAX_MSG_LENGTH]; //Buffer para los strings recibidos
    int n; //Guarda la cantidad de bytes del recv
    vector<std::thread> threads_mensajes; // Vector de los threads usados para el reenvio de mensajes
    
    while(1)
    {
        n = recv(socketConectado, buf, sizeof(buf), 0);
        string string_buffer(buf); // Pasamos a string el buffer
        
        cout << "1. Soy el string que recibe el servidor = " << string_buffer << endl;

        if(getClient(string_buffer, listaClientes))
        {   
            cout << "2. Entre a la condicion de login incorrecto" << endl;
            send(socketConectado, "No" , string_buffer.length(), 0);
        }
        else
        {
            cout << "2. Entre a la condicion de login correcto" << endl;
            addClient(socketConectado, string_buffer, listaClientes);
            send(socketConectado, "Si" , string_buffer.length(), 0);
            break;
        }   
    }        

    while(1)
    {
        Client usuario;
        string string_mensaje;
        
        n = recv(socketConectado, buf, sizeof(buf), 0); //Recibe lo que manda el cliente

        if (n == 0)
        {
            close(socketConectado);
        }
        if (n < 0) 
        { 
            perror("Error recibiendo");
        }
        
        buf[n] = '\0';

        // REENVIO DE MENSAJE

        for(int i = 0; i < listaClientes.size(); i++) //Determinamos quien es el usuario a partir de su socket
        {
            if(listaClientes[i].socket == socketConectado)
            {   
                usuario = listaClientes[i];
            }
        }
        
        cout  << "Recibi: " << buf << ",  Usuario: " << usuario.nombre << ", Socket: " << socketConectado << endl; 


        if(listaClientes.size() >= 2) // Verificamos que haya otro cliente al cual reenviar para evitar errores
        {
            for(int x = 0; x < listaClientes.size(); x++)
            {
                if (listaClientes[x].nombre != usuario.nombre)
                {
                    string string_auxiliar(buf);
                    string_mensaje = usuario.nombre + ": " + string_auxiliar; 
                    
                    cout << "Soy el string mensaje que envio : " << string_mensaje << endl;
                    // cout << "String mensaje data  " <<  string_mensaje.data() << endl;
                    // cout << "String mensaje length  " <<  string_mensaje.length() << endl;
                    // cout << "Socket  " <<  listaClientes[x].socket << endl;
                    
                    Client clienteReenvio = listaClientes[x];
                    
                    threads_mensajes.push_back((thread(sendToClient, &clienteReenvio , string_mensaje)));
                }
            }
        }
        
        
        // cout << "Antes del join " << endl;
        // cout << "Lista cliente size: " << listaClientes.size() << endl;
        // cout << "Size threads mensajes: " << sizeof(threads_mensajes)/sizeof(threads_mensajes[0]) << endl;

        if(listaClientes.size() >= 2)
        {
            for(int i = 0 ; i < listaClientes.size(); i++)
            {	

                if(threads_mensajes[i].joinable())
                {
                    threads_mensajes[i].join();
                }
            }	
        }
            
            


        /* Parsear el buffer recibido*/
        /* COMPLETAR */

        /* Detectar el tipo de mensaje (crudo(solo texto) o comando interno(/..),
           y ejecutar la funcion correspondiente segun el caso */
        /* COMPLETAR */

}
} 



int connection_setup(struct sockaddr_in local){
    
    int listening_socket;
    int s;
    
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creando socket");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(PORT);
    local.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0) {
        perror("Error haciendo bind");
        exit(1);
    }
    
    if (listen(s, 5) == -1) {
        perror("Error escuchando");
        exit(1);
    }
    
    listening_socket = s;
    
    printf("Server is on! \n");

    return listening_socket;
}

int accept_connection(int socketListening, struct sockaddr_in remote)
{
    int socketConectado;
    if ((socketConectado = accept(socketListening, NULL, NULL)) == -1) {
    perror("Error aceptando conexión");
    exit(1);
    }
    return socketConectado;
}

int main(void)
{
    struct sockaddr_in local; // Direccion local
    struct sockaddr_in remote; // Direccion del host remoto
    

    int nthreads = std::thread::hardware_concurrency();
    std::thread threads[nthreads];


    vector<Client> listaClientes;
    vector<int> vector;

    int i = 0;

    int socketListening = connection_setup(local); // Socket para conexiones entrantes
    cout << "Socket Listening = " << socketListening << endl;

    struct Client cliente;

    while(1) { 
        int socketConectado = accept_connection(socketListening, remote);
        
        cout << "Socket Conectado = " << socketConectado << endl;

        
        threads[i] = thread(connection_handler, socketConectado, ref(listaClientes));
     
        i++;


    }
    for(int i = 0 ; i < nthreads; i++)
    {	
        threads[i].join();
    }	
    /* COMPLETAR */

    /* Cerramos las conexiones pendientes. */
    /* COMPLETAR */
  
    /* Cerramos la conexión que escucha. */

    return 0;
}

