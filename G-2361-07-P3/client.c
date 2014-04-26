#include <stdio.h>
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
 
#define RETURN_NULL(x) if ((x)==NULL) exit (1)
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(1); }
 
#define RSA_CLIENT_CERT       "crt/clientcert.pem"
#define RSA_CLIENT_KEY        "crt/clientkey.pem"
 
#define RSA_CLIENT_CA_CERT      "crt/rootcert.pem"
 
void main() {

    int     err;
    int     sock;
    struct sockaddr_in server_addr;
    char  *str;
    char    buf [4096];
    char    hello[80];

    SSL_CTX         *ctx;
    SSL             *ssl;
    SSL_METHOD      *meth;
    X509            *server_cert;
    EVP_PKEY        *pkey;

    short int       s_port = 5555;
    const char      *s_ipaddr = "127.0.0.1";

    printf ("Escriba un mensaje al servidor: ");
    fgets (hello, 80, stdin);

    /* inicializar la librería SSL y registrar los métodos de cifrado soportados */
    SSL_library_init();

    /* cargar mensajes de error que serán usados */
    SSL_load_error_strings();

    /* añade soporte para las versión SSL 2 y 3 */
    meth = SSLv3_method();

    /* crea un nuevo contexto para la utilización de la capa SSL */
    ctx = SSL_CTX_new(meth);                        

    RETURN_NULL(ctx);

    /* especificar el certificado que utilizará nuestra aplicación */
    if (SSL_CTX_use_certificate_file(ctx, RSA_CLIENT_CERT, 

        SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /*  clave privada de nuestra aplicación */
    if (SSL_CTX_use_PrivateKey_file(ctx, RSA_CLIENT_KEY, 
        SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* verifica si la clave esta asociada al certificado */
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr,"Private key does not match the certificate public key\n");
        exit(1);
    }

    /* CA utilizado para validar los certificados recibidos por la aplicación  */
    if (!SSL_CTX_load_verify_locations(ctx, RSA_CLIENT_CA_CERT, NULL)) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* garantizar que se verifica la autenticidad del otro extremo */
    SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
    SSL_CTX_set_verify_depth(ctx,1);

    sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);   

    RETURN_ERR(sock, "socket");

    memset (&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(s_port);  
    server_addr.sin_addr.s_addr = inet_addr(s_ipaddr); 

    err = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)); 

    RETURN_ERR(err, "connect");

    /* crear una estructura SSL */
    ssl = SSL_new (ctx);

    RETURN_NULL(ssl);

    /* asociar la estructura SSL creada al canal de comunicación */
    SSL_set_fd(ssl, sock);
 
    /* inicializará el handshake con el servidor */
    err = SSL_connect(ssl);

    RETURN_SSL(err);

    printf ("Cifrado elegido: %s\n", SSL_get_cipher (ssl));

    /* Certificado del servidor */
    server_cert = SSL_get_peer_certificate (ssl);    
 
    if (server_cert != NULL) {
        printf ("Certificado del servidor:\n");

        str = X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
        RETURN_NULL(str);
        printf ("\t subject: %s\n", str);
        free (str);

        str = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
        RETURN_NULL(str);
        printf ("\t issuer: %s\n", str);
        free(str);

        X509_free (server_cert);
    } else {
        printf("El servidor no tiene certificado.\n");
    } 
 
    err = SSL_write(ssl, hello, strlen(hello));  

    RETURN_SSL(err);

    err = SSL_read(ssl, buf, sizeof(buf)-1);                     

    RETURN_SSL(err);
    buf[err] = '\0';
    printf ("Mensaje del servidor: %s\n", buf);
 
    /* cerrar el canal de comunicación */
    err = SSL_shutdown(ssl);
    RETURN_SSL(err);
    err = close(sock);
    RETURN_ERR(err, "close");

    /* liberar las estructuras creadas de forma dinámica */
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}