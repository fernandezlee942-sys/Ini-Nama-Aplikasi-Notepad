/********************************************************************************
** Form generated from reading UI file 'notepad.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTEPAD_H
#define UI_NOTEPAD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Notepad
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave_as;
    QAction *actionSave;
    QAction *actionClose_Tab;
    QAction *actionExit;
    QAction *actionUndo;
    QAction *actionFind;
    QAction *actionFind_Next;
    QAction *actionFind_Previous;
    QAction *actionReplace;
    QAction *actionStatus_Bar;
    QAction *actionWord_Wrap;
    QAction *actionRedo;
    QAction *actionPlaylist_1;
    QAction *actionNew_Playlist;
    QAction *actionPrint;
    QAction *actionPaste;
    QAction *actionCut;
    QAction *actionCopy;
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuMusic;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *Notepad)
    {
        if (Notepad->objectName().isEmpty())
            Notepad->setObjectName("Notepad");
        Notepad->resize(400, 300);
        Notepad->setMinimumSize(QSize(400, 300));
        Notepad->setMaximumSize(QSize(1280, 720));
        actionNew = new QAction(Notepad);
        actionNew->setObjectName("actionNew");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/IMAGE/Icons/New.bmp"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionNew->setIcon(icon);
        actionOpen = new QAction(Notepad);
        actionOpen->setObjectName("actionOpen");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/IMAGE/Icons/Open.bmp"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionOpen->setIcon(icon1);
        actionSave_as = new QAction(Notepad);
        actionSave_as->setObjectName("actionSave_as");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/IMAGE/Icons/Save.bmp"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSave_as->setIcon(icon2);
        actionSave = new QAction(Notepad);
        actionSave->setObjectName("actionSave");
        actionClose_Tab = new QAction(Notepad);
        actionClose_Tab->setObjectName("actionClose_Tab");
        actionExit = new QAction(Notepad);
        actionExit->setObjectName("actionExit");
        actionUndo = new QAction(Notepad);
        actionUndo->setObjectName("actionUndo");
        actionFind = new QAction(Notepad);
        actionFind->setObjectName("actionFind");
        actionFind_Next = new QAction(Notepad);
        actionFind_Next->setObjectName("actionFind_Next");
        actionFind_Previous = new QAction(Notepad);
        actionFind_Previous->setObjectName("actionFind_Previous");
        actionReplace = new QAction(Notepad);
        actionReplace->setObjectName("actionReplace");
        actionStatus_Bar = new QAction(Notepad);
        actionStatus_Bar->setObjectName("actionStatus_Bar");
        actionWord_Wrap = new QAction(Notepad);
        actionWord_Wrap->setObjectName("actionWord_Wrap");
        actionRedo = new QAction(Notepad);
        actionRedo->setObjectName("actionRedo");
        actionPlaylist_1 = new QAction(Notepad);
        actionPlaylist_1->setObjectName("actionPlaylist_1");
        actionNew_Playlist = new QAction(Notepad);
        actionNew_Playlist->setObjectName("actionNew_Playlist");
        actionPrint = new QAction(Notepad);
        actionPrint->setObjectName("actionPrint");
        actionPaste = new QAction(Notepad);
        actionPaste->setObjectName("actionPaste");
        actionCut = new QAction(Notepad);
        actionCut->setObjectName("actionCut");
        actionCopy = new QAction(Notepad);
        actionCopy->setObjectName("actionCopy");
        centralwidget = new QWidget(Notepad);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(9, 9, 341, 241));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(verticalLayoutWidget);
        textEdit->setObjectName("textEdit");

        verticalLayout->addWidget(textEdit);

        Notepad->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Notepad);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 400, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuMusic = new QMenu(menubar);
        menuMusic->setObjectName("menuMusic");
        Notepad->setMenuBar(menubar);
        statusbar = new QStatusBar(Notepad);
        statusbar->setObjectName("statusbar");
        Notepad->setStatusBar(statusbar);
        toolBar = new QToolBar(Notepad);
        toolBar->setObjectName("toolBar");
        Notepad->addToolBar(Qt::ToolBarArea::RightToolBarArea, toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuMusic->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_as);
        menuFile->addAction(actionPrint);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionClose_Tab);
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuEdit->addAction(actionFind);
        menuEdit->addAction(actionFind_Next);
        menuEdit->addAction(actionFind_Previous);
        menuEdit->addAction(actionReplace);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        menuEdit->addAction(actionCut);
        menuView->addAction(actionStatus_Bar);
        menuView->addAction(actionWord_Wrap);
        menuMusic->addAction(actionPlaylist_1);
        menuMusic->addAction(actionNew_Playlist);
        toolBar->addAction(actionNew);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionSave_as);

        retranslateUi(Notepad);

        QMetaObject::connectSlotsByName(Notepad);
    } // setupUi

    void retranslateUi(QMainWindow *Notepad)
    {
        Notepad->setWindowTitle(QCoreApplication::translate("Notepad", "Notepad", nullptr));
        actionNew->setText(QCoreApplication::translate("Notepad", " New Tab", nullptr));
        actionOpen->setText(QCoreApplication::translate("Notepad", "Open", nullptr));
        actionSave_as->setText(QCoreApplication::translate("Notepad", "Save As", nullptr));
        actionSave->setText(QCoreApplication::translate("Notepad", "Save", nullptr));
        actionClose_Tab->setText(QCoreApplication::translate("Notepad", "Close Tab", nullptr));
        actionExit->setText(QCoreApplication::translate("Notepad", "Exit", nullptr));
        actionUndo->setText(QCoreApplication::translate("Notepad", "Undo", nullptr));
        actionFind->setText(QCoreApplication::translate("Notepad", "Find", nullptr));
        actionFind_Next->setText(QCoreApplication::translate("Notepad", "Find Next", nullptr));
        actionFind_Previous->setText(QCoreApplication::translate("Notepad", "Find Previous", nullptr));
        actionReplace->setText(QCoreApplication::translate("Notepad", "Replace", nullptr));
        actionStatus_Bar->setText(QCoreApplication::translate("Notepad", "Status Bar", nullptr));
        actionWord_Wrap->setText(QCoreApplication::translate("Notepad", "Word Wrap", nullptr));
        actionRedo->setText(QCoreApplication::translate("Notepad", "Redo", nullptr));
        actionPlaylist_1->setText(QCoreApplication::translate("Notepad", "Open Playlist", nullptr));
        actionNew_Playlist->setText(QCoreApplication::translate("Notepad", "New Playlist", nullptr));
        actionPrint->setText(QCoreApplication::translate("Notepad", "Print", nullptr));
        actionPaste->setText(QCoreApplication::translate("Notepad", "Paste", nullptr));
        actionCut->setText(QCoreApplication::translate("Notepad", "Cut", nullptr));
        actionCopy->setText(QCoreApplication::translate("Notepad", "Copy", nullptr));
        menuFile->setTitle(QCoreApplication::translate("Notepad", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("Notepad", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("Notepad", "View", nullptr));
        menuMusic->setTitle(QCoreApplication::translate("Notepad", "Music", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("Notepad", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Notepad: public Ui_Notepad {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTEPAD_H
