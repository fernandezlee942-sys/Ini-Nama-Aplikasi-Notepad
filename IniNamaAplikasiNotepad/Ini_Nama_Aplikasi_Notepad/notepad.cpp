#include "notepad.h"
#include "./ui_notepad.h"
#include "timer.h"
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
    tmr->show();
}