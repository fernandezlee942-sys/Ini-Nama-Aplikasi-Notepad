#include "notepad.h"
#include "./ui_notepad.h"
#include "timer.h"
#include "calculator.h"
#include "calendar.h"
#include <QPalette>
#include <QStyle>


#include <QIcon>
#include <QString>
#include <QPushButton>

// if you see smtg like label->show(), do not be scared of it cos it just means tht the object label is a pointer, most of the time here we use pointer ask gpt why

// a bit of theory, ram itu ada stack (tumpukan, pake last in first out sgt cepat, tpi kecil dan terbatas dipake buat sesuatu ke int x=5; begitu ketemu } ia bakal langsung dihapus memori) dan heap(tumpuka besar/acak, memori yg lbh luas dan bebas, ia berantakan dan g berurut, lbh lambat dri stack diapke untuk sesuatu yg ada new-nya kek, new QLabel(), ia g bakal hilang kecuali kt panggil delete)



Notepad::Notepad(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::Notepad)
{

    ui->setupUi(this);
    this -> setCentralWidget(ui->textEdit);
    //make the text edit in design mode fill the whole screen
    //not sure myself of how it works
}


//Buka projek dri ide jgn lewat github nnti g jalan

//for those trying to open the resourcesnotepad.qrc, right click on the qrc file and open with editor idk why but this fkin app is too dumb to let us double click the qrc file once it become folder (not even sure why they even let it work as a folder in the first place)


//whenever you add a image in the resource to put it in the icon folder or maybe make a new folder but in the same path as the icon folder cos this app is too dumb to just copy the file from our local disk to the folder where the project is and will try to just save the image path from ur computer locally
//to put it simply : put image or any other asset in the same path as icons, or else we, the other collaborator cant use the asset and it is likely the app wont run as intended


//another note, this app is too dumb to update their <resource root> folder so if you dont find ur the folder you make for the asset, try tapping the reset icon


// NB : ctrl + / cmn jalan kalo dh ad teks beda dgn vscode yg lgsg ksi // walau barisnya kosong



Notepad::~Notepad()
{
    delete ui;
}


// Tutor Youtube: ( add it urself if you find the video useful )
// https://www.youtube.com/watch?v=I96uPDifZ1w&t=283s


void Notepad::on_actionNew_triggered()
{
    currentFile.clear();
    ui->textEdit ->setText(QString());
    //make new file
}


void Notepad::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(filename);
    currentFile = filename;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Cannot open file : "+file.errorString());
        return;
    }

    setWindowTitle(filename);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();

}

void Notepad::on_actionSave_as_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Cannot save file : "+file.errorString());
        return;
    }
    currentFile = filename;
    setWindowTitle(filename);
    QTextStream out (&file);
    QString text = ui->textEdit->toPlainText();
    out<<text;
    file.close();

}

//for me tommorow (bgn pagi dikit, ini yg dibwh ini jgn lupa trk header perlu kalo mau pake library buat print), tutorial kemarin ampe
// 32:20


void Notepad::on_actionPrint_triggered()
{
    QPrinter printer;
    printer.setPrinterName("Printer Name ");
    QPrintDialog pDialog(&printer, this);
    if(pDialog.exec()==QDialog::Rejected){
        QMessageBox::warning(this, "Warning", "Cannot Access Printer");
        return;
    }
    ui->textEdit->print(&printer);
}


void Notepad::on_actionExit_triggered()
{
    QApplication::quit();
}

void Notepad::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void Notepad::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void Notepad::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void Notepad::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void Notepad::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

void Notepad::on_actionTimer_triggered()
{
    Timer *tmr = new Timer(nullptr);
    tmr->setAttribute(Qt::WA_DeleteOnClose);
    tmr->show();
}

void Notepad::on_actionCalculator_triggered()
{
    Calculator *clc = new Calculator(nullptr);
    clc->setAttribute(Qt::WA_DeleteOnClose);
    clc->show();
}

void Notepad::on_actionCalendar_triggered()
{
    Calendar *cld = new Calendar(nullptr);
    cld->setAttribute(Qt::WA_DeleteOnClose);
    cld->show();
}


void Notepad::on_actionLight_triggered()
{
    QPalette lightPalette;

    // Tentukan warna tema terang racikan sendiri (Clean White & Grey)
    QColor latarBelakangUtama(245, 245, 245);    // Abu-abu sangat muda
    QColor latarBelakangKetik(255, 255, 255);    // Putih bersih
    QColor warnaTeksHitam(30, 30, 30);           // Hitam elegan (tidak terlalu pekat)
    QColor latarBelakangTombol(230, 230, 230);   // Abu-abu untuk button
    QColor abuAbuMuda(200, 200, 200);

    // Tuangkan warna ke elemen-elemen Qt
    lightPalette.setColor(QPalette::Window, latarBelakangUtama);
    lightPalette.setColor(QPalette::WindowText, warnaTeksHitam);
    lightPalette.setColor(QPalette::Base, latarBelakangKetik);     // Tempat ngetik text edit
    lightPalette.setColor(QPalette::AlternateBase, latarBelakangUtama);
    lightPalette.setColor(QPalette::ToolTipBase, latarBelakangKetik);
    lightPalette.setColor(QPalette::ToolTipText, warnaTeksHitam);
    lightPalette.setColor(QPalette::Text, warnaTeksHitam);         // Teks hasil ketikan
    lightPalette.setColor(QPalette::Button, latarBelakangTombol);
    lightPalette.setColor(QPalette::ButtonText, warnaTeksHitam);   // Teks di dalam tombol
    lightPalette.setColor(QPalette::PlaceholderText, abuAbuMuda);  // Teks petunjuk (hint)

    // Terapkan secara global ke aplikasi
    qApp->setPalette(lightPalette);
    updateAllIcons(false);
}

void Notepad::on_actionDark_triggered()
{
    QPalette darkPalette;

    // Tentukan warna tema gelap racikan sendiri (Dark Charcoal & Obsidian)
    QColor latarBelakangUtama(40, 40, 40);       // Abu-abu tua/gelap
    QColor latarBelakangKetik(25, 25, 25);       // Lebih gelap untuk area mengetik
    QColor warnaTeksPutih(240, 240, 240);        // Putih cerah
    QColor latarBelakangTombol(55, 55, 55);      // Abu-abu sedang untuk button
    QColor abuAbuTua(100, 100, 100);

    // Tuangkan warna ke elemen-elemen Qt
    darkPalette.setColor(QPalette::Window, latarBelakangUtama);
    darkPalette.setColor(QPalette::WindowText, warnaTeksPutih);
    darkPalette.setColor(QPalette::Base, latarBelakangKetik);      // Tempat ngetik text edit
    darkPalette.setColor(QPalette::AlternateBase, latarBelakangUtama);
    darkPalette.setColor(QPalette::ToolTipBase, latarBelakangKetik);
    darkPalette.setColor(QPalette::ToolTipText, warnaTeksPutih);
    darkPalette.setColor(QPalette::Text, warnaTeksPutih);          // Teks hasil ketikan
    darkPalette.setColor(QPalette::Button, latarBelakangTombol);
    darkPalette.setColor(QPalette::ButtonText, warnaTeksPutih);    // Teks di dalam tombol
    darkPalette.setColor(QPalette::PlaceholderText, abuAbuTua);    // Teks petunjuk (hint)

    // Terapkan secara global ke aplikasi
    qApp->setPalette(darkPalette);
    updateAllIcons(true);
}

// Helper function to swap between _W and _B based on the target theme
QIcon getThemedIcon(QString originalPath, bool isDarkTheme)
{
    // Clean up path just in case
    originalPath = originalPath.trimmed();

    if (isDarkTheme) {
        // If it contains _W, replace it with _B
        if (originalPath.contains("_W")) {
            originalPath.replace("_W", "_B");
        }
    } else {
        // If it contains _B, replace it with _W
        if (originalPath.contains("_B")) {
            originalPath.replace("_B", "_W");
        }
    }

    return QIcon(originalPath);
}

void Notepad::updateAllIcons(bool isDark)
{
    // Fix Window Frame / Taskbar Icon
    // Jika gelap pakai logo _B, jika terang pakai logo _W
    QString windowIconPath = ":/IMAGE/Icons/main_logo";
    windowIconPath += (isDark ? "_B.png" : "_W.png");
    this->setWindowIcon(QIcon(windowIconPath));

    // Ambil list tombol dengan tanda 'const' agar aman dari detach
    const QList<QAction*> semuaTombol = this->findChildren<QAction*>();

    // Tambahkan const pada pointer di dalam loop
    for (QAction* const tombol : semuaTombol) {
        QString namaObjek = tombol->objectName();

        // Skip default system buttons that don't have custom icons
        if (namaObjek.isEmpty() || namaObjek.startsWith("qt_")) {
            continue;
        }

        // Match image name dynamically to button name (e.g., ":/logos/btn_save_B.png")
        QIcon ikonSaatIni = tombol->icon();
        if (ikonSaatIni.isNull()) {
            continue;
        }

        QString pathAsli = tombol->property("originalIconPath").toString();

        // Jika pertama kali dijalankan, kunci path dasar khusus untuk tombol ini saja
        if (pathAsli.isEmpty()) {
            QString namaBersih = namaObjek;
            if (namaObjek.startsWith("action", Qt::CaseInsensitive) && namaObjek.length() > 6) {
                namaBersih = namaObjek.mid(6);
            }

            QString baseKapital = ":/IMAGE/Icons/" + namaBersih;
            QString baseKecil = ":/IMAGE/Icons/" + namaBersih.toLower();

            // Kunci path berdasarkan file yang BENAR-BENAR ada untuk tombol ini
            if (QFile::exists(baseKapital + "_B.png") || QFile::exists(baseKapital + "_W.png") ||
                QFile::exists(baseKapital + "_b.png") || QFile::exists(baseKapital + "_w.png")) {
                pathAsli = baseKapital;
            }
            else if (QFile::exists(baseKecil + "_B.png") || QFile::exists(baseKecil + "_W.png") ||
                     QFile::exists(baseKecil + "_b.png") || QFile::exists(baseKecil + "_w.png")) {
                pathAsli = baseKecil;
            }

            if (!pathAsli.isEmpty()) {
                tombol->setProperty("originalIconPath", pathAsli);
            }
        }

        // Eksekusi penukaran ikon secara ketat (Dinding pemisah Black dan White)
        if (!pathAsli.isEmpty()) {
            QString pathFinal = "";

            if (isDark) {
                // Tema Gelap (Black) -> Hanya cari yang berakhiran _B atau _b
                if (QFile::exists(pathAsli + "_B.png"))       pathFinal = pathAsli + "_B.png";
                else if (QFile::exists(pathAsli + "_b.png"))  pathFinal = pathAsli + "_b.png";
            }
            else {
                // Tema Terang (White) -> Hanya cari yang berakhiran _W atau _w
                if (QFile::exists(pathAsli + "_W.png"))       pathFinal = pathAsli + "_W.png";
                else if (QFile::exists(pathAsli + "_w.png"))  pathFinal = pathAsli + "_w.png";
            }

            // Jika file tema yang dituju ada, pasang langsung!
            if (!pathFinal.isEmpty()) {
                tombol->setIcon(QIcon(pathFinal));
            }
        }
    }
}