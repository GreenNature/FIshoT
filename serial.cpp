#pragma comment(lib, "advapi32")//라이브러리 등록 전처리기

#include "serial.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <windows.h>
#include <winreg.h>//레지스트리에 관한 헤더파일
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
    open_devmgmt_button->setText(QString::fromLocal8Bit("장치관리자 열기"));
    open_devmgmt_button->setToolTip(QString::fromLocal8Bit("장치관리자를 실행 합니다."));

    open_comport_button->setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);"
                                               "color : rgb(255,255,255);"));
    open_comport_button->setText(QString::fromLocal8Bit("접속하기"));
    open_comport_button->setToolTip(QString::fromLocal8Bit("장비와 접속을 시도합니다."));

    debug_clear_button->setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);"
                                               "color : rgb(255,255,255);"));
    debug_clear_button->setText(QString::fromLocal8Bit("내용 지우기"));
    debug_clear_button->setToolTip(QString::fromLocal8Bit("디버깅 창의 내용을 전부 지웁니다."));

    comport_combo->setStyleSheet(QString("background-color : rgb(75,75,75); selection-background-color : rgb(75,75,75)"));
    comport_combo->setToolTip(QString::fromLocal8Bit("장비가 연결되어 있는 포트번호를 선택합니다."));

    CLOSE_BUTTON.setStyleSheet(QString("QPushButton:hover:!pressed{"
                                               "background-color : rgb(85,85,85);"
                                               "border: 1px solid lightgray;"
                                               "}"
                                               "QPushButton:pressed{"
                                               "background-color : rgb(51,51,51);"
                                               "border : 1px solid rgb(51,51,51);"
                                               "background-color: rgb(75, 75, 75);color : rgb(255,255,255);"));

    CLOSE_BUTTON.setText(QString::fromLocal8Bit("확인"));

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
        comport_combo->addItem(QString::fromLocal8Bit("포트 없음"));
        serial_able = false;
    }
    show();
}

void Serial::comport_control_button_clicked()
{
    if(comport_status == false) //컴포트가 닫혀있으면 연다.
    {
        port->setPortName(comport_combo->currentText());
        port->setBaudRate(port->Baud115200); //보레이트 115200으로 설정
        port->setDataBits(port->Data8);
        port->setParity(port->NoParity);
        port->setStopBits(port->OneStop);
        port->setFlowControl(port->NoFlowControl);

        if(port->open(QIODevice::ReadWrite))
        {
            comport_status = true;
            open_comport_button->setText(QString::fromLocal8Bit("접속끊기"));
            open_comport_button->setToolTip(QString::fromLocal8Bit("장비와 접속을 끊습니다."));
            QMessageBox::information(this,QString("SUCCESS"),QString("Serial port open complete"));
        }
        else
        {
            QMessageBox::warning(this,QString("Fail!"),QString("Serial port open Fail"));
        }
    }
    else //컴포트가 열려있으면 닫는다.
    {
        port->close();
        open_comport_button->setText(QString::fromLocal8Bit("접속하기"));
        open_comport_button->setToolTip(QString::fromLocal8Bit("장비와 접속을 시도합니다."));
        comport_status = false;
    }
}

void Serial::text_Changed(QString text_Buffer)
{
    if(!send_Data->isNull())//보낼 데이터가 아무것도 안 쓰여져있으면 그냥 패스
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

QStringList Serial::read_comport(QStringList *com_list)//현재 연결된 컴포트 값을 불러온다.
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

int stringToNumber(const char* data, int startNum)//숫자를 표현한 문자 데이터를 숫자로 변환
{
  int number = 0, a_cipher = 1, NumberEndPoint, index=0;
  char *numberData, sign = 1; //sign : 양수 또는 음수 판별
  for (index = startNum;; index++)//숫자의 개수를 카운트
  {
    if ((data[index] == NULL) || !((48 <= data[index]) && (data[index] <= 57))) //값이 NULL 값 또는 숫자를 표현한 문자가 아닐 때의 그 지점이 숫자의 끝
    {
      if (index == startNum)//만약 숫자가 아닌 지점이 처음일 때 숫자가 없다고 판단하여 오류 메세지 출력 후 리턴 -1
      {
        if (data[index] == '-') //만약 숫자가 아닌지점이 처음인데 '-'이면 음수라고 판단
        {
          sign = -1; //부호를 음수로 바꾼다.
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
  for (int i = NumberEndPoint; i >= 0; i--) //맨 뒤의 숫자지점에서 부터 시작점 까지
  {
    numberData[i] = data[i + startNum] - 48; //숫자를 표현한 아스키코드에서 48을 빼면 표현하려는 숫자가 나옴 예) 아스키코드 56(8) - 48(0) = 8
    number += numberData[i] * a_cipher; //각 자릿수에 맞춰준다.
    a_cipher *= 10;
    printf("numberData[%d] : %d\n", i, numberData[i]);
  }
  free(numberData);//동적으로 할당한 데이터 영역 해제
  return number * sign;
}
