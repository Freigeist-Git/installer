#pragma once

#include <QtGui>
#include <QApplication>

using namespace std; 

class Mainwindow : public QMainWindow {
    
    Q_OBJECT  

    public:
    
        Mainwindow(QWidget *parent = 0);
        
    private slots:
    
        void selectDir();
        void startInstall();
        void startUninstall();
        
        void keyPressEvent(QKeyEvent * e);
    
    private:
        
        void recurseAddDir(QDir,QStringList&);
        QString getType(QString,QString&);
        void installer(bool mode);
        QString set(QString);
        
        QSettings *settings;
        QLabel *mainLabel,*dirLabel;
        QPushButton *installBtn,*uninstallBtn,*selectBtn;
        
};
