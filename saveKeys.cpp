#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
//Biblioteca sodium
#include <sodium.h>

using namespace std;


void saveKey(const char *filename, unsigned char key[]){
	FILE *file;
	//Escrita em modo binario
	file = fopen(filename, "wb");
	fwrite(key, strlen((char *)key), 1, file);
}


unsigned char *readKey(const char *filename){
	FILE *file;
	unsigned char *key;
	//Leitura em modo binario
	file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	key = new unsigned char[size+1];
	fread(key, size, 1, file);

	return key;
}

void generateKeys(){
    ifstream file;
    file.open("keys/plate-pub-key.bin");
   
    if(!file.good()){
    	//Chaves publica e privada que estarao na camera
    	unsigned char camPubKey[crypto_box_PUBLICKEYBYTES],
            camPriKey[crypto_box_SECRETKEYBYTES];

    	//Chaves publica e privada das placas
    	unsigned char platePubKey[crypto_box_PUBLICKEYBYTES],
            platePriKey[crypto_box_SECRETKEYBYTES];

        cout << "No, the file not exists" << endl;
        crypto_box_keypair(platePubKey, platePriKey);
        crypto_box_keypair(camPubKey, camPriKey);
        saveKey("keys/plate-pub-key.bin", platePubKey);
        saveKey("keys/plate-pri-key.bin", platePriKey);
        saveKey("keys/cam-pub-key.bin", camPubKey);
        saveKey("keys/cam-pri-key.bin", camPriKey);	
    }    
    file.close();
}

int main(){
    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    //Chaves publica e privada da camera
    unsigned char *camPubKey;
    unsigned char *camPriKey;

    //Chaves publica e privada da placa
    unsigned char *platePubKey;
    unsigned char *platePriKey;

    //Caso nao tenha chaves ainda, geramos ...
    generateKeys();
    
    //Recuperamos as chaves que estao salvas em arquivo
    camPubKey = readKey("keys/cam-pub-key.bin");
    camPriKey = readKey("keys/cam-pri-key.bin");
    platePubKey = readKey("keys/plate-pub-key.bin");
    platePriKey = readKey("keys/cam-pub-key.bin");

    //cout << "CamPubKey: " << endl;
    //cout << "CamPubKey length: " << strlen((char*) camPubKey) << endl;

    //cout << "La no finalzao" << endl;
    //for(int i = 0; i < strlen((char*) platePubKey); i++){
    //	printf("%x ", platePubKey[i]);
    //}

	return 0;
}