// git push origin branch_name
// git status
// git fetch
// git pull
// git commit -m "update001"
// git push origin branch_name
// git branch
// git switch branch_name
// git log, q buat stop ctrl c g bs nth knp
// git log --oneline (biar g perlu tekan enter trus)
// git push -f



// Buat yg ikut tutorial youtube tpi sadar kt g ada file .pro itu karna kt pake Cmake bukan qmkae, google utk detail lebih
// Link youtube to learn how to use CMakeLists.txt (havent watched it myself)
// https://www.youtube.com/watch?v=OigQPU2ukss
#include <QStyleFactory>
#include "notepad.h"
#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyle>
#include <QImageReader>
//widgets, event handling, mouse movement

int main(int argc, char *argv[])
{
// argc -> argument count tells the computer how many items werer typed into the command line to start the program
// *argv[] -> array char (string) untuk alasan pake pointer karna kkt blom tau jlh char yg dipilih usernya, kek semisal nama apknya ./Notepad

// You may ask, why bother giving the argument in the int main if we're gonna use it in the qapplication instead of the main function, i tried googling it and its bcos the argc and argv is a data made by the os and due to how cpp is made you cant bring out a argument from outside unless you take it from main function

// singkatnya int argc dan argv itu biar bs dioper ke QApplication
// QApplication dri command yang kita tulis (argc dan argv itu command yg kt tulis di terminal/shell versi yang dipahami komputernya) bakal jalanin programnya sesuai apa yg kt tulis cth : ./Notepad notes.txt --darkmode, ia bakal jalanin Notepad.exe (argc[0] argumen ke 1), buka file notes.txt (argv[1] argumen ke 2) dan jalaninnya di darkmode (argv[2] argumen ke 3)


// If you are using a terminal (like Termux or a Windows CMD) to run your Notepad app, you might type:./Notepad notes.txt --darkmode
// intinya semisal kt mau jalanin file executeable (ini buat yg pake linux dkk yg suka ketik sendiri beda dgn windows yg cmn modal klik, ia bakal tampung jlh string yg ada dan baca stringnya)

// The computer splits that sentence by the spaces and hands it to your main function like this:
// Argument         Value             Index
// Program Name     ./Notepad         argv[0]
// First Extra      notes.txt         argv[1]
// Second Extra     --darkmode        argv[2]

    QApplication a(argc, argv);
    // ini g perlu diperhatiin, kerjanya cmn kek jalanin aplikasi sesuai apa yang kita ketik

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    Notepad w;
    // Masukin data berkaitan dengan notepad kt di ram
    // Notepad itu class (cek notepad.h), class Notepad itu perlu variabel ini kebetulan ditls w g hrs w si






    // Detect if the user's system background is dark or light
    QColor bgOS = a.style()->standardPalette().color(QPalette::Window);
    bool systemIsDark = (bgOS.value() < 128);


    if (systemIsDark) {
        // If system is dark, boot application in Dark Colors right away
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(40, 40, 40));
        darkPalette.setColor(QPalette::WindowText, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::Text, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::Button, QColor(55, 55, 55));
        darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
        a.setPalette(darkPalette);
    }
    else {
        // ☀️ If system is light, boot application in Light Colors right away
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor(245, 245, 245));
        lightPalette.setColor(QPalette::WindowText, QColor(30, 30, 30));
        lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));     // Area ketik notepad
        lightPalette.setColor(QPalette::Text, QColor(30, 30, 30));         // Warna font teks
        lightPalette.setColor(QPalette::Button, QColor(230, 230, 230));
        lightPalette.setColor(QPalette::ButtonText, QColor(30, 30, 30));
        a.setPalette(lightPalette);
    }

    // RUN THE MASTER AUTOMATION RIGHT HERE
    // Ini akan otomatis mengirimkan nilai true (ke _B.png) atau false (ke _W.png)
    w.updateAllIcons(systemIsDark);



    qDebug() << QImageReader::supportedImageFormats();

    w.show();
    // Tampilin apa yg dh ad diram




    //test

    // the hello world version of qt, you may be wondering whts tht label->show(); baca di bagian atas notepad.cpp itu ad penjelasan super duper panjangnya
    // QLabel *label = new QLabel("Hello World!");
    // label->resize(400,300);
    // 400 lebar, 800 panjang
    // label->setWindowTitle("Fernandez Ganteng 123");
    // label->show();





    return QCoreApplication::exec();
    //ini kerjanya jaga event loop (biasa pas kode dh siap ia bakal lgsg end kan, nah ini event loop buat cegah notepad dimatikan sesuka hati
    //intinya ia selalu tanya ke komputer tiap detik, ada perintah baru ga kalo ad perintah save jalankan save klo ad perintah load jalankan load kalo ad perintah exit keluar dkk

    // do you know how we always use using namespace std so we dont have to write std::cout<<"";? its smtg like tht i dont recommand using namespace since you wont know from wht the function comes from

    // namespace Teknik {
    // void sapa() { std::cout << "Halo dari Teknik!"; }
    // }
    // Teknik::sapa();

    // namespace Hukum {
    // void sapa() { std::cout << "Halo dari Hukum!"; }
    // }
    // Hukum::sapa();

    //smtg like tht

}


// You dont normally temper with main.cpp all of this comments is just in case ur curious abt wht's happening here


// Jika mau coba di cpp kek test print label komentari Notepad w; ampe w.show(); trus buat kodenya di main.cpp baru di runa