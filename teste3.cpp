//Bibliotecas nativas do sistema
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

void generateKeys(string keyName){
    string genPrivKey = "openssl genrsa -out keys/"+ keyName +".pri 2048";
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

void encryptFile(string fileName, string newFileName, string keyName){
    string encrypt = "openssl rsautl -encrypt -inkey keys/"+ keyName +".pub -pubin -in "+ fileName +" -out encBinary.txt";
    system(encrypt.c_str());
    string encryptB64= "base64 encBinary.txt >"+newFileName;
    system(encryptB64.c_str());
}

void decryptFile(string fileName, string newFileName, string keyName){
    ifstream file;
    file.open(newFileName.c_str());
    //Se não existir o arquivo decriptografado, geramos ...
    if(!file.good()){
        string decryptB4 = "base64 --decode " +fileName+ ">decBinary.txt";
        system(decryptB4.c_str());
        string decrypt = "openssl rsautl -decrypt -inkey keys/"+ keyName +".pri -in decBinary.txt -out "+newFileName;
        //cout << decrypt << endl;
        system(decrypt.c_str());
    }    
    file.close();
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
        cout << "Problemas ao abrir o arquivo";
    }

}

void generateQrCode(string qrCodeName, string fileName){
    string data = getDataFromFile(fileName.c_str());
    //data = "\"" + data +"\"";
    string generateQrCode = "qrencode \"" + data + "\" -o qrCodes/"+qrCodeName+".png";
    //cout << generateQrCode << endl;
    system(generateQrCode.c_str());
}

int readQrCode(int argc, char *argv[], string fileName, string newFileName,string keyName){
    int exit = 1;
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
    while(exit){
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

            saveDataInFile(fileName.c_str(), data);            
            decryptFile(fileName, newFileName, keyName);

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
            //break;
            //exit = 0;
        }
        imshow("Leitor", grayImg);
        if (waitKey(30) == 27){  
            cout << "esc key is pressed by user" << endl;  
            return 0;  
        }
    }
}

int main(int argc, char *argv[]){
    generateKeys("camera");
    //generateQrCode("qrCode1", "file-example-enc.txt");
    //encryptFile("file-example.txt", "file-example-enc.txt", "camera");
    //decryptFile("file-example-enc.txt", "file-example-dec.txt", "camera");
    //generateQrCode("qrCode1", "file-example-enc.txt");
    readQrCode(argc, argv, "file-example-enc.txt", "file-example-dec.txt", "camera");
    //

	return 0;
}