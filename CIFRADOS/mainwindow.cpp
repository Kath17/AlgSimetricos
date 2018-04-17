#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "saferk64.h"
#include "rijndael.h"

#include <cstring>
#include <string>
#include <sstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_activated(int index)
{
    if (index == 0)
    {
        ui->textEdit_3->setText("Hola soy el Rijndael");
        RoS = false;

    }
    else if (index == 1)
    {
        ui->textEdit_3->setText("Hola soy el SAFER");
        RoS = true;
    }
}

QString getStringFromUnsignedChar( unsigned char *str ){
    QString result = "";
    int lengthOfString = strlen( reinterpret_cast<const char*>(str) );

    // print string in reverse order
    QString s;
    for( int i = 0; i < lengthOfString; i++ ){
        s = QString( "%1" ).arg( str[i], 0, 16 );

        // account for single-digit hex values (always must serialize as two digits)
        if( s.length() == 1 )
            result.append( "0" );

        result.append( s );
    }

    return result;
}

vector<string> Separar(char* texto)
{
    vector <string> Bloques;
    int longitud = strlen(texto);

    string resultado="";
    int j=1;
    for (int i=0; i<longitud; i++)
    {
        char character = texto[i];
        if(character=='-')
            j++;
        string str(1,character);
        resultado = resultado + str;
        if(j==9 || i+1 == longitud)
        {
            Bloques.push_back(resultado);
            resultado ="";
            j=1;
        }
     }
    return Bloques;
}

vector<string> Separar2(char* texto)
{
    vector <string> Bloques;
    int longitud = strlen(texto);

    string resultado="";
    int j=1;
    for (int i=0; i<longitud; i++)
    {
        char character = texto[i];
        if(character=='-')
            j++;
        string str(1,character);
        resultado = resultado + str;
        if(j==16 || i+1 == longitud)
        {
            Bloques.push_back(resultado);
            resultado ="";
            j=1;
        }
     }
    return Bloques;
}

void MainWindow::on_pushButton_clicked()
{
    QString texto = ui->textEdit->toPlainText();
    QString llav = ui->textEdit_2->toPlainText();

    // ------------------------ RIJNDAEL -----------------------//
    if(RoS == false){

        std::cout << "TEXTO"  << texto.toStdString() <<std::endl;
        std::cout << "LLAVE"  << llav.toStdString() <<std::endl;
        QString hex =string_to_hex(texto);
        string tmp = hex.toStdString();
        char *hex2 = new char[tmp.length()+1];
        std::strcpy(hex2, tmp.c_str());
        vector<string> separado2 = Separar2(hex2);

        for(int i = 0; i<separado2.size();i++)
        {
            std::cout << separado2[i];

        }

        rounds = 256;

        keyLength = rounds/32;
        rounds = keyLength + 6;

        for(int i=0; i<separado2.size();i++)
        {

            unsigned char c[16];


            for(int i=0, j=0; i<separado2[i].length();i++, j++)
            {

                //char *hex="f7";
                char *hex = new char[2];
                std::strcpy(hex,separado2[i].substr(i,2).c_str()); ;
                int d;
                sscanf(hex,"%02x", &d);
                c[j]=(unsigned char)d;
                printf("%s %02x\n", hex, c[j]);
                i++;
            }

            for (int i=0; i<16; i++)
            {
                plaintext[i]=c[i];
            }


            Expand_Keys();
            Encrypt();

            printf("\nEncrypted: \n");
                //cout << encrypted;
                for(i = 0; i < keyLength * blockSize; i++)
                {
                    printf("%02x ", encrypted[i]);
                }
                printf("\n\n");

        }



    }

    // --------------------------SAFER --------------------------//
    else{

        string plainText = texto.toStdString();
        string llave = llav.toStdString();

        char *encriptado = new char[plainText.length()+1];
        char *llavedes = new char[llave.length()+1];

        std::strcpy(encriptado, plainText.c_str());
        std::strcpy(llavedes, llave.c_str());

        short key[4], S[512], S_inv[512];
        K = (uchar **) calloc(2 * ROUNDS + 3, sizeof(char *));
        for (i = 0; i < 2 * ROUNDS + 3; i++) {
            K[i] = (uchar *) calloc(9, sizeof(char));
        }
        key[0] = 256 * 8 + 7;
        key[1] = 256 * 6 + 5;
        key[2] = 256 * 4 + 3;
        key[3] = 256 * 2 + 1;

        vector<string> bloques;
        bloques = Separar(encriptado);

        char *encriptado2;
        string resultado;
        QString desencriptado;

        cout<<"Size:"<<bloques.size();

        for(int i=0; i<bloques.size();i++)
        {
            encriptado2 = new char[bloques[i].length()+1];
            std::strcpy(encriptado2, bloques[i].c_str());
            cout<<bloques[i]<<endl;

            check_user_inputs(encriptado2, llavedes);
            SAFER_K_64_key_schedule(key, S, S_inv, K);
            SAFER_K_64_decryption(X, Y, S, S_inv, K);

            for (int j = 1; j <= 8; j++) {
                char decrypted = Y[j] + '0';
                if (decrypted != '0') {
                    string str(1,decrypted);
                    cout<<str;
                    resultado = resultado + str.c_str();
                }
            }
            cout<<"Resultado:"<<resultado<<endl;
            uchar X[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
            uchar Y[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        }

        printf("Texto a desencriptar: %s \n", plainText.c_str());
        printf("Llave: %s \n", llave.c_str());
        printf("Texto desencriptado: \n");
        printf("______________________________\n");

        desencriptado = QString::fromStdString(resultado);
        ui->textEdit_3->setText(desencriptado);
        printf("\n‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾");
    }

}
