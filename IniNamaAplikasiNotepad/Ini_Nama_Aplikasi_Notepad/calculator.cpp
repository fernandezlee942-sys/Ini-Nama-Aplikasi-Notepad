#include "calculator.h"
#include "ui_calculator.h"
#include <QRegularExpression>
#include <stack>
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

    connect(ui->e,SIGNAL(released()),
            this,SLOT(EPressed()));

    connect(ui->kurungawal,SIGNAL(released()),
            this,SLOT(kurungawalPressed()));

    connect(ui->kurungakhir,SIGNAL(released()),
            this,SLOT(kurungakhirPressed()));

    connect(ui->koma,SIGNAL(released()),
            this,SLOT(DecimalPressed()));
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

void Calculator::MathButtonPressed()
{
    QPushButton *button=(QPushButton*)sender();

    QString op=button->text();

    QString displayVal=ui->display->text();

    ui->display->setText(displayVal+op);
}

void Calculator::EqualButton()
{
    QString expr=ui->display->text();

    expr.replace("x","*");

    if(expr=="e")
        expr="2.718281828";

    expr.replace("+e","+2.718281828");
    expr.replace("-e","-2.718281828");
    expr.replace("*e","*2.718281828");
    expr.replace("/e","/2.718281828");
    expr.replace("(e","(2.718281828");

    double result=evaluate(expr);

    ui->display->setText(QString::number(result));
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
    QString displayVal=ui->display->text();

    if(displayVal=="0")
        ui->display->setText("sin(");
    else
        ui->display->setText(displayVal+"sin(");
}

void Calculator::CosPressed()
{
    QString displayVal=ui->display->text();

    if(displayVal=="0")
        ui->display->setText("cos(");
    else
        ui->display->setText(displayVal+"cos(");
}

void Calculator::TanPressed()
{
    QString displayVal=ui->display->text();

    if(displayVal=="0")
        ui->display->setText("tan(");
    else
        ui->display->setText(displayVal+"tan(");
}

void Calculator::EPressed()
{
    QString displayVal = ui->display->text();

    if(displayVal=="0")
    {
        ui->display->setText("e");
    }
    else
    {
        ui->display->setText(displayVal+"e");
    }
}

double Calculator::evaluate(QString expr)
{
    std::stack<double> values;
    std::stack<QChar> ops;

    auto applyOp = [&](double a,double b,QChar op)
    {
        if(op=='+') return a+b;
        if(op=='-') return a-b;
        if(op=='*') return a*b;
        return a/b;
    };

    auto precedence=[&](QChar op)
    {
        if(op=='+'||op=='-') return 1;
        if(op=='*'||op=='/') return 2;
        return 0;
    };

    int i=0;

    while(i<expr.length())
    {
        if(expr[i]==' ')
        {
            i++;
            continue;
        }

        if(expr[i]=='(')
        {
            ops.push('(');
            i++;
        }

        else if(
            expr[i].isDigit() ||
            expr[i]=='.' ||
            (expr[i]=='-' &&
             (i==0 || expr[i-1]=='(' ||
              expr[i-1]=='+' ||
              expr[i-1]=='-' ||
              expr[i-1]=='*' ||
              expr[i-1]=='/'))
            )
        {
            QString num;

            while(i<expr.length() &&
                   (expr[i].isDigit() ||
                    expr[i]=='.' ||
                    expr[i]=='e' ||
                    expr[i]=='E' ||
                    expr[i]=='-'))
            {
                num+=expr[i];
                i++;
            }

            values.push(num.toDouble());
        }

        else if(expr.mid(i,3)=="sin")
        {
            i+=4;

            QString inside;

            int depth=1;

            while(i<expr.length() && depth>0)
            {
                if(expr[i]=='(') depth++;

                else if(expr[i]==')')
                {
                    depth--;

                    if(depth==0)
                        break;
                }

                inside+=expr[i];

                i++;
            }

            values.push(
                sin(evaluate(inside))
                );

            i++;
        }

        else if(expr.mid(i,3)=="cos")
        {
            i+=4;

            QString inside;

            int depth=1;

            while(i<expr.length() && depth>0)
            {
                if(expr[i]=='(') depth++;

                else if(expr[i]==')')
                {
                    depth--;

                    if(depth==0)
                        break;
                }

                inside+=expr[i];

                i++;
            }

            values.push(
                cos(evaluate(inside))
                );

            i++;
        }

        else if(expr.mid(i,3)=="tan")
        {
            i+=4;

            QString inside;

            int depth=1;

            while(i<expr.length() && depth>0)
            {
                if(expr[i]=='(') depth++;

                else if(expr[i]==')')
                {
                    depth--;

                    if(depth==0)
                        break;
                }

                inside+=expr[i];

                i++;
            }

            values.push(
                tan(evaluate(inside))
                );

            i++;
        }

        else if(expr[i]==')')
        {
            while(!ops.empty() && ops.top()!='(')
            {
                double b=values.top();
                values.pop();

                double a=values.top();
                values.pop();

                QChar op=ops.top();
                ops.pop();

                values.push(applyOp(a,b,op));
            }

            ops.pop();

            i++;
        }

        else
        {
            while(!ops.empty() &&
                   precedence(ops.top())>=precedence(expr[i]))
            {
                double b=values.top();
                values.pop();

                double a=values.top();
                values.pop();

                QChar op=ops.top();
                ops.pop();

                values.push(applyOp(a,b,op));
            }

            ops.push(expr[i]);

            i++;
        }
    }

    while(!ops.empty())
    {
        double b=values.top();
        values.pop();

        double a=values.top();
        values.pop();

        QChar op=ops.top();
        ops.pop();

        values.push(applyOp(a,b,op));
    }

    return values.top();
}

void Calculator::kurungawalPressed()
{
    QString displayVal=ui->display->text();

    if(displayVal=="0")
        ui->display->setText("(");
    else
        ui->display->setText(displayVal+"(");
}

void Calculator::kurungakhirPressed()
{
    ui->display->setText(
        ui->display->text()+")"
        );
}

void Calculator::DecimalPressed()
{
    QString displayVal=ui->display->text();

    int lastOperator=
        std::max(
            displayVal.lastIndexOf('+'),
            std::max(
                displayVal.lastIndexOf('-'),
                std::max(
                    displayVal.lastIndexOf('*'),
                    displayVal.lastIndexOf('/')
                    )
                )
            );

    QString currentNumber=
        displayVal.mid(lastOperator+1);

    if(!currentNumber.contains('.'))
    {
        ui->display->setText(displayVal+".");
    }
}

