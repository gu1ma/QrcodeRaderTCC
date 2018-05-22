//Bibliotecas nativas
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

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

Mat segmentation(Mat input, int v){
    Mat output;

    int t = 0;

    //Parametros de limiarização otsu
    double thresh = 0;
    double maxValue = 255;

    if(v == 0){
        adaptiveThreshold(input,output,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,7,11);
    }

    //Gaussiano
    if(v == 1){
        adaptiveThreshold(input,output,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,7,11);
    }

    //Binarizacao por Otsu Thresh binario
    if(v == 2){
        threshold(input, output, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); 
    }

    //Binarizacao por Otsu Thresh tozero
    if(v == 3){
        threshold(input, output, 0, 255, CV_THRESH_TOZERO | CV_THRESH_OTSU); 
    }    

    return output;
}

Mat histogramEqualization(Mat input){
    Mat output;

    equalizeHist(input, output);

    return output;
}

void generateKeys(string keyName, string keyLenght){
    string genPrivKey = "openssl genrsa -out keys/"+ keyName +".pri "+keyLenght;
    string genPubKey = "openssl rsa -in keys/"+ keyName +".pri -pubout > keys/"+ keyName +".pub";
    string fileName = "keys/" + keyName + ".pri";
    ifstream file;
    file.open(fileName.c_str());
    //Se não existir as chaves, geramos
    if(!file.good()){
        system(genPrivKey.c_str());
        system(genPubKey.c_str());
    }    
    file.close();
}

//Encriptando dados com RSA
void encryptFileWithRsa(string fileName, string newFileName, string keyName){
    string encrypt = "openssl rsautl -encrypt -inkey keys/"+ keyName +".pub -pubin -in "+ fileName +".txt -out encBinary.txt";
    system(encrypt.c_str());
    string encryptB64= "base64 encBinary.txt >"+newFileName+".txt";
    system(encryptB64.c_str());
}

//Decriptando dados com RSA
void decryptFileWithRsa(string fileName, string newFileName, string keyName){
    ifstream file;
    file.open(newFileName.c_str());
    //Se não existir o arquivo decifrado, geramos ...
    //if(!file.good()){
        string decryptB4 = "base64 --decode " +fileName+ ">decBinary.txt";
        system(decryptB4.c_str());
        string decrypt = "openssl rsautl -decrypt -inkey keys/"+ keyName +".pri -in decBinary.txt -out "+newFileName;
        //cout << decrypt << endl;
        system(decrypt.c_str());
    //}    
    file.close();
}

//Encriptando dados com AES
void encryptFileWithAES(string fileName, string newFileName, string keyFile){
    string encrypt = "openssl aes-128-cbc -in " +fileName+ " -a -out "+newFileName+" -k keys/"+keyFile+".txt";
    system(encrypt.c_str());
}

//Decriptando dados com AES
void decryptFileWithAES(string fileName, string newFileName, string keyFile){
    string decrypt = 
    "openssl aes-128-cbc -d -a -in "+fileName+" -out "+newFileName+" -k keys/"+keyFile;
    system(decrypt.c_str());
}


string getDataFromFile(const char *fileName){
    fstream fs;
    stringstream buffer;
    fs.open(fileName);
    buffer << fs.rdbuf();
    fs.close();
    return buffer.str();
}

void saveDataInFile(const char *fileName, string data){
    ofstream file;
    file.open(fileName);
    if(file.is_open()){
        file << data;
    } else {
        cout << "Error on open file";
    }
}

void generateQrCode(string qrCodeName, string f){
    string fileName = f+".txt";
    string data = getDataFromFile(fileName.c_str());
    string generateQrCode = "qrencode \"" + data + "\" -s 10 -l L -o qrCodes/"+qrCodeName+".png";
    system(generateQrCode.c_str());
} 

int readQrCode(string fileName, string newFileName,string keyName, bool aes){
    ImageScanner scanner;
    Mat grayImg;
    Mat img;
    double dWidth;
    double dHeight;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    string data;
    fileName = fileName + ".txt";
    newFileName = newFileName + ".txt";
    
    //Webcam externa
    VideoCapture cap(1);

    //FULL HD
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

    //Setando a taxa de frames por segundo
    cap.set(CV_CAP_PROP_FPS, 30);

    //Desligando foco automatico
    cap.set(CAP_PROP_AUTOFOCUS, 0);

    if (!cap.isOpened()){  
        cout << "Error on open cam" << endl;  
        return -1;  
    } 


    dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    namedWindow("QR - Reader", WINDOW_NORMAL);

    //Largura, altura
    resizeWindow("QR - Reader", 700, 800);
    moveWindow("QR - Reader", 850, 0);

    int d = 1;
    int s = 0;
    int c = 0;
    int k = -1;
    //
    int count = 0;
    //
    int count2 = 0;

    while(true){

        cap.read(img);

        cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
       
        if(k == 'n'){
            s = 0;
        }

        if(k == 's'){
            s = 1;
        }

        if(k == 'h'){
            s = 2;
        }

        if(k == 'q'){
            s = 3;
        }

        if(k == 'c'){
            c = 1;
        }

        if(k == 'x'){
            c = 0;
        }
        
        if(s == 1){
            grayImg = segmentation(grayImg, 2);
        } else if(s == 2){
            grayImg = histogramEqualization(grayImg);
        } else if(s == 3){
            grayImg = histogramEqualization(grayImg);
            grayImg = segmentation(grayImg, 2);
        }

        int width = grayImg.cols, height = grayImg.rows;
        uchar *raw = (uchar *) grayImg.data;
        //Parametros largura, altura, formato, dado, tamanho
        Image image(width, height, "Y800", raw, width * height);
        //Scaneia a imagem
        scanner.scan(image);

        for(
            Image::SymbolIterator symbol = image.symbol_begin();
            symbol != image.symbol_end(); 
            ++symbol
        ){
            vector<Point> vp;
            data = symbol->get_data();
            saveDataInFile(fileName.c_str(), data); 


            cout << "Dado lido!!!" << endl;


            if(aes){
                decryptFileWithAES(fileName, newFileName, keyName);
            } else {
                decryptFileWithRsa(fileName, newFileName, keyName);
            }
            
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

                //Linha preta
                line(grayImg,pts[i],pts[(i+1)%4],Scalar(0,0,0),3);
            }

        }

        imshow("QR - Reader", grayImg);

        k = waitKey(30);
        if (k == 27){  
            cout << "A tecla ESQ foi pressionada pelo usuario" << endl;  
            return 0;  
        }
    }
}

int main(int argc, char *argv[]){
    
    int input = 0;

    while(input != -1){
        cout << "Digite 1 para iniciar o leitor, ou 2 para gerar o QR Code, ou -1 para sair" << endl;
        cin >> input;

        switch(input){
            case -1:
                cout << "Saindo do programa" << endl;
                break;
            case 1: 
                cout << "Inicia o leitor" << endl;
                readQrCode("file-example-enc", "file-example-dec", "camera", false);
                break;
            case 2: 
                cout << "Gera os dados" << endl;
                generateKeys("camera", "192");
                encryptFileWithRsa("file-example", "f-enc-rsa", "camera");
                generateQrCode("qrcode", "f-enc-rsa");
                break;

            default:
                cout << "Opção incorreta" << endl;
                break;

        }
    }



    //AES
    //encryptFileWithRsa("file-example", "f-enc-aes", "aesPass");
    //generateQrCode("placaaes", "f-enc-rsa");


    //encryptFileWithAES("file-example.txt", "f-enc.txt", "aesPass.txt");


    
    //generateQrCode("placarsa", "f-enc-rsa.txt");
    //generateQrCode("pjsonpaes128-s10-cl", "f-enc.txt");

    return 0;
}