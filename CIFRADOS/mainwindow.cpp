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
        string str(1,character);
        resultado = resultado + str;

        j++;
        if( j==33 || i+1 == longitud )
        {
            Bloques.push_back(resultado);
            resultado = "";
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

        std::cout << "TEXTO: "  << texto.toStdString() <<std::endl;
        std::cout << "LLAVE: "  << llav.toStdString() <<std::endl;


        //Verificar tamaño de llave.
        if( llav.size()%32 != 0  )
        {

            ui->textEdit_2->setText("Inserte una llave de 256 bits (32 caracteres)");

        }
        else
        {

            //Tamaño de llave por defecto.
            rounds = 256; std::cout << "Tamaño de la llave:" << rounds << std::endl;
            keyLength = rounds/32; std::cout << "Tamaño de la llave:" << keyLength << std::endl;
            rounds = keyLength + 6; std::cout << "Rondas:" << rounds << std::endl;
            //CONVERTIR LLAVE EN HEXADECIMAL.
            QString hex_llav =string_to_hex(llav);
            string tmp_llav = hex_llav.toStdString();
            std::cout << "LLAVE HEX: " << tmp_llav << std::endl;
            unsigned char lla[32];

            for( int i=0, j=0; i<tmp_llav.size(); i++, j++  )
            {
                char *hex2 = new char[2];
                std::strcpy(hex2, tmp_llav.substr(i,2).c_str() );
                int d;
                sscanf(hex2, "%02x",&d);
                lla[j] =(unsigned char)d;
                printf("%02x ",lla[j]);
                i++;
            }
            std::cout << std::endl;
            //Guardar los datos convertidos a hex al array usado para encriptar.
            for (int i=0; i<32; i++)
            {
                Key[i]=lla[i];
            }



            //SEPARAR TEXTO EN BLOQUES
            QString hex =string_to_hex(texto);
            string tmp = hex.toStdString();
            std::cout << "TEXTO HEX: " << tmp << std::endl;

            char *hex2 = new char[tmp.length()+1];
            std::strcpy(hex2, tmp.c_str());
            vector<string> separado2 = Separar2(hex2);
            std::cout << "Bloques: " << separado2.size() << endl;

            for(auto i:separado2)
            {
                std::cout << "Tamaño: " << i.size() << std::endl;
                std::cout << i << std::endl;

            }

            string encript="";



            for(int b=0 ; b<separado2.size(); b++  )
            {


                unsigned char c[16];

                for( int i=0, j=0; i<separado2.at(b).size(); i++, j++  )
                {
                    char *hex = new char[2];
                    std::strcpy(hex, separado2.at(b).substr(i,2).c_str() );
                    int d;
                    sscanf(hex, "%02x",&d);
                    c[j] =(unsigned char)d;
                    printf("%02x ",c[j]);
                    i++;
                }

                //Guardar los datos convertidos a hex al array usado para encriptar.
                for (int k=0; k<16; k++)
                {
                    plaintext[k]=c[k];
                }


                Expand_Keys();
                Encrypt();

                //convirtiendo a string lo encriptado
                printf("\nEncrypted: \n");

                for(int m = 0; m < keyLength * blockSize; m++)
                {
                    printf("%02x ", encrypted[m]);
                    encript = encript  + string(1,char(encrypted[m]));
                }
                printf("\n\n");



                QString hexENCR =string_to_hex( QString::fromStdString(encript));
                string tmpENCR = hexENCR.toStdString();
                std::cout << "TEXTO HEX: " << tmpENCR << std::endl;


            }



            ui->textEdit_3->setText(QString::fromStdString(encript));

            /*
            Expand_Keys();
            Decrypt();

            printf("\nDecrypted: \n");

            for (i = 0; i < blockSize * 4; i++)
            {
                printf("%02x ", decrypted[i]);
            }
            printf("\n\n");
            */
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
