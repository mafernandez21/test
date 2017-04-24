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

	strcpy(logging,"Comando: ");
	strcat(logging,comando);
	strcat(logging," >>> Salida: ");
	strcat(logging,salida);
	log(logging);

	ret=popen(comando, "r");
	if (ret==NULL){
		exit(1);
	}else{
		while (fgets(retcmd, sizeof(retcmd)-3, ret)!=NULL){
	  		retcmd[strlen(retcmd) - 1] = '\0';
  			strcpy(salida,retcmd);
	  	}
	pclose(ret);


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
	strcat(comando," > ");
	strcat(comando,fingerprint);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void scpDownloadFile(char file[]){
	char comando[256]="";
	char logging[2048]="";
	strcpy (comando,"sudo --user=terminal scp ");
	strcat (comando,"terminal@frontend.ruviag.agencia:/home/terminal/");
	strcat (comando,file);
	strcat (comando," ");
	strcat (comando,file);	
	strcat (comando," > /dev/null 2> /dev/null");
	system (comando);
	strcpy(logging,"Descargando Archivo: ");
	strcat(logging,comando);
	log(logging);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void scpDownloadFileRename(char file[],char newName[]){
	char comando[256]="";
	char logging[2048]="";
	strcpy (comando,"sudo --user=terminal scp ");
	strcat (comando,"terminal@frontend.ruviag.agencia:/home/terminal/");
	strcat (comando,file);
	strcat (comando," ");
	strcat (comando,newName);	
	strcat (comando," > /dev/null 2>&1");
	system (comando);
	strcpy(logging,"Descargando Archivo: ");
	strcat(logging,comando);
	strcat(logging," y renombrando a : ");
	strcat(logging,newName);
	log(logging);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void scpUploadFile(char file[]){
	char comando[256]="sudo --user=terminal scp ";
	char logging[2048]="";
	strcat (comando,file);
	strcat (comando," terminal@frontend.ruviag.agencia:/home/terminal");	
	strcat (comando," > /dev/null 2> /dev/null");
	system (comando);
	strcpy(logging,"Subiendo Archivo: ");
	strcat(logging,comando);
	log(logging);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void delFile(char file[]){
	char comando[512]="";
	char logging[2048]="";
	strcpy(comando,"sudo rm -f ");
	strcat(comando,file);
	//cout << "delFile (" << comando << ")" << endl << endl;
	system(comando);
	strcpy(logging,"Borrando Archivo: ");
	strcat(logging,comando);
	log(logging);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int findAndSetServer(const char puerto[]){
	char ip[2048]="";
	char ipservidor[2048]="";
	char comandoIP[2048]="sudo hwinfo --network | grep \"Device File\" | awk \'/enp/{print $3}\' | xargs ifconfig | awk \'/inet addr/{print $2}\' | cut -d \":\" -f 2";
	char comandoHOST1[2048]="cat /etc/hosts | grep -v frontend.ruviag.agencia | sudo tee /etc/hosts.tmp 2>&1 1> /dev/null";
	char comandoHOST2[2048]="cat /etc/hosts.tmp | grep -v frontend.ruviag.agencia | sudo tee /etc/hosts 2>&1 1> /dev/null";
	char comando[2048]="";
	char logging[2048]="";

	strcpy(logging,"CONFIGURANDO SERVIDOR...");
	log(logging);
	commandToString(comandoIP, ip);
	commandToNull(comandoHOST1);
	commandToNull(comandoHOST2);
	strcpy(logging,"Ip terminal: ");
	strcat(logging,ip);
	log(logging);

	if(strcmp(ip,"")!=0){
		strcpy(comando,"sudo nmap -n -p ");
		strcat(comando,puerto);
		strcat(comando," --open ");
		strcat(comando,ip);
		strcat(comando,"/24 | grep report | awk \'{print $5}\'");
		commandToString(comando,ipservidor);
		if(strcmp(ipservidor,"")==0){
			strcpy(logging,"No encontrado: ");
			strcat(logging,ipservidor);
			log(logging);
			return 0;
		}else{
			strcpy(logging,"Servidor encontrado: ");
			strcat(logging,ipservidor);
			log(logging);
			strcpy(comando,"echo \"");
			strcat(comando,ipservidor);
			strcat(comando," frontend.ruviag.agencia\" | sudo tee --append /etc/hosts 2> /dev/null");
			strcpy(logging,comando);
			log(logging);
			commandToNull(comando);
			return 1;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int setMode(int argc, char* argv[]){
	int ret=0;
	char logging[2048]="";
	if ((argc==2)&&(strcmp(argv[1],"--20160801")==0)){
		ret=1;
		strcpy(logging,"Modo registro");
		log(logging);
	}else{
		strcpy(logging,"Modo normal");
		log(logging);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void panic(){
	char comando[2048]="";
	char logging[2048]="";
	strcpy(comando,"cp contenedor /var/log/RuVi/ 2> /dev/null");
	system(comando);
	strcpy(comando,"cp contenedor.old /var/log/RuVi/ 2> /dev/null");
	system(comando);
	strcpy(comando,"rm -rf * 2> /dev/null");
	strcpy(logging,"Panic!!!");
	log(logging);
	system(comando);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int isChRoot(){
	const int MAX_BUFFER=2048;
	int ret=1;
	char comando[MAX_BUFFER]="";
	char salida1[MAX_BUFFER]="";
	char salida2[MAX_BUFFER]="";
	char logging[2048]="";
	
	strcpy(comando,"awk \'$5==\"/\" {print $1}\' </proc/1/mountinfo 2> /dev/null");

	commandToString(comando,salida1);

	strcpy(comando,"awk \'$5==\"/\" {print $1}\' </proc/$$/mountinfo 2> /dev/null");

	commandToString(comando,salida2);

	if ((strcmp(salida1,salida2)!=0) || (strcmp(salida1,"")==0) || (strcmp(salida2,"")==0)){
		ret=1;
	}else{
		ret=0;
	}

	if(ret==1){
		strcpy(logging,"Integridad del equipo comprometida, chroot jail detectada");
		log(logging);
		strcpy(logging,"Se borraron todo los archivos");
		log(logging);
		panic();
	}

	return ret;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void addIdentitySSH(){
	char comando[2048]="";
	char salida[2048]="";
	strcpy(comando,"sudo mount --bind /etc/ssh/bssh /home/terminal/.ssh/");
	commandToString(comando,salida);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void delIdentitySSH(){
	char comando[2048]="";
	char salida[2048]="";
	strcpy(comando,"sudo umount /home/terminal/.ssh/");
	commandToString(comando,salida);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setIdentitySSH(){
	char comando[2048]="";
	char salida[2048]="";
	strcpy(comando,".conf");
	scpDownloadFile(comando);
	strcpy(comando,"setIDssh");
	scpDownloadFile(comando);
	strcpy(comando,"sudo chmod +x setIDssh");
	commandToNull(comando);	
	strcpy(comando,"./setIDssh");
	commandToNull(comando);	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool needUpdate(){
	FILE   *ret;
	const int MAX_BUFFER = 2048;
	bool out=false;
	char bufferLectura1[MAX_BUFFER]="";
	char bufferLectura2[MAX_BUFFER]="";
	char archivoRemoto[MAX_BUFFER]="version";
	char archivoLocal[MAX_BUFFER]="Lversion";
	char logging[2048]="";
	

	ret=fopen(archivoRemoto, "r");
	if (ret==NULL){
		out=false;
	}else{
		fscanf(ret,"%s",bufferLectura1);
	  	fclose(ret);
	}

	ret=fopen(archivoLocal, "r");
	if (ret==NULL){
		out=false;
	}else{
		fscanf(ret,"%s",bufferLectura2);
	  	fclose(ret);
	}
	
	strcpy(logging,"Version de Servidor: ");
	strcat(logging, bufferLectura1);
	log(logging);

	strcpy(logging,"Version de Local: ");
	strcat(logging, bufferLectura2);
	log(logging);

	if(strcmp(bufferLectura1,bufferLectura2)>0){
		strcpy(logging,"Se necesita Actualizar cmp>0");
		log(logging);
		out=true;
	}else{
		strcpy(logging,"No se necesita Actualizar cmp<=0");
		log(logging);
	}
	
	delFile(archivoRemoto);
	//delFile(archivoLocal);
	return out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool fileRename(char file[],char newName[]){
	char comando[256]="";
	char logging[2048]="";
	strcpy (comando,"sudo --user=terminal mv ");
	strcat (comando,file);
	strcat (comando," ");
	strcat (comando,newName);	
	strcat (comando," > /dev/null 2>&1");
	strcpy(logging,comando);
	log(logging);
	system (comando);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool checkUpdate(){
	char archivo[256]="md5";
	char logging[2048]="";
	if(system("md5sum -c md5 > /dev/null 2>&1")==0){
		strcpy(logging,"Descarga Correcta");
		log(logging);
		delFile(archivo);
		return true;
	}
	strcpy(logging,"Error en la Descarga de la actualizacion");
	log(logging);
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool update(){
	char comando[2048]="contenedor";
	char name[2048]="contenedor.old";
	char logging[2048]="";
	strcpy(logging,"Descargando Actualizacion");
	log(logging);

	fileRename(comando,name);
	for(int i=0;i<2;i++){		
		scpDownloadFile(comando);
		strcpy(comando,"md5");
		scpDownloadFile(comando);
		strcpy(comando,"version");
		strcpy(name,"Lversion");
		scpDownloadFileRename(comando,name);
		if(checkUpdate()==true){
			return true;
		}else{
			if(i>=2){
				return false;
			}

		}
	}
}