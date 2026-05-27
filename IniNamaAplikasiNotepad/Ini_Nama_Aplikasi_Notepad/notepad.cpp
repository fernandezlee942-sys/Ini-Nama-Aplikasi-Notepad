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


#include <QClipboard>
#include <QMimeData>

#include <QDateTime>
#include <QUrl>

#include <QFileInfo>
#include <QBuffer>
// if you see smtg like label->show(), do not be scared of it cos it just means tht the object label is a pointer, most of the time here we use pointer ask gpt why

// a bit of theory, ram itu ada stack (tumpukan, pake last in first out sgt cepat, tpi kecil dan terbatas dipake buat sesuatu ke int x=5; begitu ketemu } ia bakal langsung dihapus memori) dan heap(tumpuka besar/acak, memori yg lbh luas dan bebas, ia berantakan dan g berurut, lbh lambat dri stack diapke untuk sesuatu yg ada new-nya kek, new QLabel(), ia g bakal hilang kecuali kt panggil delete)



Notepad::Notepad(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::Notepad)
{

    ui->setupUi(this);

    on_actionNew_triggered();
    //make the text edit in design mode fill the whole screen
    //not sure myself of how it works
}




// Tambahkan ini di bagian paling bawah file notepad.cpp

QTextEdit* Notepad::getActiveEditor()
{
    // 1. Ambil halaman tab yang sedang aktif di layar
    QWidget *currentWidget = ui->tabWidget->currentWidget();
    if (!currentWidget) return nullptr;

    // 2. Trik Jitu: Cek apakah halaman aktif itu SENDIRI adalah sebuah QTextEdit
    // (Ini akan lolos karena kamu memasukkan newEditor langsung ke addTab)
    QTextEdit *editor = qobject_cast<QTextEdit*>(currentWidget);

    // 3. Jika ternyata gagal, baru cari ke dalam anak-anaknya menggunakan findChild
    if (!editor) {
        editor = currentWidget->findChild<QTextEdit*>();
    }

    // 4. Kembalikan objek yang berhasil ditemukan ke fungsi Save As
    return editor;
}

// cegah leak memory
void Notepad::on_tabWidget_tabCloseRequested(int index)
{
    // Cek dulu apakah tab yang mau ditutup ini adalah tab terakhir yang tersisa
    if (ui->tabWidget->count() == 1) {
        // Jika ini tab terakhir, alihkan langsung ke fungsi Exit utama kita
        // agar dilakukan pengecekan status simpan (isModified) secara otomatis!
        on_actionExit_triggered();
        return;
    }

    // Jika tab yang ditutup bukan yang terakhir (misal masih ada 3 tab lain),
    // hapus tab yang diklik seperti biasa
    QWidget *tabPage = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    if (tabPage) {
        tabPage->deleteLater();
    }
}

void Notepad::on_actionExit_triggered()
{
    // Cek seluruh tab secara bergantian sebelum benar-benar menutup aplikasi
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        QWidget *tabPage = ui->tabWidget->widget(i);
        if (!tabPage) continue;

        QTextEdit *editor = tabPage->findChild<QTextEdit*>();

        // Jika tidak ketemu lewat findChild, cek apakah widget tab itu sendiri adalah QTextEdit
        if (!editor) {
            editor = qobject_cast<QTextEdit*>(tabPage);
        }

        // Jika ada tab yang isinya sempat diubah user dan belum di-save (isModified == true)
        if (editor && editor->document()->isModified()) {
            ui->tabWidget->setCurrentIndex(i); // Pindahkan fokus layar ke tab yang belum disimpan tersebut

            // Ambil nama tab bersih (buang karakter bintang jika ada)
            QString tabName = ui->tabWidget->tabText(i).remove("*");

            QMessageBox::StandardButton balasan;
            balasan = QMessageBox::warning(this, "Simpan Perubahan?",
                                           QString("Dokumen '%1' telah diubah.\nIngin menyimpan sebelum keluar?").arg(tabName),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (balasan == QMessageBox::Save) {
                on_actionSave_as_triggered(); // Paksa buka dialog simpan dulu
                return; // Tunda exit agar user bisa menyelesaikan dialog simpan
            }
            else if (balasan == QMessageBox::Cancel) {
                return; // Batalkan total proses keluar, kembali ke aplikasi
            }
            // Jika memilih Discard, loop akan lanjut ke tab berikutnya tanpa menyimpan
        }
    }

    // Jika semua tab lolos pengecekan (semua sudah disimpan/bersih), matikan aplikasi secara aman
    QApplication::quit();
}

void Notepad::updateTabTitle()
{
    // Cari tahu editor mana yang memicu perubahan teks ini
    QTextEdit *activeEditor = qobject_cast<QTextEdit*>(sender());
    if (!activeEditor) return;

    // Cari indeks tab dari editor tersebut
    int index = ui->tabWidget->indexOf(activeEditor);
    if (index == -1) return;

    // Ambil judul tab saat ini
    QString currentTitle = ui->tabWidget->tabText(index);

    // Jika teks diubah dan belum ada tanda bintang (*), tambahkan bintang!
    if (activeEditor->document()->isModified()) {
        if (!currentTitle.endsWith("*")) {
            ui->tabWidget->setTabText(index, currentTitle + "*");
        }
    }
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
    QTextEdit *newEditor = new QTextEdit(this);
    newEditor->setText(QString());

    int tabIndex = ui->tabWidget->addTab(newEditor, "Untitled");

    // Pindahkan mata user ke tab yang baru dibuat tersebut
    ui->tabWidget->setCurrentIndex(tabIndex);
    // Ikat properti file lokal kosong ke dalam objek editor ini sendiri
    newEditor->setProperty("filePath", QString(""));

    connect(newEditor, &QTextEdit::textChanged, this, &Notepad::updateTabTitle);
}
// Pastikan #include <QDir> sudah ada di bagian paling atas notepad.cpp bersama include lainnya

void Notepad::on_actionSave_as_triggered()
{
    // Ambil objek teks editor yang saat ini sedang aktif di depan layar
    QTextEdit *activeEditor = getActiveEditor();
    if (!activeEditor) return; // Antisipasi jika tidak ada tab yang terbuka

    // TENTUKAN PATH LOKAL & BUAT FOLDER JIKA BELUM ADA
    QString defaultPath = "C:/iniaplikasi notepad";
    QDir dir(defaultPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // DETEKSI GAMBAR: Periksa apakah di dalam dokumen QTextEdit terdapat objek gambar
    bool adaGambar = activeEditor->document()->toHtml().contains("<img");

    QString fileFilter;
    if (adaGambar) {
        // Jika ada gambar, batasi pilihan save hanya ke HTML dan PDF
        fileFilter = "HTML Files (*.html);;PDF Files (*.pdf)";

        // Beri tahu user/dosen lewat pop-up kecil agar UX aplikasi terlihat profesional
        QMessageBox::information(this, "Deteksi Gambar",
                                 "Dokumen mengandung gambar. Format simpanan dibatasi hanya ke HTML atau PDF agar gambar tidak hilang.");
    } else {
        // Jika teks biasa, buka semua format seperti biasa
        fileFilter = "Text Files (*.txt);;C++ Files (*.cpp);;Python Files (*.py);;HTML Files (*.html);;PDF Files (*.pdf)";
    }

    // Buka dialog penamaan file dengan filter yang sudah disesuaikan
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Save as",
                                                    defaultPath,
                                                    fileFilter);

    if (filename.isEmpty()) return; // Jika user menekan tombol 'Cancel'

    // EKSEKUSI KHUSUS UNTUK FORMAT PDF
    if (filename.endsWith(".pdf", Qt::CaseInsensitive)) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);
        activeEditor->print(&printer); // Qt otomatis merender teks + gambar ke dalam berkas PDF
    }
    // EKSEKUSI UNTUK FORMAT TEKS / HTML
    else {
        QFile file(filename);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
            return;
        }

        QTextStream out(&file);
        QString dataKonten;

        // Ambil data dalam bentuk HTML jika user memilih ekstensi .html
        if (filename.endsWith(".html", Qt::CaseInsensitive)) {
            dataKonten = activeEditor->toHtml(); // Gambar dikonversi ke kode biner Base64 di dalam HTML
        } else {
            dataKonten = activeEditor->toPlainText(); // Teks mentah biasa untuk .txt, .cpp, .py
        }

        out << dataKonten;
        file.close();
    }

    // Finalisasi status dokumen dan judul tab
    activeEditor->document()->setModified(false);

    QFileInfo fileInfo(filename);
    int currentIndex = ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(currentIndex, fileInfo.fileName());

    activeEditor->setProperty("filePath", filename);
}






void Notepad::on_actionOpen_triggered()
{
    // Ambil lokasi file dari storage lewat dialog box
    QString filename = QFileDialog::getOpenFileName(this, "Open the file");
    if (filename.isEmpty()) return; // Jika user menekan tombol 'Cancel' / batal buka file

    // Siapkan objek file untuk dibaca teksnya
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }

    // Ekstrak data teks dari file menggunakan stream reader
    QTextStream in(&file);
    QString text = in.readAll();
    file.close(); // Jangan lupa ditutup kembali filenya setelah dibaca

    // Lahirkan objek QTextEdit baru di memori Heap khusus untuk menampung file ini
    QTextEdit *newEditor = new QTextEdit(this);

    // PERBAIKAN DI SINI: Deteksi cara memuat dokumen berdasarkan ekstensi file
    if (filename.endsWith(".html", Qt::CaseInsensitive)) {
        newEditor->setHtml(text); // 🖼️ Render kode biner Base64 menjadi gambar asli di layar
    } else {
        newEditor->setPlainText(text); // 📄 Muat teks biasa tanpa render tag kode untuk .txt, .cpp, .py
    }

    // Potong alamat path panjang menjadi nama filenya saja untuk judul tab (Contoh: "tugas.txt")
    QFileInfo fileInfo(filename);
    QString shortName = fileInfo.fileName();

    // Masukkan editor baru tersebut ke dalam QTabWidget sebagai tab baru
    int tabIndex = ui->tabWidget->addTab(newEditor, shortName);

    // Langsung pindahkan fokus layar user ke tab file yang baru dibuka tersebut
    ui->tabWidget->setCurrentIndex(tabIndex);

    // KUNCI AMAN: Simpan alamat full path asli ke properti privat editor ini (pengganti currentFile)
    newEditor->setProperty("filePath", filename);

    // Hubungkan ke pendeteksi ketikan agar jika setelah dibuka teksnya diedit, bintang (*) bisa muncul
    connect(newEditor, &QTextEdit::textChanged, this, &Notepad::updateTabTitle);
}






void Notepad::on_actionPrint_triggered()
{
    // 1. Ambil editor yang sedang aktif di depan layar
    QTextEdit *activeEditor = getActiveEditor();
    if (!activeEditor) return; // Antisipasi jika tidak ada tab yang terbuka

    QPrinter printer;
    printer.setPrinterName("Printer Name ");
    QPrintDialog pDialog(&printer, this);
    if(pDialog.exec() == QDialog::Rejected){
        QMessageBox::warning(this, "Warning", "Cannot Access Printer");
        return;
    }

    // 2. Cetak isi teks dari tab yang aktif
    activeEditor->print(&printer);
}

void Notepad::on_actionCopy_triggered()
{
    QTextEdit *activeEditor = getActiveEditor();
    if (activeEditor) {
        activeEditor->copy(); // Hanya meng-copy teks dari tab yang aktif
    }
}
#include <QTimer> // Pastikan ini sudah ada di bagian paling atas notepad.cpp

void Notepad::on_actionPaste_triggered()
{
    // 🔥 TRIK MAUT: Jalankan fungsi paste yang sesungguhnya setelah jeda 100 milidetik
    // Ini memberi waktu bagi Windows + V untuk selesai menyalin data ke RAM Clipboard!
    QTimer::singleShot(100, this, &Notepad::eksekusiPasteGambarSakti);
}

// BUAT FUNGSI BARU INI DI BAWAHNYA (Jangan lupa daftarkan "void eksekusiPasteGambarSakti();" di notepad.h bagian private slots)
void Notepad::eksekusiPasteGambarSakti()
{
    QTextEdit *activeEditor = getActiveEditor();
    if (!activeEditor) return;

    // Sekarang RAM Clipboard dijamin sudah terisi penuh oleh Windows + V
    QImage gambarClipboard = QApplication::clipboard()->image();

    if (!gambarClipboard.isNull()) {
        QImage gambarStandar = gambarClipboard.convertToFormat(QImage::Format_RGB32);
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);

        if (gambarStandar.save(&buffer, "JPG")) {
            QString base64Data = ba.toBase64();
            QString tagHtmlGambar = QString("<img src='data:image/jpeg;base64,%1' />").arg(base64Data);

            activeEditor->textCursor().insertHtml(tagHtmlGambar);
            activeEditor->document()->setModified(true);
            updateTabTitle();
            return;
        }
    }

    // Jika setelah ditunggu ternyata isinya bukan gambar melainkan teks biasa
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if (mimeData->hasText()) {
        activeEditor->paste();
    }
}

void Notepad::on_actionCut_triggered()
{
    QTextEdit *activeEditor = getActiveEditor();
    if (activeEditor) {
        activeEditor->cut(); // Hanya memotong teks pada tab yang aktif
    }
}

void Notepad::on_actionUndo_triggered()
{
    QTextEdit *activeEditor = getActiveEditor();
    if (activeEditor) {
        activeEditor->undo(); // Hanya membatalkan ketikan (undo) di tab yang aktif
    }
}

void Notepad::on_actionRedo_triggered()
{
    QTextEdit *activeEditor = getActiveEditor();
    if (activeEditor) {
        activeEditor->redo(); // Hanya mengulang ketikan (redo) di tab yang aktif
    }
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










