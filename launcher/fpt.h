#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <openssl/md5.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void log(char log[]){
	char echo[2048]="echo `date`.\" : ";
	char logFile[2048]=" \" >>/var/log/RuVi/ruvi-log.log";
	strcat(echo,log);
	strcat(echo,logFile);
	system(echo);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void logError(char error[]){
	char echo[2048]="echo `date`.\" : ";
	char logFile[2048]=" \" >>/var/log/RuVi/ruvi-errors.log";
	strcat(echo,error);
	strcat(echo,logFile);
	system(echo);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void commandToString(char comando[], char salida[]){
	FILE   *ret;
	const int MAX_BUFFER = 2048;
	char retcmd[MAX_BUFFER];
	char bufferLectura[MAX_BUFFER];
	char logging[MAX_BUFFER]="";
	ret=popen(comando, "r");
	if (ret==NULL){
		exit(1);
	}else{
		while (fgets(retcmd, sizeof(retcmd)-3, ret)!=NULL){
	  		retcmd[strlen(retcmd) - 1] = '\0';
  			strcpy(salida,retcmd);
	  	}
	pclose(ret);

	strcpy(logging,"Comando: ");
	strcat(logging,comando);
	strcat(logging," >>> Salida: ");
	strcat(logging,salida);
	log(logging);
  	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void commandToNull(char comando[]){
	char comandoToNull[2048]="";
	char salida[2048]="";
	strcat(comandoToNull,comando);
	commandToString(comandoToNull,salida);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int checkFingerprint(char fingerprint[]){
	FILE   *ret;
	const int MAX_BUFFER = 2048;
	int out=0;
	char bufferLectura[MAX_BUFFER]="";
	char file[MAX_BUFFER]="";
	char logging[MAX_BUFFER]="";
	strcpy(file,fingerprint);
	ret=fopen(file, "r");
		if (ret==NULL){
			out=0;
			strcpy(logging,"Fingerprint fallo");
			log(logging);
		}else{
			fscanf(ret,"%s",bufferLectura);
		  	fclose(ret);
		}
		if(strcmp(bufferLectura,fingerprint)==0){
			out=1;
			strcpy(logging,"Fingerprint aprobado");
			log(logging);
		}
	return out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void saveFingerprintToFile(char fingerprint[]){
	char comando[256]="echo ";
	char logging[2048]="";
	strcat(comando,fingerprint);
	strcat(comando," >> ");
	strcat(comando,"/etc/RuVi/config.txt");
	log(comando);
	system(comando);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setFingerprint(char identity[], char fingerprint[]){
	unsigned char digest[32];
	char logging[2048]="";
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, identity, strlen(identity));
    MD5_Final(digest, &ctx);
    char mdString[33];
	for (int i = 0; i < 16; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    }
 	strcpy(fingerprint,mdString);
 	strcpy(logging,"Fingerprint generado ");
	strcat(logging,mdString);
	log(logging);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getIdentity(char salida[]){
	const int MAX_BUFFER = 2048;
	char salidatemp[MAX_BUFFER]="";
	char comando[MAX_BUFFER]="";
	char logging[2048]="";
	strcpy(salida,"");
	strcpy(comando,"sudo lshw -class network | grep serial: | cut -c 15-");
	commandToString(comando,salidatemp);
	strcpy(comando,"sudo lshw -class network | grep serie: | cut -c 15-");
	commandToString(comando,salidatemp);
	strcat(salida,salidatemp);
	strcpy(comando,"sudo hdparm -i /dev/sda | cut -d ',' -f3 | grep SerialNo= | cut -c 11-100");
	commandToString(comando,salidatemp);
	strcat(salida,salidatemp);
	strcpy(logging,"Identificacion ");
	strcat(logging,salida);
	log(logging);
}