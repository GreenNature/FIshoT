#pragma comment(lib, "advapi32")//���̺귯�� ��� ��ó����

#include "serial.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <windows.h>
#include <winreg.h>//������Ʈ���� ���� �������
#include <QToolTip>
#include <stdio.h>

Serial::Serial(QWidget *parent) : QDialog(parent)
{
    setStyleSheet(QString("selection-background-color :rgb(51,51,51); background-color : rgb(51,51,51); "
                            "color : rgb(255,255,255);"));

    setWindowModality(Qt::WindowModal);
    setModal(true);
    hide();

    open_devmgmt_button->setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);"
                                               "color : rgb(255,255,255);"));
    open_devmgmt_button->setText(QString::fromLocal8Bit("��ġ������ ����"));
    open_devmgmt_button->setToolTip(QString::fromLocal8Bit("��ġ�����ڸ� ���� �մϴ�."));

    open_comport_button->setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);"
                                               "color : rgb(255,255,255);"));
    open_comport_button->setText(QString::fromLocal8Bit("�����ϱ�"));
    open_comport_button->setToolTip(QString::fromLocal8Bit("���� ������ �õ��մϴ�."));

    debug_clear_button->setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);"
                                               "color : rgb(255,255,255);"));
    debug_clear_button->setText(QString::fromLocal8Bit("���� �����"));
    debug_clear_button->setToolTip(QString::fromLocal8Bit("����� â�� ������ ���� ����ϴ�."));

    comport_combo->setStyleSheet(QString("background-color : rgb(75,75,75); selection-background-color : rgb(75,75,75)"));
    comport_combo->setToolTip(QString::fromLocal8Bit("��� ����Ǿ� �ִ� ��Ʈ��ȣ�� �����մϴ�."));

    CLOSE_BUTTON.setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);color : rgb(255,255,255);"));

    CLOSE_BUTTON.setText(QString::fromLocal8Bit("Ȯ��"));

    debug_receive_edit->setReadOnly(true);

    QHBoxLayout *hboxlayout = new QHBoxLayout();
    hboxlayout->addWidget(open_comport_button);
    hboxlayout->addWidget(debug_clear_button);

    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->addWidget(comport_combo);
    vboxlayout->addLayout(hboxlayout);
    vboxlayout->addWidget(debug_receive_edit);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(&CLOSE_BUTTON);
    rightLayout->addWidget(open_devmgmt_button);
    rightLayout->addStretch();

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addLayout(vboxlayout,0,0);
    mainLayout->addLayout(rightLayout,0,1);
    setLayout(mainLayout);

    resize(sizeHint().width()+10,sizeHint().height()+10);
    setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    setSizeGripEnabled(false);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    connect(open_devmgmt_button,SIGNAL(clicked(bool)), this, SLOT(open_devmgmt()));
    connect(open_comport_button,SIGNAL(clicked(bool)), this, SLOT(comport_control_button_clicked()));
    connect(&CLOSE_BUTTON,SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(port, SIGNAL(readyRead()),this, SLOT(data_receive()));
    connect(debug_clear_button, SIGNAL(clicked(bool)), this, SLOT(debug_clear()));

}

Serial::~Serial()
{

}

bool Serial::able()
{
    return serial_able;
}

void Serial::setting_open()
{
    QStringList _comports = read_comport();
    bool sameThing = false;
    if(_comports.length() > 0)
    {
        for(int i=0; i<_comports.length(); i++)
        {
            for(int j=0; j<comport_combo->count(); j++)
                if(comport_combo->itemText(j).compare(_comports.at(i)) == 0)
                {
                    sameThing = true;
                }

            if(!sameThing)
                comport_combo->addItem(_comports.at(i));

            sameThing = false;
        }
        serial_able = true;
    }
    else
    {
        comport_combo->addItem(QString::fromLocal8Bit("��Ʈ ����"));
        serial_able = false;
    }
    show();
}

void Serial::comport_control_button_clicked()
{
    if(comport_status == false) //����Ʈ�� ���������� ����.
    {
        port->setPortName(comport_combo->currentText());
        port->setBaudRate(port->Baud115200); //������Ʈ 115200���� ����
        port->setDataBits(port->Data8);
        port->setParity(port->NoParity);
        port->setStopBits(port->OneStop);
        port->setFlowControl(port->NoFlowControl);

        if(port->open(QIODevice::ReadWrite))
        {
            comport_status = true;
            open_comport_button->setText(QString::fromLocal8Bit("���Ӳ���"));
            open_comport_button->setToolTip(QString::fromLocal8Bit("���� ������ �����ϴ�."));
            QMessageBox::information(this,QString("SUCCESS"),QString("Serial port open complete"));
        }
        else
        {
            QMessageBox::warning(this,QString("Fail!"),QString("Serial port open Fail"));
        }
    }
    else //����Ʈ�� ���������� �ݴ´�.
    {
        port->close();
        open_comport_button->setText(QString::fromLocal8Bit("�����ϱ�"));
        open_comport_button->setToolTip(QString::fromLocal8Bit("���� ������ �õ��մϴ�."));
        comport_status = false;
    }
}

void Serial::text_Changed(QString text_Buffer)
{
    if(!send_Data->isNull())//���� �����Ͱ� �ƹ��͵� �� ������������ �׳� �н�
    {
        send_Data = new QByteArray(text_Buffer.toStdString().c_str());
    }
}


void Serial::text_Sending()
{
    port->write("");
}

void Serial::write(char *d)
{
    port->write(d);
}

void Serial::write(QByteArray *d)
{
    port->write(*d);
}

void Serial::write(QString *d)
{
    port->write(d->toUtf8());
}

QStringList Serial::read_comport(QStringList *com_list)//���� ����� ����Ʈ ���� �ҷ��´�.
{
    HKEY hKey;
    RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 			&hKey);

    TCHAR szName[100], szData[20];
    DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
    memset(szData, 0x00, sizeof(szData));
    memset(szName, 0x00, sizeof(szName));

    QStringList _list;

    while (ERROR_SUCCESS ==
        RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL))
    {
        index++;
        RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, 					&dwSize2);
//        ui->comportCombo->addItem(QString::fromWCharArray(szData));
        _list << QString::fromWCharArray(szData);
        memset(szData, 0x00, sizeof(szData));
        memset(szName, 0x00, sizeof(szName));
        dwSize = 100;
        dwSize2 = 20;
    }
    com_list = &_list;
    return _list;
    RegCloseKey(hKey);
}

void Serial::open_devmgmt()
{
    system("start devmgmt.msc");
}

void Serial::data_receive()
{
    read_Data = port->readAll();
    //ui->labReceive->append(read_Data);
    debug_receive_edit->insertPlainText(read_Data);
    debug_receive_edit->moveCursor(QTextCursor::End);
    //qDebug() << read_Data.length() <<endl;
    emit(data_available(read_Data));
}

void Serial::debug_clear()
{
    debug_receive_edit->clear();
}

int stringToNumber(const char* data, int startNum)//���ڸ� ǥ���� ���� �����͸� ���ڷ� ��ȯ
{
  int number = 0, a_cipher = 1, NumberEndPoint, index=0;
  char *numberData, sign = 1; //sign : ��� �Ǵ� ���� �Ǻ�
  for (index = startNum;; index++)//������ ������ ī��Ʈ
  {
    if ((data[index] == NULL) || !((48 <= data[index]) && (data[index] <= 57))) //���� NULL �� �Ǵ� ���ڸ� ǥ���� ���ڰ� �ƴ� ���� �� ������ ������ ��
    {
      if (index == startNum)//���� ���ڰ� �ƴ� ������ ó���� �� ���ڰ� ���ٰ� �Ǵ��Ͽ� ���� �޼��� ��� �� ���� -1
      {
        if (data[index] == '-') //���� ���ڰ� �ƴ������� ó���ε� '-'�̸� ������� �Ǵ�
        {
          sign = -1; //��ȣ�� ������ �ٲ۴�.
          startNum++;
          continue;
        }
        else
        {
          printf("ERROR! : Received data is not number");
          return -1;
        }
      }
      NumberEndPoint = index-startNum-1;
      break;
    }
  }
  numberData = (char*)malloc(sizeof(char)* NumberEndPoint + 1);
  for (int i = NumberEndPoint; i >= 0; i--) //�� ���� ������������ ���� ������ ����
  {
    numberData[i] = data[i + startNum] - 48; //���ڸ� ǥ���� �ƽ�Ű�ڵ忡�� 48�� ���� ǥ���Ϸ��� ���ڰ� ���� ��) �ƽ�Ű�ڵ� 56(8) - 48(0) = 8
    number += numberData[i] * a_cipher; //�� �ڸ����� �����ش�.
    a_cipher *= 10;
    printf("numberData[%d] : %d\n", i, numberData[i]);
  }
  free(numberData);//�������� �Ҵ��� ������ ���� ����
  return number * sign;
}
