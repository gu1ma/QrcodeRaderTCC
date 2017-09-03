#include <iostream>

using namespace std;


int main(){
	#define MESSAGE (const unsigned char *) "test"
	#define MESSAGE_LEN 4
	#define CIPHERTEXT_LEN (crypto_box_MACBYTES + MESSAGE_LEN)

	unsigned char alice_publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char alice_secretkey[crypto_box_SECRETKEYBYTES];
	crypto_box_keypair(alice_publickey, alice_secretkey);

	unsigned char bob_publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char bob_secretkey[crypto_box_SECRETKEYBYTES];
	crypto_box_keypair(bob_publickey, bob_secretkey);

	unsigned char nonce[crypto_box_NONCEBYTES];
	unsigned char ciphertext[CIPHERTEXT_LEN];
	randombytes_buf(nonce, sizeof nonce);
	if (crypto_box_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce,
	                    bob_publickey, alice_secretkey) != 0) {
	    /* error */
	}

	cout << "ciphertext: " << ciphertext << endl;

	unsigned char decrypted[MESSAGE_LEN];
	if (crypto_box_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce,
	                         alice_publickey, bob_secretkey) != 0) {
	    /* message for Bob pretending to be from Alice has been forged! */
	}

	cout << "decripted: " << decripted << endl;

return 0;
}