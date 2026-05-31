#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

namespace Ui {
class Calculator;
}

class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator();

private:
    Ui::Calculator *ui;
    double evaluate(QString expr);

private slots:
    void NumPressed();
    void MathButtonPressed();
    void EqualButton();
    void ChangeNumber();
    void on_del_clicked();
    void on_c_clicked();
    void SinPressed();
    void CosPressed();
    void TanPressed();
    void EPressed();
    void kurungawalPressed();
    void kurungakhirPressed();
    void DecimalPressed();
};

#endif // CALCULATOR_H
