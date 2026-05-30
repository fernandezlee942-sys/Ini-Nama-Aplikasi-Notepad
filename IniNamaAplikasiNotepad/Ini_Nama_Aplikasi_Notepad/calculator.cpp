#include "calculator.h"
#include "ui_calculator.h"
#include <QRegularExpression>
#include <cmath>

double calcVal = 0.0;
bool bagiTrigger = false;
bool kaliTrigger = false;
bool tambahTrigger = false;
bool kurangTrigger = false;
bool sinTrigger = false;
bool cosTrigger = false;
bool tanTrigger = false;

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);

    ui->display->setText(QString::number(calcVal));
    QPushButton *numButtons[10];

    for(int i=0;i<10;++i){
        QString butName = "tombol" + QString::number(i);
        numButtons[i] = Calculator::findChild<QPushButton *>(butName);
        connect(numButtons[i], SIGNAL(released()), this,
                SLOT(NumPressed()));
}
    connect(ui->tambah, SIGNAL(released()), this,
            SLOT(MathButtonPressed()));
    connect(ui->kurang, SIGNAL(released()), this,
            SLOT(MathButtonPressed()));
    connect(ui->bagi, SIGNAL(released()), this,
            SLOT(MathButtonPressed()));
    connect(ui->kali, SIGNAL(released()), this,
            SLOT(MathButtonPressed()));

    connect(ui->samadengan, SIGNAL(released()), this,
            SLOT(EqualButton()));

    connect(ui->change, SIGNAL(released()), this,
            SLOT(ChangeNumber()));

    connect(ui->c, SIGNAL(released()), this,
            SLOT(on_c_clicked()));

    connect(ui->sin, SIGNAL(released()), this,
            SLOT(SinPressed()));

    connect(ui->cos, SIGNAL(released()), this,
            SLOT(CosPressed()));

    connect(ui->tan, SIGNAL(released()), this,
            SLOT(TanPressed()));

}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::NumPressed()
{
    QPushButton *button=(QPushButton*)sender();

    QString butval = button->text();

    QString displayVal = ui->display->text();

    if(displayVal=="0")
    {
        ui->display->setText(butval);
    }
    else
    {
        ui->display->setText(displayVal + butval);
    }
}

void Calculator::MathButtonPressed(){
    bagiTrigger = false;
    kaliTrigger = false;
    tambahTrigger = false;
    kurangTrigger = false;

    QString displayVal = ui->display->text();
    calcVal = displayVal.toDouble();
    QPushButton *button =(QPushButton *)sender();
    QString butval = button -> text();

    if (QString :: compare (butval, "/", Qt :: CaseInsensitive) == 0){
        bagiTrigger = true;
    }

    else if (QString :: compare (butval, "*", Qt :: CaseInsensitive) == 0){
        kaliTrigger = true;
    }

    else if (QString :: compare (butval, "+", Qt :: CaseInsensitive) == 0){
        tambahTrigger = true;
    }

    else {
        kurangTrigger = true;
    }

    ui->display->setText("");
}

void Calculator::EqualButton()
{
    double solution = 0.0;

    QString displayVal = ui->display->text();

    // ===== TRIGONOMETRI =====

    if (sinTrigger)
    {
        QString numStr = displayVal;
        numStr.remove("sin(");

        double value = numStr.toDouble();

        solution = sin(value);

        sinTrigger = false;
    }

    else if (cosTrigger)
    {
        QString numStr = displayVal;
        numStr.remove("cos(");

        double value = numStr.toDouble();

        solution = cos(value);

        cosTrigger = false;
    }

    else if (tanTrigger)
    {
        QString numStr = displayVal;
        numStr.remove("tan(");

        double value = numStr.toDouble();

        solution = tan(value);

        tanTrigger = false;
    }

    // ===== OPERATOR BIASA =====

    else
    {
        double dbldisplayval = displayVal.toDouble();

        if (tambahTrigger || kurangTrigger || kaliTrigger || bagiTrigger)
        {
            if (tambahTrigger)
            {
                solution = calcVal + dbldisplayval;
            }

            else if (kurangTrigger)
            {
                solution = calcVal - dbldisplayval;
            }

            else if (kaliTrigger)
            {
                solution = calcVal * dbldisplayval;
            }

            else
            {
                solution = calcVal / dbldisplayval;
            }
        }
    }

    ui->display->setText(QString::number(solution));
}

void Calculator::ChangeNumber(){
    QString displayVal = ui->display->text();
    QRegularExpression reg("[-]?[0-9.]*");

    if (reg.match(displayVal).hasMatch()){
        double dbldisplayval = displayVal.toDouble();
        double dbldisplayvalsign = -1 * dbldisplayval;
        ui->display->setText(QString::number(dbldisplayvalsign));
    }
}

void Calculator::on_del_clicked()
{
    QString displayVal = ui->display->text();

    if(displayVal.length() > 1)
    {
        displayVal.chop(1);
        ui->display->setText(displayVal);
    }
    else
    {
        ui->display->setText("0");
    }
}

void Calculator::on_c_clicked()
{
    ui->display->setText("0");
}

void Calculator::SinPressed()
{
    sinTrigger = true;
    cosTrigger = false;
    tanTrigger = false;

    ui->display->setText("sin(");
}

void Calculator::CosPressed()
{
    cosTrigger = true;
    sinTrigger = false;
    tanTrigger = false;

    ui->display->setText("cos(");
}

void Calculator::TanPressed()
{
    tanTrigger = true;
    sinTrigger = false;
    cosTrigger = false;

    ui->display->setText("tan(");
}