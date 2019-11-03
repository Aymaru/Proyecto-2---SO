#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

char* substr(const char *src, int m, int n){
	// get length of the destination string
	int len = n - m;

	// allocate (len + 1) chars for destination (+1 for extra null character)
	char dest = (char)malloc(sizeof(char) * (len + 1));

	// extracts characters between m'th and n'th index from source string
	// and copy them into the destination string
	for (int i = m; i < n && (*src != '\0'); i++)
	{
		*dest = *(src + i);
		dest++;
	}

	// null-terminate the destination string
	*dest = '\0';

	// return the destination string
	return dest - len;
}

char *replaceWord(const char *s, const char *oldW, const char *newW) 
{ 
    char *result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) 
    { 
        if (strstr(&s[i], oldW) == &s[i]) 
        { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) 
    { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) 
        { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
} 

int main() 
{
    char tiempoSlider[] = "100000";
    char descripcionAlbum[] = "Album de Vacaciones 2019";
    char pathImagenes [] = "/home/sebastian/Documents/Proyecto OSTube/Proyecto-2---SO/Imagenes/";
    char pathVideos [] = "/home/sebastian/Documents/Proyecto OSTube/Proyecto-2---SO/Videos/";
    struct stat fileStat;
    DIR *i;DIR *v;
    struct dirent *dir;struct dirent *dir2;
    i = opendir(pathImagenes);
    char imgName[100000];
    if (i){
        while ((dir = readdir(i)) != NULL)
        {
            if (strncmp(dir->d_name, ".", 1) == 0){}else{
                strcat(imgName,"'");
                strcat(imgName,dir->d_name);
                strcat(imgName,"'");
                strcat(imgName,",");
            }
        }
        strcpy(imgName,substr(imgName, 0, strlen(imgName)-1));
        strcpy(imgName,replaceWord(imgName, ".jpg", ""));
        //printf(imgName);
        closedir(i);
    }

    v = opendir(pathVideos);
    char videoName[100000];
    char temp[1000];

    if (v){
        while ((dir2 = readdir(v)) != NULL)
        {
            if (strncmp(dir2->d_name, ".", 1) == 0){}else{
                strcat(videoName,"[");
                strcat(videoName,"'");
                strcat(videoName,dir2->d_name);
                strcat(videoName,"'");
                strcat(videoName,",");
                strcat(videoName,"'Vacaciones'");
                strcat(videoName,",");
                strcpy(temp,pathVideos);
                strcat(temp,dir2->d_name);
                struct stat fileStat;   
                if(stat(temp,&fileStat) < 0){return 1;}    
                strcat(videoName,"'");
                int aux = fileStat.st_size;
                char aux2[100]; 
                sprintf(aux2,"%d bytes",aux);
                strcat(videoName,aux2);
                strcat(videoName,"'");
                time_t now = time(&fileStat.st_mtime);
                struct tm *info;
                info = localtime( &now );
                strcat(videoName,",");
                strcat(videoName,"'");
                strcat(videoName,ctime(&fileStat.st_mtime));
                strcat(videoName,"'");
                strcat(videoName,"],");
            }
        }
        strcpy(videoName,substr(videoName, 0, strlen(videoName)-1));
        strcpy(videoName,replaceWord(videoName, ".MP4", ""));
        strcpy(videoName,replaceWord(videoName, "\n", ""));
        //printf(videoName);
        closedir(v);
    }

    
    //Cantidad de archivos
    char indexHTML[100000]= "<!DOCTYPE html><html><head><title>OSTube</title><link rel=\"stylesheet\" type=\"text/css\" href=\"https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\"><link rel=\"stylesheet\" href=\"estilos.css\"><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" ></script><script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script> </head><body><h1>OSTube</h1><p id=\"infoGalaria\">Galeria de Videos</p><div id=\"slider\"><div id=\"box\"><img src=\"bienvenidos.png\"></div><button class=\"prew fa fa-chevron-left\" onclick=\"prewImage()\"></button><button class=\"next fa fa-chevron-right\" onclick=\"nextImage()\"></button></div><button onclick=\"myFunction()\" id=\"play\" data-toggle=\"modal\" data-target=\"#exampleModalCenter\">Ver video</button><script type=\"text/javascript\"> var slider_content = document.getElementById('box');var image = [";
    //Insertamos los nombres de las imagenes: 'a','b','c'
    strcat(indexHTML,imgName);
    
    strcat(indexHTML, "];image.sort();var imageInfo = [");
    //Insertamos la informacion de los videos: ['caso1','A1','25','10/20/2019 - 20:20'],['caso2','A2','26','11/20/2019 - 21:20'],['caso3','A3','27','12/20/2019 - 22:20'],['caso4','A4!!','28','13/20/2019 - 23:20']]
    strcat(indexHTML,videoName);
    strcat(indexHTML, "];imageInfo.sort();var i = image.length;function nextImage(){if (i<image.length) {i= i+1;}else{i = 1;}slider_content.innerHTML = \"<img src=Imagenes/\"+image[i-1]+\".jpg data-toggle=\\\"modal\\\" data-target=\\\"#exampleModalCenter\\\"><hr width=\\\"80%\\\"><h2>Información del Video:</h2><ul><li><p>Nombre : \"+imageInfo[i - 1] [0]+\"</p></li><li><p>Descripción : ");
    strcat(indexHTML,descripcionAlbum);
    strcat(indexHTML,"</p></li><li><p>Tamaño : \"+imageInfo[i - 1] [2]+\"</p></li><li><p>Creación : \"+imageInfo[i - 1] [3]+\"</p></li></ul>\";}function prewImage(){if (i<image.length+1 && i>1) {i= i-1;}else{i = image.length;}slider_content.innerHTML = \"<img src=Imagenes/\"+image[i-1]+\".jpg data-toggle=\\\"modal\\\" data-target=\\\"#exampleModalCenter\\\"><hr width=\\\"80%\\\"><h2>Información del Video:</h2><ul><li><p>Nombre : \"+imageInfo[i - 1] [0]+\"</p></li><li><p>Descripción : ");
    strcat(indexHTML,descripcionAlbum);
    strcat(indexHTML,"</p></li><li><p>Tamaño : \"+imageInfo[i - 1] [2]+\"</p></li><li><p>Creación : \"+imageInfo[i - 1] [3]+\"</p></li></ul>\";}setInterval(nextImage ,");
    strcat(indexHTML, tiempoSlider);
    strcat(indexHTML, ");function myFunction() {var r = confirm(\"¿Desea ver el video?\"); if (r == true) {slider_content.innerHTML = \"<div class=\\\"modal fade\\\" data-keyboard=\\\"false\\\" id=\\\"exampleModalCenter\\\" tabindex=\\\"-1\\\" role=\\\"dialog\\\" aria-labelledby=\\\"exampleModalCenterTitle\\\" aria-hidden=\\\"true\\\"><div class=\\\"modal-dialog modal-dialog-centered\\\" role=\\\"document\\\"><div class=\\\"modal-content\\\"><div class=\\\"modal-header\\\"><button type=\\\"button\\\" class=\\\"close\\\" data-dismiss=\\\"modal\\\" aria-label=\\\"Close\\\"><span aria-hidden=\\\"true\\\">&times;</span></button>      </div><div class=\\\"modal-body\\\"><video height=\\\"700\\\" controls autoplay loop id=\\\"videoDinamico\\\"><source src=Videos/\"+image[i-1]+\".MP4 type=\\\"video/mp4\\\"></video></div><div class=\\\"modal-footer\\\"><button onclick=\\\"cerrar()\\\" type=\\\"button\\\" class=\\\"cerrado\\\" data-dismiss=\\\"modal\\\">Cerrar</div> </div> </div></div>\";} else {location.reload();}}function cerrar() {location.reload();}</script><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\"></script></body></html>");
    
    //printf(indexHTML);
    FILE *fp;
    fp= fopen("index.html","w+");
    if (fp == NULL){}else{
        fputs(indexHTML,fp);
    }
    fclose(fp);
    
    return 0;

    
}