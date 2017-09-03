//Bibliotecas do sistema
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <typeinfo>

//Arquivo para converter bin para b64 e vice versa
//#include "b64Encoder.cpp"

//Biblioteca sodium
#include <sodium.h>

//Necessario incluir essa biblioteca para corrigir exceção 
//Bibliotecas zbar
#include <cstddef>
#include <zbar.h>  

//Bibliotecas OpenCV
#include <cv.h>  
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>  

using namespace std;
using namespace zbar;  
using namespace cv;    

ofstream file;

//Chaves publica e privada que estarao na camera
unsigned char camPubKey[crypto_box_PUBLICKEYBYTES],
    		  camPriKey[crypto_box_SECRETKEYBYTES];


//Chaves publica e privada das placas
unsigned char platePubKey[crypto_box_PUBLICKEYBYTES],
			  platePriKey[crypto_box_SECRETKEYBYTES];

int messageLength;
int cipherTextLength;
unsigned char nonce[crypto_box_NONCEBYTES];

void getData(){
	char dados[100];

	file.open("file-example.txt");
	
	cout << "Digite o nome do carro: ";
	cin.getline(dados, sizeof(dados));
	file << "Nome do carro: " << dados << " \n";
	cout << "\nDigite a placa do carro: ";
	cin.getline(dados, sizeof(dados));
	file << "Placa: " << dados << " \n";

	file.close();
}

string getDataFromFile(char *fileName){
    fstream fs;
    stringstream buffer;
    fs.open(fileName);
    buffer << fs.rdbuf();
    fs.close();
    return buffer.str();
}

void saveEncryptedDataInFile(char *fileName, unsigned char *data){
  ofstream fs;
  fs.open(fileName);
  if(fs.is_open())
    fs << data;
  else 
    cout << "Problemas ao abrir o arquivo";
  fs.close();
}

void decryptRsa(string encrypted){
	unsigned char decrypted[messageLength];
	unsigned char *message = new unsigned char[encrypted.length()+1];
	strcpy((char *)message, encrypted.c_str());

  messageLength = strlen((char *) message);
  cipherTextLength = (crypto_box_MACBYTES + messageLength);

  randombytes_buf(nonce, sizeof nonce);

	//cout <<"message: " << message << endl;
  //cout <<"cipherTextLength: " << cipherTextLength << endl;
  //cout <<"nonce: " << nonce << endl;
  //cout <<"platePubKey: " << platePubKey << endl;
  //cout <<"camPubKey: " << camPubKey << endl;

	if (crypto_box_open_easy(decrypted, message, cipherTextLength, nonce,
                         platePubKey, camPriKey) != 0) {
    	cout << "Houve um erro ao tentar descriptografar o dado";
    	//return 1;
	}    

	//cout << "Mensagem decriptografada: " << decrypted << endl;
}

int readQrCode(int argc, char *argv[]){
	ImageScanner scanner;
 	Mat img;
 	Mat grayImg;
 	double dWidth;
 	double dHeight;
 	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
 	img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  string data;
  VideoCapture cap(0);
  if (!cap.isOpened()){  
     cout << "Cannot open the video cam" << endl;  
     return -1;  
  } 
 	dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
 	dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  namedWindow("Leitor", CV_WINDOW_AUTOSIZE);
  while(1){
    bool bSucess = cap.read(img);
    cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
    int width = grayImg.cols;
    int height = grayImg.rows;
    uchar *raw = (uchar *) grayImg.data;

    Image image(width, height, "Y800", raw, width * height);
    //scan image
    int n = scanner.scan(image);
    //get results
    for(Image::SymbolIterator symbol = image.symbol_begin();
      symbol != image.symbol_end(); 
      ++symbol
      ){
      vector<Point> vp;
      //<< "decoded " << symbol->get_type_name()
      
      //cout << "Tipo: " << typeid(symbol->get_data()).name() << endl;
      
      //cout << "Dados criptografados: " << symbol->get_data() << "" << endl;
      data = symbol->get_data();

      //Cria retangulo ao redor do QRCode 
      int n = symbol->get_location_size();
      for (int i = 0; i < n; i++){
        vp.push_back(Point(symbol->get_location_x(i),
          symbol->get_location_y(i)));
      }
      RotatedRect r = minAreaRect(vp);   
       Point2f pts[4];   
       r.points(pts);   
       for(int i = 0;i < 4;i++){   
         line(grayImg,pts[i],pts[(i+1)%4],Scalar(255,0,0),3);   
      } 

    }
    decryptRsa(data);
    imshow("Leitor", grayImg);
    if (waitKey(30) == 27){  
       cout << "esc key is pressed by user" << endl;  
       return 0;  
    }

  }
}

void encryptRsa(string data){
	const unsigned char *message = 
	(const unsigned char*) data.c_str();
	messageLength = strlen((char *) message);
	cipherTextLength = (crypto_box_MACBYTES + messageLength);

	unsigned char ciphertext[cipherTextLength];
	randombytes_buf(nonce, sizeof nonce);

	if (crypto_box_easy(ciphertext, message, messageLength, nonce,
                    camPubKey, platePriKey) != 0) {
    	/* error */
    	cout << "Houve algum erro na criptografia" << endl;
	}

	string cipherMessage(reinterpret_cast<char*>(ciphertext));
	cipherMessage = "\"" + cipherMessage +"\"";
  	cipherMessage = string("qrencode ") + cipherMessage + string(" -o teste.png"); 
  	system(cipherMessage.c_str());
}

//Função para gerar chaves publica e privada da camera
void generateCamKeys(){
  crypto_box_keypair(camPubKey, camPriKey);
  //Cria arquivo com a chave publica da camera
  file.open("cam-pub-key.txt");
  file << camPubKey;
  file.close();

  //Cria arquivo com a chave privada da camera
  file.open("cam-pri-key.txt");
  file << camPriKey;
  file.close();
}

//Função para gerar chaves publica e privada da placa
void generatePlateKeys(){
  crypto_box_keypair(platePubKey, platePriKey);
  //Cria arquivo com a chave publica da placa
  file.open("plate-pub-key.txt");
  file << platePubKey;
  file.close();

  //Cria arquivo com a chave privada da camera
  file.open("plate-pri-key.txt");
  file << platePriKey;
  file.close();
}

int main(int argc, char *argv[]){
	//sodium_init() inicializa a lib, deve ser chamada antes de qualquer outra função
	if (sodium_init() == -1) {
    cout << "Error on init sodium library";
    return 1;
  }

  crypto_box_keypair(platePubKey, platePriKey);

  const char *hex;
  size_t l = strlen(platePubKey);

  sodium_bin2hex(hex,l * 2 + 1, platePubKey, l);

  cout << "Hex: "<< hex << endl;

  /*string key = getDataFromFile("keys/cam-pub-key.txt"); 
  copy(key.begin(), key.end(), camPubKey);

  key = getDataFromFile("keys/cam-pri-key.txt"); 
  copy(key.begin(), key.end(), camPriKey);

  key = getDataFromFile("keys/plate-pub-key.txt"); 
  copy(key.begin(), key.end(), platePubKey);

  key = getDataFromFile("keys/plate-pri-key.txt"); 
  copy(key.begin(), key.end(), platePriKey);*/

  //cout << "Cam pub: " << camPubKey << endl; 
  //cout << "Cam pri: " << camPriKey << endl; 
  //cout << "P pub: " << platePubKey << endl; 
  //cout << "P pri: " << platePriKey << endl; 

	//encryptRsa(getDataFromFile("file-example.txt"));
	//readQrCode(argc, argv);

	return 0;
}
