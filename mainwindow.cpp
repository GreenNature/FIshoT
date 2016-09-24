#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include <QDebug>
#include <QToolBar>
#include <QPixmap>
#include <QMessageBox>
#include <QLocale>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pPalette	= new QPalette();
    m_pPixmap	= new QPixmap(":/Resource/Ex9MlTC.jpg");

    m_pPalette->setBrush(QPalette::Background,QBrush(*m_pPixmap));
    setPalette(*m_pPalette);

    createActions();
    createMenus();
    createToolBar();


    settingWifi();

    statusBar()->addWidget(progressbar);
    progressbar->setTextVisible(false);
    progressbar->setAlignment(Qt::AlignLeft);
    status_tip->setIndent(3);
    status_tip->setFixedWidth(200);
    statusBar()->addWidget(status_tip);

    connect(ui->ReadAP,SIGNAL(clicked(bool)),this,SLOT(read_comport_event()));
    connect(ui->upload_button, SIGNAL(clicked(bool)), this, SLOT(serial_send_data()));

    //tnwjd
    QListWidgetItem *wowsystem = new QListWidgetItem(QIcon(QString(":/Resource/wifi-40.ico")),QString("wowsystem"));
    QListWidgetItem *GreenNature = new QListWidgetItem(QIcon(QString(":/Resource/wifi-100.ico")),QString("GreenNature"));
    ui->AP_LIST->addItem(wowsystem);
    ui->AP_LIST->addItem(GreenNature);
    ui->AP_LIST->addAction(aboutAction);
    ui->AP_LIST->addItem(tr("item1"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent (QResizeEvent* event)
{
    event;
    m_pPalette->setBrush(QPalette::Background,QBrush(m_pPixmap->scaled(width(),height())));
    setPalette(*m_pPalette);
}

void MainWindow::createActions()
{
    infoAction = new QAction(tr("Informaion"),this);
    infoAction->setShortcut(QKeySequence::HelpContents);
    infoAction->setToolTip(tr("View a this program's Information"));

    settingAction = new QAction(tr("&Setting"),this);
    settingAction->setShortcut(Qt::Key_F9);
    settingAction->setToolTip(tr("You can program setting"));
    settingAction->setIcon(QIcon(QString(":/Resource/1473258333_gear.ico")));

    aboutAction = new QAction(tr("&About - FIshoT"),this);
    aboutAction->setIcon(QIcon(QString(":/Resource/Ico.ico")));

    connect(infoAction,SIGNAL(triggered(bool)),this, SLOT(information()));
    connect(aboutAction,SIGNAL(triggered(bool)),this, SLOT(about()));
    connect(settingAction,SIGNAL(triggered(bool)),this, SLOT(setting()));
}

void MainWindow::information()
{
    QDialog *about = new QDialog(this, 0);
    Ui_About aboutUi;
    aboutUi.setupUi(about);
    about->setModal(true);
    about->setWindowModality(Qt::WindowModal);
    about->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    about->hide();
    about->show();
}

void MainWindow::about()
{
    QMessageBox::about(this,"About - FIshoT",tr("Information<br>") + link);
}

void MainWindow::setting()
{
    serial->setting_open();
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Info"));
    fileMenu->addAction(infoAction);
    fileMenu->addAction(settingAction);
    fileMenu = menuBar()->addMenu(tr("&Cut"));
    fileMenu->addAction(settingAction);
    fileMenu->addSeparator();
    fileMenu->addAction(infoAction);

    settingMenu = menuBar()->addMenu(tr("&Setting"));
    settingMenu->addAction(settingAction);

    settingSubMenu = settingMenu->addMenu(tr("&SubSetting"));
    settingSubMenu->addAction(infoAction);
    settingSubMenu->addAction(settingAction);

    //add Help Menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(infoAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAction);
}

void MainWindow::createContextMenu()
{

}

void MainWindow::createStatusBar()
{
    statusBar()->setStyleSheet("QStatusBar {background: gray}");
}

void MainWindow::createToolBar()
{
    mainToolBar = addToolBar(tr("&Info"));
    mainToolBar->addAction(infoAction);
    mainToolBar->addAction(settingAction);
    mainToolBar->setStyleSheet("QToolButton:hover {background-color: darkgray; }");
}

void MainWindow::settingWifi()
{
    //연결된 AP를 표시하는 레이블 설정
    ui->ConnectedWifi->setText(QString::fromLocal8Bit("현재 연결된 AP가 없습니다."));
    //ui->ConnectedWifi->setToolTip(QString::fromLocal8Bit("현재 연결된 AP를 표시합니다."));
    
    //AP 읽는 버튼 설정
    //ui->ReadAP->setToolTip(QString::fromLocal8Bit("와이파이 목록을 읽습니다."));

    //AP password visible setting
    connect(ui->password_visible,SIGNAL(toggled(bool)),this,SLOT(password_visible_slot(bool)));
}

void MainWindow::update_AP_List(QStringList apList)
{
    AP_DATA;
    //ui->AP_LIST->addItem()
}

void MainWindow::password_visible_slot(bool checked)
{
    if(checked)
    {
        ui->ap_password_lineEdit->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->ap_password_lineEdit->setEchoMode(QLineEdit::Password);
    }
}

void MainWindow::read_comport_event()
{
    QStringList *list = new QStringList(serial->read_comport());
    for(int i=0; i<list->length(); i++)
    {
        qDebug() << list->at(i) << endl;
    }
    QStringListModel *APList_Model = new QStringListModel(*list);

    delete APList_Model;
}

void MainWindow::serial_send_data()
{
    current_work = CODE_IP; //tnwjd
    status_tip->setText(tr("send start data"));
    progressbar->setValue(10);
    data_parse();
}

void MainWindow::data_parse()
{
    char d[2] = {0x01,NULL};
    if(serial->able())
    {
        serial->write(d);
        data_response = true;
        status_tip->setText(tr("Wait response . . ."));
        progressbar->setValue(30);

        uploadTimer->start(10000);
        disconnect(serial,SIGNAL(data_available(QByteArray)), this, SLOT(serial_response(QByteArray)));
        connect(serial,SIGNAL(data_available(QByteArray)), this, SLOT(serial_response(QByteArray)));
        connect(uploadTimer,SIGNAL(timeout()),this, SLOT(uploadTimeOut()));
    }
    else
    {
        QMessageBox::warning(this,tr("Error"),tr("SerialPort : Device not open"));
        progressbar->setValue(0);
        status_tip->setText(tr("Please connect to device"));
    }
}

void MainWindow::serial_response(QByteArray d)
{
    if(!data_response_timer)
    {
        uploadTimer->stop();
        uploadTimer->start(5000);
        data_response_timer = true;
    }

    status_tip->setText(tr("Receive response"));
    progressbar->setValue(40);
    QString receive_code_scan_ap(QString("\r%1\r").arg(CODE_SCAN_AP));
    if(d.contains((char)0x01))
    {
        disconnect(serial,SIGNAL(data_available(QByteArray)), this, SLOT(serial_response(QByteArray)));
        uploadTimer->stop();
        disconnect(uploadTimer,SIGNAL(timeout()),this, SLOT(uploadTimeOut()));
        data_response_timer=false;

        status_tip->setText(tr("Now upload data. Don't disconnect to device"));
        progressbar->setValue(50);
        if(current_work == CODE_IP)
        {
            serial_data_list.ip_address.append(QString("%1\r%2\r\n").arg(CODE_IP).arg(ui->ip_address->text()));
            serial->write(&serial_data_list.ip_address);
            qDebug() << serial_data_list.ip_address;
            serial_data_list.ip_address.clear();
        }
        else if(current_work == CODE_SSID)
        {

        }
        else if(current_work == CODE_SCAN_AP)
        {
            serial_data_list.ip_address.append(QString("%1\r%2\r\n").arg(CODE_SCAN_AP).arg('A'));
            serial->write(&serial_data_list.ip_address);
            qDebug() << serial_data_list.ip_address;
        }

        data_response = false;
        progressbar->setValue(90);
        status_tip->setText(tr("Success"));
        progressbar->setValue(100);
    }
    else if(d.contains(receive_code_scan_ap.toUtf8()))
    {

        //SCAN 코드를 받을 때에는 명령 헤더가 \rCODE_SCAN_AP\r 로 이루어져 있음
    }
    else
    {
        qDebug() << serial_data_list.ip_address << endl;
        qDebug() << d <<endl;
    }
}

void MainWindow::uploadError()
{
    disconnect(serial,SIGNAL(data_available(QByteArray)), this, SLOT(serial_response(QByteArray)));
    disconnect(uploadTimer,SIGNAL(timeout()),this, SLOT(uploadTimeOut()));

    QMessageBox::warning(this,tr("Timeout"),tr("Device didn't response.<br>check your device or serial connecting. <br>"
                                               "if this error continually occurred, you need some help ") + link);
    uploadTimer->stop();
    progressbar->setValue(0);
    disconnect(uploadTimer,SIGNAL(timeout()),this, SLOT(uploadTimeOut()));
    data_response_timer = false;
    data_response = false;
}

void MainWindow::uploadTimeOut()
{
    uploadError();
}
