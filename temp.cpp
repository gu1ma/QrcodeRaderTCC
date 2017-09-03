unsigned char decrypted[messageLength];

    

	if (crypto_box_open_easy(decrypted, ciphertext, cipherTextLength, nonce,
                         platePubKey, camPriKey) != 0) {
    	/* message for Bob pretending to be from Alice has been forged! */
    	cout << "Houve um erro ao tentar descriptografar o dado";
    	return 1;
	}    

	cout << "Mensagem decriptografada: " << decrypted << endl;