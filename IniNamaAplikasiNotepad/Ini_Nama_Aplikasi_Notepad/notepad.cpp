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

#include <QShortcut>
#include <QKeySequence>


// Tambahan include baru di paling atas file:
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <QDesktopServices>


// if you see smtg like label->show(), do not be scared of it cos it just means tht the object label is a pointer, most of the time here we use pointer ask gpt why

// a bit of theory, ram itu ada stack (tumpukan, pake last in first out sgt cepat, tpi kecil dan terbatas dipake buat sesuatu ke int x=5; begitu ketemu } ia bakal langsung dihapus memori) dan heap(tumpuka besar/acak, memori yg lbh luas dan bebas, ia berantakan dan g berurut, lbh lambat dri stack diapke untuk sesuatu yg ada new-nya kek, new QLabel(), ia g bakal hilang kecuali kt panggil delete)


Notepad::Notepad(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);


    // DI DALAM CONSTRUCTOR Notepad::Notepad
    #ifdef HAS_MULTIMEDIA
        mediaPlayer = new QMediaPlayer(this);
        audioOutput = new QAudioOutput(this);
        mediaPlayer->setAudioOutput(audioOutput);
    #endif

    // 1. PENGGANTIAN SHORTCUT PASTE (SOLUSI UTAMA)
    // Jangan gunakan ui->actionPaste->setShortcut jika ingin membajak Ctrl+V dari QTextEdit
    QShortcut *pasteShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this);
    pasteShortcut->setContext(Qt::WindowShortcut); // Deteksi di seluruh area jendela aplikasi
    connect(pasteShortcut, &QShortcut::activated, this, &Notepad::on_actionPaste_triggered);

    // Pintasan lainnya tetap aman menggunakan QAction atau QShortcut manual
    ui->actionSave_as->setShortcut(QKeySequence::Save);
    ui->actionNew->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));

    // 2. Pintasan Manual Ctrl + W untuk Menutup Tab Aktif
    QShortcut *closeTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), this);
    connect(closeTabShortcut, &QShortcut::activated, this, &Notepad::on_shortcutCloseTab_triggered);

    // 3. Inisialisasi Tab Pertama Saat Aplikasi Dibuka
    on_actionNew_triggered();
}

void Notepad::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index);
    // Setiap kali tab berubah (Ctrl+Tab / Ctrl+T / Klik), paksa kursor masuk ke teks editor
    QTextEdit *activeEditor = getActiveEditor();
    if (activeEditor) {
        activeEditor->setFocus(Qt::OtherFocusReason);
    }
}

QTextEdit* Notepad::getActiveEditor()
{
    // 1. Ambil halaman tab yang sedang aktif di layar saat ini
    QWidget *currentWidget = ui->tabWidget->currentWidget();
    if (!currentWidget) return nullptr;

    // 2. Cek apakah halaman aktif itu SENDIRI adalah sebuah QTextEdit
    QTextEdit *editor = qobject_cast<QTextEdit*>(currentWidget);

    // 3. Jika gagal, cari ke dalam anak-anaknya menggunakan findChild (Metode Sapu Jagat)
    if (!editor) {
        editor = currentWidget->findChild<QTextEdit*>();
    }

    return editor;
}

// Fungsi penjembatan khusus pintasan keyboard Ctrl + W
void Notepad::on_shortcutCloseTab_triggered()
{
    // 1. Cari tahu indeks tab mana yang saat ini sedang ditonton oleh user
    int currentIndex = ui->tabWidget->currentIndex();

    // 2. Jika ada tab yang terbuka (indeks valid/bukan -1), oper indeksnya ke fungsi penghapus milikmu
    if (currentIndex != -1) {
        on_tabWidget_tabCloseRequested(currentIndex);
    }
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

    ui->tabWidget->setCurrentIndex(tabIndex);
    newEditor->setFocus();
    newEditor->setProperty("filePath", QString(""));

    connect(newEditor, &QTextEdit::textChanged, this, &Notepad::updateTabTitle);

    // =========================================================================
    // CARA GAMPANG: Paksa editor baru ini untuk patuh pada sistem shortcut kita
    // =========================================================================
    newEditor->installEventFilter(this);
}
// Pastikan #include <QDir> sudah ada di bagian paling atas notepad.cpp bersama include lainnya


bool Notepad::eventFilter(QObject *obj, QEvent *event)
{
    // Jika event yang terjadi adalah tombol keyboard ditekan di dalam QTextEdit
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // Cek apakah yang ditekan adalah Ctrl + V
        if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_V) {
            // Jalankan fungsi paste gambar sakti milikmu secara paksa!
            on_actionPaste_triggered();
            return true; // "true" artinya kita bajak event-nya, QTextEdit bawaan gak bakal dapet
        }
    }

    // Kembalikan ke fungsi normal untuk tombol-tombol lainnya (A, B, C, Enter, dll)
    return QMainWindow::eventFilter(obj, event);
}


void Notepad::on_actionSave_as_triggered()
{
    // Ambil objek teks editor yang saat ini sedang aktif di depan layar
    QTextEdit *activeEditor = getActiveEditor();
    if (!activeEditor) return; // Antisipasi jika tidak ada tab yang terbuka

    // TENTUKAN PATH LOKAL & BUAT FOLDER JIKA BELUM ADA
    QString defaultPath = "C:/Ini_Nama_Aplikasi_Notepad/Files";
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

        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
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

    newEditor->setFocus(); // <─── Memaksa kursor langsung aktif di tab yang baru dibuka tanpa ritual Ctrl+T & Ctrl+W!
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

    // Ambil data MIME dari Clipboard untuk memeriksa tipe data secara akurat
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if (!mimeData) return;

    // 1. JIKA CLIPBOARD BERISI GAMBAR
    if (mimeData->hasImage()) {
        QImage gambarClipboard = QApplication::clipboard()->image();

        if (!gambarClipboard.isNull()) {
            QImage gambarStandar = gambarClipboard.convertToFormat(QImage::Format_RGB32);
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);

            if (gambarStandar.save(&buffer, "JPG")) {
                QString base64Data = ba.toBase64();
                QString tagHtmlGambar = QString("<img src='data:image/jpeg;base64,%1' />").arg(base64Data);

                // Ambil kursor asli editor aktif
                QTextCursor kursorAktif = activeEditor->textCursor();

                // Bungkus transaksi manipulasi teks agar dokumen tidak terkunci internal
                kursorAktif.beginEditBlock();
                kursorAktif.insertHtml(tagHtmlGambar);
                kursorAktif.endEditBlock();

                // Paksa kursor asli editor di layar untuk memperbarui posisinya maju melewati gambar
                activeEditor->setTextCursor(kursorAktif);

                activeEditor->document()->setModified(true);
                updateTabTitle();

                // ─── SOLUSI REBUT FOKUS (PENGHANCUR RITUAL) ───
                activeEditor->activateWindow();            // Pastikan window aplikasi menangkap fokus utama
                activeEditor->setFocus(Qt::OtherFocusReason); // Kembalikan kursor ketikan aktif ke dalam teks editor

                // Paksa Qt memproses semua antrean render grafis layout segera
                QCoreApplication::processEvents();
                return;
            }
        }
    }

    // 2. JIKA CLIPBOARD BERISI TEKS BIASA / HTML
    if (mimeData->hasText()) {
        activeEditor->insertPlainText(mimeData->text());
        activeEditor->document()->setModified(true);
        updateTabTitle();

        // Pastikan teks biasa juga mengembalikan kursor secara otomatis
        activeEditor->setFocus(Qt::OtherFocusReason);
    }
}

void Notepad::on_actionInsert_Image_triggered()
{
    // 1. Ambil objek teks editor yang saat ini sedang aktif di depan layar
    QTextEdit *activeEditor = getActiveEditor();
    if (!activeEditor) return; // Antisipasi jika tidak ada tab yang terbuka

    // 2. Buka dialog Windows/Linux Explorer untuk memilih file gambar
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Pilih Gambar untuk Dimasukkan"), "",
                                                    tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));

    // 3. Jika user tidak menekan cancel (file path tidak kosong)
    if (!filePath.isEmpty()) {
        // Ambil posisi kursor ketikan user saat ini di tab aktif
        QTextCursor cursor = activeEditor->textCursor();

        // Buat format objek gambar dan masukkan path lokal gambar tersebut
        QTextImageFormat imageFormat;
        imageFormat.setName(filePath);

        // Opsional: Kamu bisa batasi ukuran lebar gambar agar tidak merusak layout (misal: lebar 400px)
        // imageFormat.setWidth(400);

        // 4. Masukkan gambar tepat di posisi kursor berada
        cursor.insertImage(imageFormat);

        // Pemicu agar tab memunculkan tanda bintang (*) karena dokumen telah dimodifikasi
        activeEditor->document()->setModified(true);
        updateTabTitle();
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
    // 1. Perbarui Ikon Utama Jendela / Window Frame
    QString windowIconPath = ":/IMAGE/Icons/main_logo";
    windowIconPath += (isDark ? "_B.png" : "_W.png");
    this->setWindowIcon(QIcon(windowIconPath));

    // 2. Ambil semua komponen QAction (Tombol Menu/Toolbar)
    const QList<QAction*> semuaTombol = this->findChildren<QAction*>();

    for (QAction* const tombol : semuaTombol) {
        QString namaObjek = tombol->objectName();

        // Lewati tombol bawaan internal sistem Qt
        if (namaObjek.isEmpty() || namaObjek.startsWith("qt_")) {
            continue;
        }

        QString pathAsli = tombol->property("originalIconPath").toString();

        // Jika pertama kali dijalankan, cari dan kunci path dasar filenya
        if (pathAsli.isEmpty()) {
            QString namaBersih = namaObjek;

            // Bersihkan prefix "action" dan "_" agar sinkron dengan nama file
            if (namaBersih.startsWith("action", Qt::CaseInsensitive)) {
                namaBersih.remove(0, 6);
            }
            if (namaBersih.startsWith("_")) {
                namaBersih.remove(0, 1);
            }

            // Kumpulkan variasi kemungkinan nama file dan folder resource
            QStringList kemungkinanPath;
            kemungkinanPath << (":/IMAGE/Icons/" + namaBersih);
            kemungkinanPath << (":/IMAGE/Icons/" + namaBersih.toLower());
            kemungkinanPath << (":/image/icons/" + namaBersih);
            kemungkinanPath << (":/image/icons/" + namaBersih.toLower());

            // Cari file mana yang benar-benar eksis di resource sistem (.qrc)
            for (const QString& pathCek : std::as_const(kemungkinanPath)) {
                if (QFile::exists(pathCek + "_B.png") || QFile::exists(pathCek + "_W.png") ||
                    QFile::exists(pathCek + "_b.png") || QFile::exists(pathCek + "_w.png")) {
                    pathAsli = pathCek;
                    break;
                }
            }

            // Jika ketemu, simpan path dasar tersebut ke dalam property objek tombol
            if (!pathAsli.isEmpty()) {
                tombol->setProperty("originalIconPath", pathAsli);
            }
        }

        // 3. Eksekusi penukaran ikon berdasarkan status tema aktif
        if (!pathAsli.isEmpty()) {
            QString pathFinal = "";

            if (isDark) {
                if (QFile::exists(pathAsli + "_B.png"))       pathFinal = pathAsli + "_B.png";
                else if (QFile::exists(pathAsli + "_b.png"))  pathFinal = pathAsli + "_b.png";
            }
            else {
                if (QFile::exists(pathAsli + "_W.png"))       pathFinal = pathAsli + "_W.png";
                else if (QFile::exists(pathAsli + "_w.png"))  pathFinal = pathAsli + "_w.png";
            }

            // Terapkan ikon baru secara paksa ke layar aplikasi
            if (!pathFinal.isEmpty()) {
                tombol->setIcon(QIcon(pathFinal));
            }
        }
    }
}










void Notepad::on_actionPlaylist_1_triggered()
{
#ifdef HAS_MULTIMEDIA
    // 1. Tentukan path default untuk folder musik & buat direktori jika belum ada
    QString defaultPath = "C:/Ini_Nama_Aplikasi_Notepad/Musics";
    QDir dir(defaultPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 2. Buka File Dialog dengan mengunci pandangan awal ke defaultPath
    QString fileAudio = QFileDialog::getOpenFileName(this,
                                                     tr("Pilih File Musik/Audio"),
                                                     defaultPath, // <── Direkam agar langsung membuka folder Musics
                                                     tr("Audio Files (*.mp3 *.wav *.ogg *.m4a)"));

    // 3. Jika user memilih file (tidak menekan tombol cancel)
    if (!fileAudio.isEmpty()) {

        // Atur sumber lagu ke media player menggunakan QUrl lokal
        mediaPlayer->setSource(QUrl::fromLocalFile(fileAudio));

        // 🔥 TAMBAHAN BARU: Set player agar mengulang musik tanpa batas (Looping)
        mediaPlayer->setLoops(QMediaPlayer::Infinite);

        // Atur volume suara default (rentang nilai dari 0.0 sampai 1.0)
        audioOutput->setVolume(0.5); // Volume 50%

        // 4. Putar Musik Sakti!
        mediaPlayer->play();

        // Beri informasi di status bar agar user tahu musik sedang berputar
        if (this->statusBar()) {
            this->statusBar()->showMessage("Currently playing: " + QFileInfo(fileAudio).fileName());
        }
    }
#else
    QMessageBox::information(this, "Fitur Nonaktif", "Modul Multimedia tidak diaktifkan di build ini.");
#endif
}

void Notepad::on_actionStop_Music_triggered()
{
#ifdef HAS_MULTIMEDIA
    if (mediaPlayer) {
        mediaPlayer->stop();

        // Opsional: Bersihkan tulisan di status bar saat musik dihentikan
        if (this->statusBar()) {
            this->statusBar()->showMessage("Musik dihentikan", 3000); // Teks hilang setelah 3 detik
        }
    }
#else
    QMessageBox::information(this, "Fitur Nonaktif", "Modul Multimedia tidak diaktifkan di build ini.");
#endif
}
