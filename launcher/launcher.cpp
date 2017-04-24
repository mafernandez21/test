/*
AUTOR: 		Martín Alejandro Fernández

ARCHIVO: 	

DEPENDENCIA: libssl-dev (sudo apt-get install libssl-dev)
			 hwinfo (sudo apt-get install hwinfo)
			 nmap (sudo apt-get install nmap)

COMPILACION: g++ launcher.cpp -o launcher -lcrypto

EJECUCION: sudo ./launcher --20160801 Para generar la llave e insertarla en el servidor
		   sudo ./launcher Para comprobar  generada con la llave en el servidor

DESCRIPCION: 	
*/
#include "terminal.h"
//
void ejecutar();
//
void terminalAprobada();
//
void terminalDesaprobada();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
	const int MAX_BUFFER = 2048;
	int mode=0;
    char identity[MAX_BUFFER]="";
	char fingerprint[33]="";
	char comando[MAX_BUFFER]="";
	char salida[MAX_BUFFER]="";
	char puerto[32]="45654";

	cout << "Espere por favor..." << endl;
	strcpy(comando,"sudo --user=terminal touch /var/log/RuVi/ruvi-log.log");
	commandToNull(comando);
	strcpy(comando,"sudo --user=terminal touch /var/log/RuVi/ruvi-errors.log");
	commandToNull(comando);

	mode=setMode(argc,argv);

	if(isChRoot()!=1){
		switch(mode){
			case 1:
				if(findAndSetServer(puerto)==1){
					getIdentity(identity);
					setFingerprint(identity,fingerprint);
					saveFingerprintToFile(fingerprint);
					scpUploadFile(fingerprint);
					delFile(fingerprint);
					setIdentitySSH();
					cout << "Proceso de registro finalizado" << endl;
					return 0;
				}else{
					cout << "Proceso de registro finalizado con errores" << endl;
					return 1;
				}
			break;

			default:
				addIdentitySSH();
				if(findAndSetServer(puerto)==1){
					getIdentity(identity);
					setFingerprint(identity,fingerprint);
					scpDownloadFile(fingerprint);
					if (checkFingerprint(fingerprint)==1){
						delFile(fingerprint);
						terminalAprobada();
					}else{
						delFile(fingerprint);
						strcpy(comando,"echo `date`. : \"Fallo el fingerprint\" >> error");
						commandToNull(comando);
						terminalDesaprobada();
						return 1;
					}	
					return 0;
				}else{
					delFile(fingerprint);
					strcpy(comando,"echo `date`. : \"Fallo al tratar de encontrar el servidor\" >> error");
					commandToNull(comando);
					terminalDesaprobada();
					return 1;
				}
				return 0;
			break;
		}
	}else{
		panic();
		strcpy(comando,"echo `date`. : \"Se ejecuto CHROOT\" >> error");
		commandToNull(comando);
		terminalDesaprobada();
		return 1;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ejecutar(){
	char comando[2048]="";
	char name[2048]="";
	char logging[2048]="";
	
	strcpy(comando,".conf");
	scpDownloadFile(comando);

	strcpy(comando,"palkey");
	scpDownloadFile(comando);

	strcpy(comando,"run");
	scpDownloadFile(comando);

	strcpy(comando,"ruviGuard");
	scpDownloadFile(comando);


	strcpy(comando,"sudo chmod +x run");
	system(comando);

	strcpy(comando,"sudo chmod +x ruviGuard");
	system(comando);

	strcpy(comando,"./run");
	system(comando);
	
	strcpy(logging,"Terminal Lista para uso!");
	log(logging);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void terminalAprobada(){
	char comando[2048]="";
	char name[2048]="";
	char logging[2048]="";

	strcpy(logging,"Terminal Aprobada");
	log(logging);

	if(system("stat contenedor > /dev/null 2>&1")!=0){
		strcpy(comando,"contenedor");
		scpDownloadFile(comando);
		strcpy(comando,"version");
		strcpy(name,"Lversion");
		scpDownloadFileRename(comando,name);
		for(;;){
			if(system("stat contenedor > /dev/null 2>&1")==0){
				break;
			}
		}
	}else{
		strcpy(comando,"version");
		scpDownloadFile(comando);
		if(needUpdate()==true){
			update();
		}
	}
	ejecutar();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void terminalDesaprobada(){
	char comando[2048]="";	
	char logging[2048]="";

	strcpy(logging,"Terminal Desaprobada, reiniciando terminal");
	log(logging);
	
	strcpy(comando,"sudo reboot");
	system(comando);
}
