/*
AUTOR: 		Martín Alejandro Fernández

ARCHIVO: 	

DEPENDENCIA: libssl-dev (sudo apt-get install libssl-dev)
			 hwinfo (sudo apt-get install hwinfo)
			 nmap (sudo apt-get install nmap)

COMPILACION: g++ fpt.cpp -o fpt -lcrypto

EJECUCION: sudo ./fpt --20160801 Para generar la llave e insertarla en el servidor
		   sudo ./fpt Para comprobar  generada con la llave en el servidor

DESCRIPCION: 	
*/
#include "fpt.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
	const int MAX_BUFFER = 2048;
	char identity[MAX_BUFFER]="";
	char fingerprint[33]="";
	char comando[MAX_BUFFER]="";
	
	cout << "Espere por favor..." << endl;
	strcpy(comando,"sudo --user=terminal touch /var/log/RuVi/ruvi-log.log");
	commandToNull(comando);
	strcpy(comando,"sudo --user=terminal touch /var/log/RuVi/ruvi-errors.log");
	commandToNull(comando);

	getIdentity(identity);
	setFingerprint(identity,fingerprint);
	saveFingerprintToFile(fingerprint);
	cout << "Huella de registro finalizada" << endl;
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////