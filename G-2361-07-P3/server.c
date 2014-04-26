#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
 
#define RSA_SERVER_CERT       "crt/servercert.pem"
#define RSA_SERVER_KEY        "crt/serverkey.pem"
 
#define RSA_SERVER_CA_CERT    "crt/rootcert.pem"
 
#define RETURN_NULL(x) if ((x)==NULL) exit(1)
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(1); }
 
void main() {

    int     err;
    int     listen_sock;
    int     sock;
    struct sockaddr_in sa_serv;
    struct sockaddr_in sa_cli;
    size_t client_len;
    char    *str;
    char     buffer[4096];
 
    SSL_CTX         *ctx;
    SSL             *ssl;
    SSL_METHOD      *meth;

    X509            *client_cert = NULL;
 
    short int       s_port = 5555;

    /* inicializar la librería SSL y registrar los métodos de cifrado soportados */
    SSL_library_init();
 
    /* cargar mensajes de error que serán usados */
    SSL_load_error_strings();
 
    /* añade soporte para las versión SSL 2 y 3 */
    meth = SSLv23_method();
 
    /* crea un nuevo contexto para la utilización de la capa SSL */
    ctx = SSL_CTX_new(meth);
 
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* especificar el certificado que utilizará nuestra aplicación */
    if (SSL_CTX_use_certificate_file(ctx, RSA_SERVER_CERT, SSL_FILETYPE_PEM) <= 0) {
        
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* clave privada de nuestra aplicación  */
    if (SSL_CTX_use_PrivateKey_file(ctx, RSA_SERVER_KEY, SSL_FILETYPE_PEM) <= 0) {
        
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* verifica si la clave esta asociada al certificado */
    if (!SSL_CTX_check_private_key(ctx)) {
 
        fprintf(stderr,"Hay un problema con el certificado y clave privada del servidor\n");
        exit(1);
    }
 
    /* CA utilizado para validar los certificados recibidos por la aplicación */
    if (!SSL_CTX_load_verify_locations(ctx, RSA_SERVER_CA_CERT, NULL)) {

        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* garantizar que se verifica la autenticidad del otro extremo */
    SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
    SSL_CTX_set_verify_depth(ctx,1);
 
    listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   
 
    RETURN_ERR(listen_sock, "socket");
    memset (&sa_serv, '\0', sizeof(sa_serv));
    sa_serv.sin_family      = AF_INET;
    sa_serv.sin_addr.s_addr = INADDR_ANY;
    sa_serv.sin_port        = htons (s_port);          
    err = bind(listen_sock, (struct sockaddr*)&sa_serv,sizeof(sa_serv));

    RETURN_ERR(err, "bind");

    err = listen(listen_sock, 5);                    

    RETURN_ERR(err, "listen");
    client_len = sizeof(sa_cli);
 
    sock = accept(listen_sock, (struct sockaddr*)&sa_cli, &client_len);
 
    RETURN_ERR(sock, "accept");
    close (listen_sock);

    char clientIpString[INET_ADDRSTRLEN];
    int clientIpInt = sa_cli.sin_addr.s_addr;
    inet_ntop( AF_INET, &clientIpInt, clientIpString, INET_ADDRSTRLEN );
 
    printf ("Conexion con %s en el puerto %x\n", 
      clientIpString, 
      sa_cli.sin_port);
 
    /* crear una estructura SSL  */
    ssl = SSL_new(ctx);
 
    RETURN_NULL(ssl);
 
    /* asociar la estructura SSL creada al canal de comunicación */
    SSL_set_fd(ssl, sock);

    /* inicializará el handshake con el servidor */
    err = SSL_accept(ssl);

    RETURN_SSL(err);

    printf("Cifrado elegido: %s\n", SSL_get_cipher (ssl));

    /* Certificado del cliente */
    client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert != NULL) {

        printf ("Certificado del cliente:\n");     
        str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
        RETURN_NULL(str);
        printf ("\t subject: %s\n", str);
        free (str);
        str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
        RETURN_NULL(str);
        printf ("\t issuer: %s\n", str);
        free (str);
        X509_free(client_cert);

    } else printf("El cliente no tiene certificado.\n");
 
    err = SSL_read(ssl, buffer, sizeof(buffer) - 1);

    RETURN_SSL(err);

    buffer[err] = '\0';

    printf ("Mensaje del cliente: %s\n", buffer);

    err = SSL_write(ssl, buffer, strlen(buffer));

    RETURN_SSL(err);

    /* cerrar el canal de comunicación */
    err = SSL_shutdown(ssl);
    RETURN_SSL(err);
    err = close(sock);
    RETURN_ERR(err, "close");

    /* liberar las estructuras creadas de forma dinámica */
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}