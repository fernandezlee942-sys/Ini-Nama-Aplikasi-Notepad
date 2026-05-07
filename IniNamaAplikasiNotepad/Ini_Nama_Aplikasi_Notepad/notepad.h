#ifndef NOTEPAD_H
#define NOTEPAD_H

// Bawaan
#include <QMainWindow>


#include <QFile>
#include <QFileDialog>
#include <QTexhfghtStream>
#include <QMessageBox>

#include <QPrinter>
#include <QPrintDialog>

#include <QLabel>


QT_BEGIN_NAMESPACE

// g usah diambil pusing, ini intinya komponen ui notepad dimasukkan disebuah namespace ui ini, buka notepad,ui di mode editor dan tanya gpt utk lbh lanjut, "Untuk alasan apapun jangan coba edit namespace ini dari Ui jadi abcd atau semacamnya nanti error dont ask me why i dont understand wht this gpt kid is saying"
//intinya namespace Ui ini jgn disentuh deh

namespace Ui {
class Notepad;
}

QT_END_NAMESPACE


class Notepad : public QMainWindow
// class inheritance, the class notepad now had all the components that is in the QMainWindow
// declaring that the notepad class will be our main windows (most of this code is automatically generated when we first make the project so dont mind it)
{
    Q_OBJECT // blg kalo class notepad kt ini class object qt
    // Q_OBJECT ini contoh macro, macro apaan? ia kek fungsi cmn g perlu (), apparently ini fungsinya ribet kali dan biar usernya gampang pake mereka buat biar use cukup ketik Q_OBJECT. intinya ini ada untuk blg kalo "ini bukan class c++ biasa jdi kasi fitur2 dari class object qt (signal, slot) kesini"

    // MOC (Meta Object Compiler)
    // Kamu: Mengetik Q_OBJECT di file notepad.h.
    // MOC: Melihat Q_OBJECT, lalu membuat file pendamping moc_notepad.cpp yang berisi logika Signal/Slot.
    // Compiler C++: Menggabungkan file buatanmu (.cpp) dan file buatan MOC (moc_notepad.cpp) menjadi satu aplikasi utuh.






    //cari penjelasan ini nanti sama private dibwhny yg ad pointer ke uinya itu kekny msh ad hubungan dengan pimpl yang namespace ui itu
    //sekalian cari pengertian syntax2 di main.cpp
public:
    explicit Notepad(QWidget *parent = nullptr);
    //ciptakan object baru namanya notepad dengan argmen qwidget, dan blgin kalo ini objek parent (kalo dihapus semua yg ad jg ikut dihapus)
    //explicit di trk di class biar pas semisal sebuah perintah nerima argumen objek dengan class notepad dipanggil pake integer(anggap aja kau ngantuk dan asal ceplos) fungsinya g ubah int jadi objek notepad.
    //nullptr sama aja kek NULL bedanay cuman ini biar ksai tau kalo ia pointer bukan integer
    //fungsi konstruktor -> fungsi dalam class yang aktif saat kita buat objek dengan class yang punya fungsi konstruktor tsb
    ~Notepad() override;
    //fungsi dekonstruktor, dijalankan saat fungsi dihapus dari memori
    //ini fungsinya nanti bakal list segala fungsi delete yg ada tru dijalanin biar memori g bocor (variabel dari stack emg otomatis dihps cmn yg ad di heap harus tetap dimatikan manual)
    //beberapa objek kek ui teks dkk harus dipake terus ampe apk ditutup jadi g efektif buat hapus objek uinya cmn karna ga dipake sementara jadinya kita biarkan mereka di heap ampe apk dimatikan (karna itu kt butuh list apa aja yg mau dihapus di saat apk dimatikan ini guna fungsi ini)
    //override ini cuman buat debugging g gitu penting, intinya kalo kt ad class musuh dan ada objek namanya naga, si naga ini inherit class dari musuh ini, nah di class musuh misal ada fungsi namanya void serang() yg kerjanya cout "musuh menyerang", nah kalo kt mau panggil di void serang tapi di naga ini kita mau kasi "naga menyembur api", kita pake void serang() override { cout<<"naga menyembuat api"; } sekarang khusus di dalam naga fungsi serang bakal nyembur api klo semisal kt mau override fungsi yang g ada (anggap typo) ia bakal kasi tau kalo fungsinya g ada dan error
    //as for why we use it in the deconstructor ask gpt i've given up onto learning it



private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_as_triggered();

    void on_actionPrint_triggered();

    void on_actionExit_triggered();

    void on_actionCopy_triggered();

    void on_actionCut_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

private:
    Ui::Notepad *ui;


    QString currentFile = "";
};
#endif // NOTEPAD_H
