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

string getDataFromFile(char *fileName){
    fstream fs;
    stringstream buffer;
    fs.open(fileName);
    buffer << fs.rdbuf();
    fs.close();
    return buffer.str();
}

void encryptRsa(
    string data,
    unsigned char *camPubKey, 
    unsigned char *camPriKey, 
    unsigned char *platePubKey,
    unsigned char *platePriKey
    ){
    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);
    const unsigned char *message = 
    (const unsigned char*) data.c_str();
    int messageLength = strlen((char *) message);
    int cipherTextLength = (crypto_box_MACBYTES + messageLength);

    unsigned char ciphertext[cipherTextLength];

    if (crypto_box_easy(ciphertext, message, messageLength, nonce,
                    camPubKey, platePriKey) != 0) {
        // error 
        cout << "Houve algum erro na criptografia" << endl;
    }

    ofstream fs;
    fs.open("encryptData.txt");
    if(fs.is_open())
        fs << ciphertext;
    else 
        cout << "Problemas ao abrir o arquivo";
    fs.close();
}

void decryptRsa(
    unsigned char *camPubKey, 
    unsigned char *camPriKey, 
    unsigned char *platePubKey,
    unsigned char *platePriKey
    ){
    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    string encrypted = getDataFromFile("encryptData.txt");
    unsigned char *message = new unsigned char[encrypted.length()+1];
    strcpy((char *)message, encrypted.c_str());
    int messageLength = strlen((char *) message);
    int cipherTextLength = (crypto_box_MACBYTES + messageLength);
    unsigned char decrypted[messageLength];

    //cout << "L: " << messageLength << endl;

    if (crypto_box_open_easy(decrypted, message, cipherTextLength, nonce, platePubKey, camPriKey) != 0) {
        cout << "Houve um erro ao tentar descriptografar o dado" << endl;;
        //return 1;
    }    

    cout << "Mensagem decriptografada: " << decrypted << endl;
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

    //encryptRsa("teste", camPubKey, camPriKey, platePubKey, platePriKey);
    decryptRsa(camPubKey, camPriKey, platePubKey, platePriKey);

	return 0;
}