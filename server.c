#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>

#include "cliente.c"

#define GetCurrentDir getcwd
#define PORT 8080
#define MAXCO 69
#define MAXBUFFER 10485760
#define SEPARATOR "THIS_STRING_SEPARATES"

// gcc server.c -o server.out -lpthread

//Listado de recursos solicitados
enum RecurseType {
  VIDEO,
  IMAGEP,
  IMAGEJ,
  CSS,
  FAVICON
};  

void *mainThread(void *arg);
char *getSolicitud(char* httpHeader);
void sendHTML(int cliente);
void sendIcon(int cliente);
void sendImage(char *recurso, int cliente);
void sendVideo(char *recurso, int cliente);
char* parseDir(char * recurso);
int isMP4(char* recurso);
int isPNG(char* recurso);
int getCantFrags(int largo, int tamanoFrag);
char * getFragmento(char * buff,int page, int largoArchivo, int tamanoFrag);
enum RecurseType getRecurseType(char* recurse);
void sendRecurse(char * recurse, enum RecurseType recurse_type,int client);
char* getDate();
char* getContentType(enum RecurseType recurse_type);

int chingarmeElServidor = 0;
int chingarmeHiloBuscaCamios = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]){


	//Generar el html del medio
	if(generate_html()){
		printf("Error Generando index.html%s\n","");
		exit(-1);
	}

	int socketServidor, socketTemporal;
	struct sockaddr_in direccionServidor;
	struct sockaddr_storage servidorStorage;
	socklen_t largoDireccion;

	//socket del servidor
	socketServidor = socket(PF_INET, SOCK_STREAM,0);

	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(PORT);

	bind(socketServidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor));

	listen(socketServidor, MAXCO);

	while(1){
		largoDireccion = sizeof(servidorStorage);
		socketTemporal = accept(socketServidor, (struct sockaddr*)&servidorStorage, &largoDireccion);

		//hilo para atender socket
		printf("%s\n","Nuevo hilo");
		pthread_t nuevoHilo;
		pthread_create(&nuevoHilo, NULL, &mainThread, (void *)&socketTemporal);

		if(chingarmeElServidor){
			break;
		}
	}
	return 0;
}

void *mainThread(void *arg){
	char *buffer = (char *)malloc(4098);
	
	int webClient = *((int *)arg);

	ssize_t datosLeidos = read(webClient, buffer, 4098);
	

	if(datosLeidos<=0){
		printf("helpp\n");
		close(webClient);
		pthread_exit(NULL);
	}
	printf("DatosLeidos: %s\n",buffer);
	char *recurso = getSolicitud(buffer); //HACER FUNCION - QUE DEVUELVA LO DE ABAJO
	//printf("recurso %s\n", recurso);

	if(strcmp(recurso, "/") == 0){
		printf("sendHTML\n");
		sendHTML(webClient);
		//printf("htmlrespondido\n");
	}
	/* else if(strcmp(recurso,"/favicon.ico") == 0){
		printf("sendIcon\n");
		//sendIcon(webClient);
	} */
	else{
		enum RecurseType recurse_type;
		sendRecurse(recurso,recurse_type = getRecurseType(recurso),webClient);
	}
	//printf("Se cierra el hilo: %d\n",webClient);
	close(webClient);
	pthread_exit(NULL);
}

enum RecurseType getRecurseType(char* recurse)
{
	int i;
	for(i=0;recurse[i] != '.';i++);
	if((tolower(recurse[i+1])=='p' && tolower(recurse[i+2])=='n'))
	{
		return IMAGEP;
	}
	if((tolower(recurse[i+1])=='j' && tolower(recurse[i+2])=='p'))
	{
		return IMAGEJ;
	}
	if((tolower(recurse[i+1])=='m' && tolower(recurse[i+2])=='p'))
	{
		return VIDEO;
	}
	if((tolower(recurse[i+1])=='c' && tolower(recurse[i+2])=='s'))
	{
		return CSS;
	}
	if((tolower(recurse[i+1])=='i' && tolower(recurse[i+2])=='c'))
	{
		return FAVICON;
	}
	return 0;
}

char *getSolicitud(char *solicitud){
	printf("%s",solicitud);
	if (solicitud[0] == 'G' && solicitud[1] == 'E' && solicitud[2] == 'T') {
		int i;
		for(i = 4; solicitud[i] != ' '; i++);
		if (i == 4){
			char *recurse = {0};
			return recurse;
		}
		int largo = i - 4;
		char *recurse = (char *)malloc(largo + 1);
		int j=0;
		for(int k = 4; k < i; k++){
			recurse[j] = solicitud[k];
			j++;
		}
		recurse[largo] = 0;
		return recurse;
	} else {
		return 0;
	}
}



void sendHTML(int cliente){
	
	pthread_mutex_lock(&mutex);
	write(cliente, htmlHeader, strlen(htmlHeader));
	htmlLargo = strlen(html);
	write(cliente,html,htmlLargo);
	pthread_mutex_unlock(&mutex);

}

char* parseDir(char * recurso){
	char cwd[FILENAME_MAX];
	GetCurrentDir( cwd, FILENAME_MAX );
	char * result = (char*)malloc(strlen(cwd)+strlen(recurso)+1);
	strcpy(result,cwd);
	strcat(result,recurso);
	return result;
}

void sendRecurse(char * recurse, enum RecurseType recurse_type,int client)
{
	//declaracion de datos
	//datos de los fragmentos
	int recurse_fragments,recurse_fragment_start,i,total_bytes_send,partial_content;
	size_t recurse_size , recurse_fragment_size;

	//datos del header 
	char *date;
	char *content_type;

	//instanciacion de datos
	printf("recurso %s\n", recurse);
	char *recurse_buffer = (char *)malloc( MAXBUFFER );
	char *recurse_path = parseDir(recurse);
	FILE *recurse_file = fopen(recurse_path,"rb");
	
	fseek(recurse_file,0,SEEK_END);
	recurse_size = (size_t)ftell(recurse_file);
	fseek(recurse_file,0,SEEK_SET);

	recurse_fragments = recurse_size/(MAXBUFFER);

	char recurse_header[2018];
	total_bytes_send = 0;
	date = getDate();
	content_type = getContentType(recurse_type);
	printf("Cantidad de paquetes %d\n",recurse_fragments);
	for(i=0;i < recurse_fragments;i++)
	{
		recurse_fragment_start = (size_t)ftell(recurse_file);
		if((recurse_size-recurse_fragment_start) < MAXBUFFER)
		{
			recurse_fragment_size = recurse_size-recurse_fragment_start;
		}
		else
		{
			recurse_fragment_size = MAXBUFFER;
		}
		printf("recurse_fragment_size: %ld, recurse_file: %ld, total bytes enviados: %d\n",recurse_fragment_size,(size_t)ftell(recurse_file),total_bytes_send);
		size_t result = fread(recurse_buffer, 1, recurse_fragment_size , recurse_file);
		total_bytes_send += recurse_fragment_size;
		printf("Envio de video. Paquete %d/%d, tam frag: %ld, tam total: %ld, inicio: %d, leidos: %d\n",i+1,recurse_fragments,recurse_fragment_size,recurse_size,recurse_fragment_start,total_bytes_send);
		
		if(partial_content)
		{
			if(partial_content == 1)
			{//Content-type: multipart/byteranges; boundary=THIS_STRING_SEPARATES
				
				sprintf(recurse_header,
					"HTTP/1,1 206 Partial content\r\n"
					"Date: %s\r\n"
					"Server: localhost:8080\r\n"
					"Accept-Ranges: bytes\r\n"
					"Access-Control-Allow-Origin: \"*\"\r\n"
					"Content-type: multipart/byteranges; boundary=%s\r\n"
					"X-Content-Type-Options: nosniff\r\n"
					"X-Xss-Protection: 1; mode=block\r\n"
					"\r\n"
					"%s\r\n", date,SEPARATOR,SEPARATOR); 				
				partial_content += 1;
			}

			sprintf(recurse_header,
				"Content-Type: %s\r\n"
				"Content-Length: %ld\r\n"
				"Content-Range: bytes %d-%ld/%ld\r\n"
				"\r\n",content_type,recurse_fragment_size,recurse_fragment_start,(recurse_fragment_start+recurse_fragment_size),recurse_size); 
		}
		else
		{
			//creacion de header del recurso
			sprintf(recurse_header,
				"HTTP/1,1 206 Partial content\r\n"
				"Date: %s\r\n"
				"Server: localhost:8080\r\n"
				"Accept-Ranges: bytes\r\n"
				"Access-Control-Allow-Origin: \"*\"\r\n"
				"Content-Type: %s\r\n"
				"Content-Length: %ld\r\n"
				"Content-Range: bytes %d-%ld/%ld\r\n"
				"X-Content-Type-Options: nosniff\r\n"
				"X-Xss-Protection: 1; mode=block\r\n"
				"\r\n", date,content_type,recurse_fragment_size,recurse_fragment_start,(recurse_fragment_start+recurse_fragment_size),recurse_size); 
		}			
		write(client, recurse_header, strlen(recurse_header));
		write(client, recurse_buffer, recurse_fragment_size);
		
		if(partial_content == 1 && i+1 != recurse_fragments)
		{
			sprintf(recurse_header,
				"--%s\r\n"
				"\r\n",SEPARATOR);
			write(client, recurse_header, strlen(recurse_header)); 
		
		}		
	}
	if(partial_content == 1)
	{
		sprintf(recurse_header,
			"--%s--\r\n",SEPARATOR);
		write(client, recurse_header, strlen(recurse_header));
	}
	fclose(recurse_file);
}

char* getDate() {
  char* date = (char *)malloc( 512 );
  char tmp[512];
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(tmp, sizeof (tmp), "%a, %d %b %Y %H:%M:%S %Z", &tm);
  strcpy(date,tmp);
  return date;
}

char* getContentType(enum RecurseType recurse_type)
{	
	char* content_type = (char *)malloc( 512 );
	switch (recurse_type)
	{
		case IMAGEP:
			strcpy(content_type,"image/png");
			break;
		case IMAGEJ:
			strcpy(content_type,"image/jpg");
			break;
		case VIDEO:
			strcpy(content_type,"video/mp4");
			break;
		case CSS:
			strcpy(content_type,"text/css");
			break;
		case FAVICON:
			strcpy(content_type,"image/png");
			break;
		default:
			break;
	}
	return content_type;
}
/* 
void sendVideo(char *recurso, int cliente){
	printf("VIDEOrecurso %s\n", recurso);
	char *direccion = parseDir(recurso);
	FILE *image = fopen(direccion,"rb");
	fseek(image,0,SEEK_END);
	size_t largoArchivo = (size_t)ftell(image);
	fseek(image,0,SEEK_SET);


	char *sendbuf = (char *)malloc( largoArchivo + 1);
	size_t result = fread(sendbuf, 1, largoArchivo , image);
	fclose(image);

	//creacion de header image
	char header[2018];
	sprintf(header,
		"HTTP/1,1 200 OK\r\n"
		"Server: localhost:8080\r\n"
		"accept-ranges: bytes\r\n"
		"access-control-allow-origin: \"*\"\r\n"
		"content-type: video/mp4\r\n"
		"x-content-type-options: nosniff\r\n"
		"x-xss-protection: 1; mode=block\r\n"
		"content-length: %ld\r\n"
		"\r\n", largoArchivo); 

	int i=0;
	write(cliente, header, strlen(header));
	write(cliente,sendbuf,largoArchivo);
	int cantPages = getCantFrags(largoArchivo,8000000);
	printf("CANTIDAD DE PAGINAS %d\n", cantPages);
	while(i<=cantPages){
		char * help = getFragmento(sendbuf,i,largoArchivo,8000000);
		write(cliente, help, largoArchivo);
		i++;
	}
}

char * getFragmento(char * buff,int page, int largoArchivo, int tamanoFrag){
	int inicio = page * tamanoFrag;
	int fin;
	if((page+1)*tamanoFrag>largoArchivo){
		fin=largoArchivo;
	}
	else{
		fin=(page+1)*tamanoFrag;
	}
	char * res = (char *)malloc( fin-inicio );
	printf("NUMERO DE PAG: %d\n", page);
	printf("FRAGMENTO INICIO: %d\n", inicio);
	while(inicio<fin ){
		res[inicio-(page*tamanoFrag)] = buff[inicio];
		inicio++;
	}
	printf("FRAGMENTO FIN: %d\n", fin);
	return res;
}
int getCantFrags(int largo, int tamanoFrag){
	int completas = largo/tamanoFrag;
	if(largo%tamanoFrag>0){
		completas++;
	}
	completas = completas - 1;
	return completas;
}
void sendImage(char *recurso, int cliente){
	printf("recurso %s\n", recurso);
	char *direccion = parseDir(recurso);
	FILE *image = fopen(direccion,"rb");
	fseek(image,0,SEEK_END);
	size_t largoArchivo = (size_t)ftell(image);
	fseek(image,0,SEEK_SET);


	char *sendbuf = (char *)malloc( largoArchivo + 1);
	size_t result = fread(sendbuf, 1, largoArchivo , image);
	fclose(image);

	//creacion de header image
	char header[2018];
	sprintf(header,
		"HTTP/1,1 200 OK\r\n"
		"Server: localhost:8080\r\n"
		"accept-ranges: bytes\r\n"
		"access-control-allow-origin: \r\n"
		"content-type: image/png\r\n"
		"x-content-type-options: nosniff\r\n"
		"x-xss-protection: 1; mode=block\r\n"
		"content-length: %ld\r\n"
		"\r\n", largoArchivo); 
	
	write(cliente, header, strlen(header));
	write(cliente, sendbuf, largoArchivo);

}

void sendIcon(int cliente){
	printf("respondio icono\n");
	sendImage("/bienvenidos.png", cliente);

}

int isPNG(char* recurse){
	int i;
	for(i=0;recurse[i] != '.';i++);
	if(recurse[i+1]=='p' && recurse[i+2]=='n')
		return 1;
	return 0;
}
int isMP4(char* recurse){
	int i;
	for(i=0;recurse[i] != '.';i++);
	if(recurse[i+1]=='m' && recurse[i+2]=='p')
		return 1;
	return 0;
}
int isCSS(char* recurse)
{

} */