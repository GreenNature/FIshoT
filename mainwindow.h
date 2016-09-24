#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include "serial.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Serial *serial = new Serial(this);

#define CODE_IP 100
#define CODE_SSID 101
#define CODE_PASSWD 102
#define CODE_SCAN_AP 103

private slots:
    void information();
    void about();
    void setting();
    void read_comport_event();
    void serial_send_data();
    void serial_response(QByteArray d);
    void uploadTimeOut();
    void password_visible_slot(bool checked);
    void update_AP_List(QStringList apList);

private:
    void createActions();
    void createMenus();
    void createContextMenu();
    void createStatusBar();
    void settingWifi();
    void data_parse();
    void uploadError();
    bool data_response = false;
    bool data_response_timer = false;

    typedef struct
    {
        QByteArray ip_address;
        QByteArray ssid;
        QByteArray passwd;
        QByteArray port;
    }serial_data_list_;
    serial_data_list_ serial_data_list;

    typedef struct AP_DATA_
    {
        QString *SSID = new QString();
        QString *RSSI = new QString();
        int SECURE_TYPE = NULL;
    }AP_DATA;

    enum SecureType {
      SECURE_TYPE_OPEN,
      SECURE_TYPE_WEP,
      SECURE_TYPE_WPA
    };

    void createToolBar();
    int current_work = NULL;
    QString link = QString("<a href=\"%1\">%1</a>").arg("www.naver.com");
    QTimer *uploadTimer = new QTimer(this);
    QMenu *fileMenu;
    QMenu *infoMenu;
    QMenu *settingMenu;
    QMenu *settingSubMenu;
    QMenu *helpMenu;
    QToolBar *mainToolBar = new QToolBar(this);
    QToolBar *infoToolBar;
    QToolBar *settingToolBar;
    QAction *infoAction;
    QAction *settingAction;
    QAction *aboutAction;
    QAction *SerialSettingAction;
    QLabel *status_tip = new QLabel(tr("wowsystem"));
    QProgressBar *progressbar = new QProgressBar(this);

    QPixmap* m_pPixmap;
    QPalette* m_pPalette;

protected:
    void resizeEvent (QResizeEvent* event);
};

#endif // MAINWINDOW_H
