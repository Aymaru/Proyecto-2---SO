#include <stdio.h>
#include <dirent.h>

#define GetCurrentDir getcwd


void createMidHTML(FILE *html, DIR *dr);
int generateHTML();
int getTOPDOWNhtml();

char *htmlTOP;
char *htmlTOPDir;
char *htmlBOT;
char *htmlBOTDir;
/*char *templateVideo = 
	"<div class=\"w3-third w3-container w3-margin-bottom\">\n"
	"\t<img src=\"images/%s\"  alt=\"TONACO\" style=\"width:100%%\""
	"class=\"w3-hover-opacity\" onclick=\"\">\n"
	"\t<div class=\"w3-container w3-white\">\n"
	"\t\t<p><b>Texto descripcion</b></p>\n"
	"\t\t<p>Texto de descripcion del archivo</p>\n"
	"\t\t<p>Tamano: en bytes</p>\n"
	"\t\t<p>Fecha y hora del archivo</p>\n"
	"\t</div>\n"
	"</div>\n";*/
char *htmlHeader = 
	"HTTP/1.1 403 Forbidden\r\n"
	"Server: localHost:8080\r\n"
	"Content-Type: text/html; charset=iso-8859-1\r\n"
	"\r\n";
char *html;
size_t htmlLargo = 0;
char *checksum;

int generateHTML(){

	char cwd[FILENAME_MAX];
	GetCurrentDir( cwd, FILENAME_MAX );
	strcat(cwd, "/index.html");
	printf("path: %s",cwd);
	FILE * htmlFile = fopen(cwd, "r");

	if(htmlFile == 0){
		printf("No se pudo encontrar %s", "htmlFile");
		return 1;
	}

	fseek(htmlFile, 0, SEEK_END);
	htmlLargo = (size_t)ftell(htmlFile);
	fseek(htmlFile, 0, SEEK_SET);


	html = (char *)malloc(htmlLargo + 1);
	fread(html, 1, htmlLargo, htmlFile);
	fclose(htmlFile);

	return 0;
}


/*
void createMidHTML(FILE * html, DIR *dr){
	struct dirent *de;


	char *abrirContenedor = "<div class=\"w3-row-padding\">";
	fwrite(abrirContenedor, 1, strlen(abrirContenedor), html);


	for (int i = 0; (de = readdir(dr)) != NULL; i++){
		if(strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
			i--;
			continue;
		}

		fprintf(html, templateVideo, de->d_name);
	}


	char *cerrarContenedor = "</div>";
	fwrite(cerrarContenedor, 1, strlen(cerrarContenedor), html);

}*/

int getTOPDOWNhtml() {
	size_t largoArchivo;


	FILE * htmlSuperior = fopen(htmlTOPDir, "r");

	if(htmlSuperior == 0){
		printf("No se pudo encontrar %s", htmlTOPDir);
		return 1;
	}

	fseek(htmlSuperior, 0, SEEK_END);
	largoArchivo = (size_t)ftell(htmlSuperior);
	fseek(htmlSuperior, 0, SEEK_SET);


	htmlTOP = (char *)malloc(largoArchivo + 1);
	fread(htmlTOP, 1, largoArchivo, htmlSuperior);
	fclose(htmlSuperior);


	FILE *htmlInferior = fopen(htmlBOTDir, "r");

	if(htmlInferior == 0){
		printf("NO se pudo encontrar %s", htmlBOTDir);
		return 1;
	}

	fseek(htmlInferior, 0, SEEK_END);
	largoArchivo = (size_t)ftell(htmlInferior);
	fseek(htmlInferior, 0 ,SEEK_SET);


	htmlBOT = (char *)malloc(largoArchivo + 1);
	fread(htmlBOT, 1, largoArchivo, htmlInferior);
	fclose(htmlInferior);
}