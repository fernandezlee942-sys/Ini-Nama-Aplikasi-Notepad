#include "calculator.h"
#include "ui_calculator.h"

double calcVal = 0.0;
bool bagiTrigger = false;
bool kaliTrigger = false;
bool tambahTrigger = false;
bool kurangTrigger = false;

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
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::NumPressed(){
    QPushButton *button = (QPushButton *)sender();
    QString butval = button->text();
    QString displayVal = ui->display->text();

    if((displayVal.toDouble() == 0) || displayVal.toDouble() == 0.0){
        ui->display->setText(butval);
    }
    else{
        QString newVal = displayVal + butval;
        double dblNewVal = newVal.toDouble();
        ui->display->setText(QString::number(dblNewVal, 'g', 16));
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

void Calculator::EqualButton(){
    double solution = 0.0;
    QString displayVal = ui->display->text();
    double dbldisplayval = displayVal.toDouble();

    if (tambahTrigger || kurangTrigger || kaliTrigger || bagiTrigger){
        if (tambahTrigger){
            solution = calcVal + dbldisplayval;
        }

        else if (kurangTrigger){
            solution = calcVal - dbldisplayval;
        }

        else if (kaliTrigger){
            solution = calcVal * dbldisplayval;
        }

        else{
            solution = calcVal / dbldisplayval;
        }
    }

    ui ->display->setText(QString::number(solution));
}
