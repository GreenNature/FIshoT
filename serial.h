#ifndef SERIAL_H
#define SERIAL_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QComboBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QLayout>
#include <QtSerialPort/QSerialPort>
#include <QProgressBar>

class Serial : public QDialog
{
    Q_OBJECT
public:
    explicit Serial(QWidget *parent);
    ~Serial();
    void setting_open();
    void write(QString *d);
    void write(QByteArray *d);
    void write(char *d);
    bool able();
    QStringList read_comport(QStringList *com_list = nullptr);


private:
    QSerialPort* port = new QSerialPort();
    QByteArray* send_Data;
    QByteArray read_Data;
    QString ComportName;
    QPushButton *open_devmgmt_button = new QPushButton(this);
    QPushButton *open_comport_button = new QPushButton(this);
    QPushButton *debug_clear_button  = new QPushButton(this);
    QPushButton CLOSE_BUTTON;
    QComboBox *comport_combo = new QComboBox(this);
    QTextEdit *debug_receive_edit = new QTextEdit(tr("\tdebug\n"),this);

    void text_Changed(QString text_Buffer);
    void text_Sending();
    void create_comport_list();

    bool comport_status = false;
    bool serial_able = false;

private slots:
    void data_receive();
    void debug_clear();

public slots:
    void open_devmgmt();
    void comport_control_button_clicked();

signals:
    void data_available(QByteArray d);
};

#endif // SERIAL_H
