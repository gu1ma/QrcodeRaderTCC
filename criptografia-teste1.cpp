#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
//Biblioteca sodium
#include <sodium.h>

using namespace std;

void saveKey(const char *filename, unsigned char key[]){
    /*ofstream file;
    //Save key on binary file
    file.open(filename, ios::binary);
    file << key;
    file.close();*/
    int l = strlen((char *)key);
    ofstream file;
    file.open(filename, ios::out | ios::binary);
    file.write(reinterpret_cast<char *>(&key), l);
    file.close();
}

void readKey(const char *filename){
    /*fstream fs;
    stringstream buffer;
    fs.open(filename);
    buffer << fs.rdbuf();
    fs.close();
    return buffer.str();*/
    //unsigned char *key;
    /*ifstream file;
    file.open(filename, ios::in | ios::binary | ios::ate);
    if(file.is_open()){
        int size = file.tellg();
        char key[1000];
        cout << "L: " << size << endl;
        file.read(&key, size);
        cout << "K > " << key << endl;
        file.close();
    }*/

}

void generateKeys(
    unsigned char camPubKey[], 
    unsigned char camPriKey[], 
    unsigned char platePubKey[],
    unsigned char platePriKey[]
    ){
    
    ifstream file;
    string fileNames[] = {
        "keys/plate-pub-key.bin", 
        "keys/plate-pri-key.bin",
        "keys/cam-pub-key.bin",
        "keys/cam-pri-key.bin"
    };

    file.open("keys/plate-pub-key.bin");
    if(file.good()){
        cout << "Ok, file exists" << endl;
        /*string splatePubKey = readKey("keys/plate-pub-key.bin");
        string splatePriKey = readKey("keys/plate-pri-key.bin");
        string scamPubKey = readKey("keys/cam-pub-key.bin");
        string scamPriKey = readKey("keys/cam-pri-key.bin");

        copy(splatePubKey.begin(), splatePubKey.end(), platePubKey);
        copy(splatePriKey.begin(), splatePriKey.end(), platePriKey);
        copy(scamPubKey.begin(), scamPubKey.end(), camPubKey);
        copy(scamPriKey.begin(), scamPriKey.end(), camPriKey);*/
        file.close();
    } else {
        cout << "No, the file not exists" << endl;
        crypto_box_keypair(platePubKey, platePriKey);
        crypto_box_keypair(camPubKey, camPriKey);
        saveKey("keys/plate-pub-key.bin", platePubKey);
        saveKey("keys/plate-pri-key.bin", platePriKey);
        saveKey("keys/cam-pub-key.bin", camPubKey);
        saveKey("keys/cam-pri-key.bin", camPriKey);
        file.close();
    }
    
}

void encryptRsa(
    string data,
    unsigned char camPubKey[], 
    unsigned char camPriKey[], 
    unsigned char platePubKey[],
    unsigned char platePriKey[],
    unsigned char nonce[]){

    const unsigned char *message = 
    (const unsigned char*) data.c_str();
    int messageLength = strlen((char *) message);
    int cipherTextLength = (crypto_box_MACBYTES + messageLength);

    unsigned char ciphertext[cipherTextLength];

    if (crypto_box_easy(ciphertext, message, messageLength, nonce,
                    camPubKey, platePriKey) != 0) {
        /* error */
        cout << "Houve algum erro na criptografia" << endl;
    }

    ofstream fs;
    fs.open("encryptData.txt");
    if(fs.is_open())
        fs << ciphertext;
    else 
        cout << "Problemas ao abrir o arquivo";
    fs.close();

    /*string cipherMessage(reinterpret_cast<char*>(ciphertext));
    cipherMessage = "\"" + cipherMessage +"\"";
    cipherMessage = string("qrencode ") + cipherMessage + string(" -o teste.png"); 
    system(cipherMessage.c_str());*/
}

void decryptRsa(
    string encrypted,
    unsigned char camPubKey[], 
    unsigned char camPriKey[], 
    unsigned char platePubKey[],
    unsigned char platePriKey[],
    int messageLength,
    unsigned char nonce[]
    ){

    unsigned char *message = new unsigned char[encrypted.length()+1];
    strcpy((char *)message, encrypted.c_str());
    //int messageLength = strlen((char *) message);
    int cipherTextLength = (crypto_box_MACBYTES + messageLength);
    unsigned char decrypted[messageLength];

    cout << "message: " << message << endl;
    cout << "cipherTextLength: " << cipherTextLength << endl;
    cout << "message length: " << messageLength << endl;
    cout << "camPub: " << camPubKey << endl;
    cout << "camPri: " << camPriKey << endl;
    cout << "platePub: " << platePubKey << endl;
    cout << "platePriv: " << platePriKey << endl;

    if (crypto_box_open_easy(decrypted, message, cipherTextLength, nonce,
                         platePubKey, camPriKey) != 0) {
        cout << "Houve um erro ao tentar descriptografar o dado" << endl;;
        //return 1;
    }    

    cout << "Mensagem decriptografada: " << decrypted << endl;
}

string getDataFromFile(char *fileName){
    fstream fs;
    stringstream buffer;
    fs.open(fileName);
    buffer << fs.rdbuf();
    fs.close();
    return buffer.str();
}

int main(int argc, char *argv[]){
    #define MESSAGE (const unsigned char *) "test"
    #define MESSAGE_LEN 4
    #define CIPHERTEXT_LEN (crypto_box_MACBYTES + MESSAGE_LEN)

    //cout << "C: " << CIPHERTEXT_LEN << endl;

    //Chaves publica e privada que estarao na camera
    unsigned char camPubKey[crypto_box_PUBLICKEYBYTES],
            camPriKey[crypto_box_SECRETKEYBYTES];

    //Chaves publica e privada das placas
    unsigned char platePubKey[crypto_box_PUBLICKEYBYTES],
          platePriKey[crypto_box_SECRETKEYBYTES];

    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    //crypto_box_keypair(platePubKey, platePriKey);
    //crypto_box_keypair(camPubKey, camPriKey);

    generateKeys(camPubKey, camPriKey, platePubKey, platePriKey);

    //readKey("keys/plate-pub-key.bin");

    /*cout << "before: " << endl;
    cout << "camPub: " << camPubKey << endl;
    cout << "camPri: " << camPriKey << endl;
    cout << "platePub: " << platePubKey << endl;
    cout << "platePriv: " << platePriKey << endl;*/


    //sodium_init() inicializa a lib, deve ser chamada antes de qualquer outra função
    /*if (sodium_init() == -1) {
        cout << "Error on init sodium library";
        return 1;
    }

    //encryptRsa("Test", camPubKey, camPriKey, platePubKey, platePriKey, nonce);
    //string encryptedData = getDataFromFile("encryptData.txt");
    //cout << "Enc: "<< encryptedData << endl;
    //decryptRsa(encryptedData, camPubKey, camPriKey, platePubKey, platePriKey, 4, nonce);

    unsigned char ciphertext[CIPHERTEXT_LEN];
    randombytes_buf(nonce, sizeof nonce);

    if (crypto_box_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce, camPubKey, platePriKey) != 0) {
        //Error 
    }

    generateKeys(camPubKey, camPriKey, platePubKey, platePriKey);

    cout << "after: " << endl;
    cout << "camPub: " << camPubKey << endl;
    cout << "camPri: " << camPriKey << endl;
    cout << "platePub: " << platePubKey << endl;
    cout << "platePriv: " << platePriKey << endl;

    //cout << "Cipher text: " << ciphertext << endl;
    ofstream file;
    file.open("crip.txt");
    file << ciphertext;
    file.close();

    string sother = getDataFromFile("crip.txt");
    unsigned char *message = new unsigned char[sother.length()+1];
    strcpy((char *)message, sother.c_str());

    unsigned char decrypted[MESSAGE_LEN];

    //randombytes_buf(nonce, sizeof nonce);

    if (crypto_box_open_easy(decrypted, message, CIPHERTEXT_LEN, nonce,
                             platePubKey, camPriKey) != 0) {
        //Error
    }
    
    cout << "Decrypted text: " << decrypted << endl;*/

    return 0;
}