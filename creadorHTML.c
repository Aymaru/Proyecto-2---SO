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

int main() 
{
    //Cantidad de archivos
    int size = 100000;
    //Parecerse a este:['a','b', 'c', 'd','e']
    char stringImagenes[size];
    //Parecerse a este:[['caso1','A1','25','10/20/2019 - 20:20'],
                     //['caso2','A2','26','11/20/2019 - 21:20'],
                     //['caso3','A3','27','12/20/2019 - 22:20'],
                     //['caso4','A4!!','28','13/20/2019 - 23:20']]
    char stringInfo[size][4];
    //Parecerse a este:['a','b', 'c', 'd','e']
    char stringVideos[size];
    char indexHTML[100000]= "<!DOCTYPE html><html><head><title>OSTube</title><link rel=\"stylesheet\" type=\"text/css\" href=\"https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\"><link rel=\"stylesheet\" href=\"estilos.css\"><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" ></script><script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script> </head><body><h1>OSTube</h1><p id=\"infoGalaria\">Galeria de Videos</p><div id=\"slider\"><div id=\"box\"><img src=\"bienvenidos.png\"></div><button class=\"prew fa fa-chevron-left\" onclick=\"prewImage()\"></button><button class=\"next fa fa-chevron-right\" onclick=\"nextImage()\"></button></div><button onclick=\"myFunction()\" class=\"button\" data-toggle=\"modal\" data-target=\"#exampleModalCenter\">Ver video</button><script type=\"text/javascript\"> var slider_content = document.getElementById('box');var image = [";
    //Insertamos los nombres de las imagenes: 'a','b','c'
    
    
    strcat(indexHTML, "];var imageInfo = [");
    //Insertamos la informacion de los videos: ['caso1','A1','25','10/20/2019 - 20:20'],['caso2','A2','26','11/20/2019 - 21:20'],['caso3','A3','27','12/20/2019 - 22:20'],['caso4','A4!!','28','13/20/2019 - 23:20']]

    strcat(indexHTML, "];var i = image.length;function nextImage(){if (i<image.length) {i= i+1;}else{i = 1;}slider_content.innerHTML = \"<img src=\"+image[i-1]+\".jpg data-toggle=\"modal\" data-target=\"#exampleModalCenter\"><hr width=\"80%\"><h2>Información del Video:</h2> <ul><li><p>Nombre : \"+imageInfo[i - 1] [0]+\"</p></li><li><p>Descripción : \"+imageInfo[i - 1] [1]+\"</p></li><li><p>Tamaño (Bytes) : \"+imageInfo[i - 1] [2]+\"</p></li><li><p>Creación : \"+imageInfo[i - 1] [3]+\"</p></li></ul>\";}function prewImage(){if (i<image.length+1 && i>1) {i= i-1;}else{i = image.length;}slider_content.innerHTML = \"<img src=\"+image[i-1]+\".jpg data-toggle=\"modal\" data-target=\"#exampleModalCenter\"><hr width=\"80%\"><h2>Información del Video:</h2><ul><li><p>Nombre : \"+imageInfo[i - 1] [0]+\"</p></li><li><p>Descripción : \"+imageInfo[i - 1][1]+\"</p></li><li><p>Tamaño (Bytes) : \"+imageInfo[i - 1] [2]+\"</p></li><li><p>Creación : \"+imageInfo[i - 1] [3]+\"</p></li></ul>\";}setInterval(nextImage , 600000);function myFunction() {var r = confirm(\"¿Desea ver el video?\"); if (r == true) {slider_content.innerHTML = \"<div class=\"modal fade\" data-keyboard=\"false\" id=\"exampleModalCenter\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"exampleModalCenterTitle\" aria-hidden=\"true\"><div class=\"modal-dialog modal-dialog-centered\" role=\"document\"><div class=\"modal-content\"><div class=\"modal-header\"><h5 class=\"modal-title\" id=\"exampleModalLongTitle\">Reproduciendo Video</h5><button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>      </div><div class=\"modal-body\"><video height=\"890\"  width=\"1080\" controls autoplay loop id=\"videoDinamico\"><source src=\"\"+image[i-1]+\".mp4\" type=\"video/mp4\"></video></div><div class=\"modal-footer\"><button onclick=\"cerrar()\" type=\"button\" class=\"cerrado\" data-dismiss=\"modal\">Cerrar</div> </div> </div></div>\";} else {location.reload();}}function cerrar() {location.reload();}</script><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\"></script></body></html>");
    
    printf(indexHTML);
    /*
    FILE *fp;
    fp= fopen("index.html","w");
    if (fp == NULL){fputs(indexHTML);exit(1);}
    fclose(fp);
    */
    return 0;
    
}