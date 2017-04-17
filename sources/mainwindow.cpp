
#include "mainwindow.h"

using namespace std; 

QTextStream out(stdout);

Mainwindow::Mainwindow(QWidget *parent) : QMainWindow(parent){
    
    /// sconfig file
    QString tmp=":resources/configuration.cfg";
    
    if(QFile::exists("configuration.cfg"))
        tmp="configuration.cfg";
    if(QFile::exists("resources/configuration.cfg"))
        tmp="resources/configuration.cfg";
        
    out<<"Config file: "<<tmp<<endl;
    settings=new QSettings(QString(tmp),QSettings::IniFormat);
             
    /// title & icon
    this->setWindowTitle(set("interface/windowTitle"));
    this->setWindowIcon(QIcon(set("interface/windowIcon")));
    
    /// labels
    mainLabel = new QLabel(set("interface/mainLabel"));
    mainLabel->setWordWrap(1);
    dirLabel = new QLabel(set("interface/dirLabel"));
    dirLabel->setWordWrap(1);
    dirLabel->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Maximum);
    
    /// buttons
    selectBtn = new QPushButton(set("interface/selectBtn"));
    connect(selectBtn, SIGNAL(clicked()),this,SLOT(selectDir()));
    selectBtn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    
    installBtn = new QPushButton(set("interface/installBtn"));
    connect(installBtn, SIGNAL(clicked()), this, SLOT(startInstall()));
    installBtn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    installBtn->setEnabled(0);
       
    uninstallBtn = new QPushButton(set("interface/uninstallBtn"));
    connect(uninstallBtn, SIGNAL(clicked()), this, SLOT(startUninstall()));
    uninstallBtn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    uninstallBtn->setEnabled(0);
    
    /// layouts
    QWidget *inside = new QWidget(this);
    this->setCentralWidget(inside);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(inside);
    
    QHBoxLayout *directLay = new QHBoxLayout();
    directLay->addWidget(selectBtn);
    directLay->addWidget(installBtn);
    directLay->addWidget(uninstallBtn);
    
    mainLayout->addWidget(mainLabel);
    mainLayout->addLayout(directLay);
    mainLayout->addWidget(dirLabel);
    
    inside->setLayout(mainLayout);
    
}

//*****************************************************************************//
//                                 SLOTS                                       //
//*****************************************************************************//

void Mainwindow::startInstall()  {installer(1);}
void Mainwindow::startUninstall(){installer(0);}

/// file dialog
void Mainwindow::selectDir(){
    dirLabel->setText(QFileDialog::getExistingDirectory(this,
            set("interface/openFolder"),set("interface/dirLabel"),
            QFileDialog::ShowDirsOnly));
            
    if(!QFile::exists(dirLabel->text()+set("interface/checkDir"))){
        installBtn->setEnabled(0);
        uninstallBtn->setEnabled(0);
        QMessageBox aboutBox(this);
        aboutBox.setText(set("interface/invalidPath"));
        aboutBox.setWindowTitle(set("interface/error"));
        aboutBox.exec();
    }
    else{
        installBtn->setEnabled(1);
        uninstallBtn->setEnabled(1);
    }
}

///key presses
void Mainwindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_C) 
        QFile::copy(":resources/configuration.cfg","configuration.cfg");
}

//*****************************************************************************//
//                               FUNCTIONS                                     //
//*****************************************************************************//

/// (un)install...
void Mainwindow::installer(bool mode){
    setCursor(Qt::WaitCursor);
    
    QString path=dirLabel->text(),error;
    
    QStringList ls;
    QDir d("resources");
    recurseAddDir(d,ls);
    
    int n=ls.count();
    
    for(int i=0; i<n; i++){
        
        QString in=ls[i],backup,type=getType(in.right(in.size()-in.lastIndexOf(".")),backup);
        if(type.isEmpty()) continue;
        QString outf=path + type + in.right(in.size()-in.lastIndexOf("/"));
        
        QDir::toNativeSeparators(outf);
        QDir::toNativeSeparators(in);
        
        /// restore on uninstall
        if(backup=="1" && !mode){
            QFile::remove(outf);
            QFile::rename(outf+set("interface/backupSuffix"),outf);
            }
        /// back up on install
        if(backup=="1" && mode){
            QFile::rename(outf,outf+set("interface/backupSuffix"));
            QFile::remove(outf);
            }
        /// remove files
        if(backup=="0")
            QFile::remove(outf);
            
        /// copy source file
        if(mode && !QFile::copy(in,outf))
            error+=in+" -> "+outf+set("interface/copyFail");
        
    }
    
    if(!error.isEmpty())
        error=set("interface/fileError1") + error + set("interface/fileError2");
    if(error.isEmpty() && mode)
        error=set("interface/installed");
    if(!mode)
        error=set("interface/uninstalled");
        
    setCursor(Qt::ArrowCursor);
    
    QMessageBox aboutBox(this);
    aboutBox.setText(error);
    aboutBox.setWindowTitle("Log");
    aboutBox.exec();
}

/// lazy... :^)
QString Mainwindow::set(QString category){
    return settings->value(category).toString();
}

/// returns file type directory
QString Mainwindow::getType(QString ext,QString& backup){
    int n=set("types/typeAmount").toInt();
    for(int i=0; i<n; i++){
        QStringList list=settings->value("types/type"+QString::number(i)).toStringList();
        for(int j=2;j<list.count();j++)
            if(ext==list[j]){
                backup=list[1];
                return list[0];
            }
    }
    return "";
}

/// recursive file search
void Mainwindow::recurseAddDir(QDir d,QStringList& list){
    QStringList qsl = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    foreach (QString file, qsl) {
        QFileInfo finfo(QString("%1/%2").arg(d.path()).arg(file));
        if (finfo.isSymLink())
            return;
        if (finfo.isDir()) {
            QString dirname = finfo.fileName();
            QDir sd(finfo.filePath());
            recurseAddDir(sd, list);
        } else
            list << QDir::toNativeSeparators(finfo.filePath());
    }
}