#include "qoscictrlwidget.h"
#include <QGridLayout>
#include <QFile>
#include <QMessageBox>

QOsciCtrlWidget::QOsciCtrlWidget(QOsciDevCtrl* pCtrl,QWidget *parent ) : QWidget(parent)
{
    m_pOsciDevCtrl = pCtrl;

    InitUI();

    connect(m_pOpenButton,&QPushButton::clicked,this,&QOsciCtrlWidget::onOpenButtonClick);
    connect(m_pRefreshButton,&QPushButton::clicked,this,&QOsciCtrlWidget::onRefreshButtonClick);
    connect(m_pSetParamButton,&QPushButton::clicked,this,&QOsciCtrlWidget::onSetParamButtonClick);
    connect(m_pCapDataButton,&QPushButton::clicked,this,&QOsciCtrlWidget::onCapButtonClick);
    connect(m_pSelectFileButton,&QPushButton::clicked,this,&QOsciCtrlWidget::onSelectFileButtonClick);


    if(m_pOsciDevCtrl != NULL)
    {
        QString sName = m_pOsciDevCtrl->getDevName();
        m_pOsciNameEdit->setText(sName);
        m_pOsciDevCtrl->openDev();
        connect(m_pOsciDevCtrl,&QOsciDevCtrl::signal_showOsciData,this,&QOsciCtrlWidget::showWaveData);
    }

}


void QOsciCtrlWidget::InitUI()
{
    m_pLogListWidget = new QListWidget(this);
    m_pOsciNameEdit = new QTextEdit(this);
    m_pFilePathEdit = new QTextEdit(this);
    m_pRefreshButton = new QPushButton("刷新",this);
    m_pSetParamButton = new QPushButton("设置",this);
    m_pCapDataButton = new QPushButton("采集",this);
    m_pOpenButton = new QPushButton("打开",this);
    m_pSelectFileButton = new QPushButton("...",this);


    m_pCustomPlot = new QCustomPlot(this);
    m_pCustomPlot->xAxis->setLabel("时间(ns)");
    m_pCustomPlot->xAxis->setLabelColor(QColor(0,0,0));
    m_pCustomPlot->xAxis->setTickLabelColor(QColor(0,0,0));
    m_pCustomPlot->yAxis->setLabel("数据(mv)");
    m_pCustomPlot->yAxis->setLabelColor(QColor(0,0,0));
    m_pCustomPlot->yAxis->setTickLabelColor(QColor(0,0,0));

    m_pCustomPlot->xAxis->setRange(0, 1024);//设置范围
    m_pCustomPlot->yAxis->setRange(-1, 1);

    m_pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);

    m_pCustomPlot->legend->setVisible(true);
    m_pCustomPlot->legend->setFont(QFont("Helvetica",9));
    QPen pen;
    for(int i = 0; i < M_OSCI_CHANNEL_NUM;i++)
    {
        m_pCustomPlot->addGraph();

        if(i == 0)
        {
            pen.setColor(Qt::red);
        }
        if(i == 1)
        {
            pen.setColor(Qt::green);
        }
        if(i == 2)
        {
            pen.setColor(Qt::blue);
        }
        if(i == 3)
        {
            pen.setColor(Qt::black);
        }

        m_pCustomPlot->graph(i)->setPen(pen);
        m_pCustomPlot->graph(i)->setName(QString("通道%1").arg(i+1));
    }

    InitStatusTableWidget();
    InitParamTableWidget();

    m_pOsciNameEdit->setMaximumHeight(30);
    m_pFilePathEdit->setMaximumHeight(30);
    m_pOpenButton->setMinimumHeight(30);
    //m_pOpenButton->setMaximumWidth(100);
    m_pCapDataButton->setMinimumHeight(30);
    m_pRefreshButton->setMinimumHeight(30);
   // m_pRefreshButton->setMaximumWidth(100);
    m_pCapDataButton->setMaximumWidth(100);
    m_pSetParamButton->setMinimumHeight(30);
    //m_pSetParamButton->setMaximumWidth(100);
    m_pLogListWidget->setMaximumHeight(200);
    m_pSelectFileButton->setMaximumWidth(30);
    m_pSelectFileButton->setMinimumHeight(30);

    QGridLayout* playout = new QGridLayout(this);

    //设置比例
    for(int i = 0; i < 8; i++)
    {
        playout->setColumnStretch(i,1);
    }

    playout->setSpacing(0);

    QLabel* p = new QLabel("示波器名称");
    p->setMaximumWidth(100);

    playout->addWidget( p,0,0,1,1);
    playout->addWidget(m_pOsciNameEdit,0,1,1,1);

    playout->addWidget(m_pOsciParamTable,1,0,1,3);
    playout->addWidget(m_pOsciTableStatusTable,2,0,1,3);

    playout->addWidget(new QLabel("选择波形文件"),0,4,1,1);
    playout->addWidget(m_pFilePathEdit,0,5,1,2);
    playout->addWidget(m_pSelectFileButton,0,7,1,1);
    playout->addWidget(m_pCustomPlot,1,4,2,4);

    playout->addWidget(m_pOpenButton,3,0,1,1);
    playout->addWidget(m_pRefreshButton,3,2,1,1);
    playout->addWidget(m_pSetParamButton,4,0,1,1);
    playout->addWidget(m_pCapDataButton,4,2,1,1);



    playout->addWidget(m_pLogListWidget,9,0,1,8);

}

/*******************************************************************
**功能：初始化状态参数列表
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::InitStatusTableWidget()
{
    QStringList sHorizontalHeader;
    QStringList sVerticalHeader;
    sVerticalHeader<<"通道1"<<"通道2"<<"通道3"<<"通道4";
    sHorizontalHeader<<"电压档位"<<"基线位置(V)"<<"触发电平(V)";


    m_pOsciTableStatusTable = new QTableWidget();

    m_pOsciTableStatusTable->setMaximumHeight(180);
    m_pOsciTableStatusTable->setMinimumWidth(600);

    m_pOsciTableStatusTable->setRowCount(4);
    m_pOsciTableStatusTable->setColumnCount(3);
    m_pOsciTableStatusTable->setColumnWidth(0,165);
    m_pOsciTableStatusTable->setColumnWidth(1,165);
    m_pOsciTableStatusTable->setColumnWidth(2,165);

    m_pOsciTableStatusTable->setHorizontalHeaderLabels(sHorizontalHeader);
    m_pOsciTableStatusTable->setVerticalHeaderLabels(sVerticalHeader);

    for(int nChannelNum = 0; nChannelNum < M_OSCI_CHANNEL_NUM; nChannelNum++)
    {
        m_pVoltDivCombox[nChannelNum] = new QComboBox(this);
        for(int i = 0; i < m_pOsciDevCtrl->m_tdisplayVoltDiv.size();i++)
        {
            m_pVoltDivCombox[nChannelNum]->addItem(m_pOsciDevCtrl->m_tdisplayVoltDiv.key(i));
        }
    }

    m_pOsciTableStatusTable->setItem(0,0,new QTableWidgetItem(""));

    for(int i = 0; i< m_pOsciTableStatusTable->rowCount();i++)
    {
        for(int j = 0; j <m_pOsciTableStatusTable->columnCount();j++)
        {
            if( j == 0)
            {
                 m_pOsciTableStatusTable->setCellWidget(i,j,m_pVoltDivCombox[i]);
            }
            else
                m_pOsciTableStatusTable->setItem(i,j,new QTableWidgetItem(""));
        }
    }
}

/*******************************************************************
**功能：初始化参数设置列表
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::InitParamTableWidget()
{
    QStringList sHorizontalHeader;
    QStringList sVerticalHeader;
    sHorizontalHeader<<"数据";
    sVerticalHeader<<"触发模式"<<"触发通道"<<"触发沿"<<"时间分辨率(ns)"<<"外触发电平(mv)"<<"触发延时(ns)"<<"采样频率(ps)"<<"采集平均次数";


    m_pTriggerModeCombox = new QComboBox(this);
    m_pTriggerChannelCombox = new QComboBox(this);
    m_pTriggerSlopeCombox = new QComboBox(this);

    //触发通道
    for(int i = 0; i < m_pOsciDevCtrl->m_tTriggerChannel.size(); i++)
    {
        m_pTriggerChannelCombox->addItem(m_pOsciDevCtrl->m_tTriggerChannel.key(i));
    }

    for(int i = 0; i < m_pOsciDevCtrl->m_tTriggerMode.size(); i++)
    {
        m_pTriggerModeCombox->addItem(m_pOsciDevCtrl->m_tTriggerMode.key(i));
    }

    for(int i = 0; i < m_pOsciDevCtrl->m_tTriggerSlope.size(); i++)
    {
        m_pTriggerSlopeCombox->addItem(m_pOsciDevCtrl->m_tTriggerSlope.key(i));
    }


    m_pOsciParamTable = new QTableWidget();

    m_pOsciParamTable->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    m_pOsciParamTable->setMaximumHeight(300);

    m_pOsciParamTable->setRowCount(8);
    m_pOsciParamTable->setColumnCount(1);
    m_pOsciParamTable->setColumnWidth(0,300);


    m_pOsciParamTable->setHorizontalHeaderLabels(sHorizontalHeader);
    m_pOsciParamTable->setVerticalHeaderLabels(sVerticalHeader);


    m_pOsciParamTable->setCellWidget(0,0,m_pTriggerModeCombox);
    m_pOsciParamTable->setCellWidget(1,0,m_pTriggerChannelCombox);
    m_pOsciParamTable->setCellWidget(2,0,m_pTriggerSlopeCombox);

    for(int i = 3; i< m_pOsciParamTable->rowCount();i++)
    {
          m_pOsciParamTable->setItem(i,0,new QTableWidgetItem(""));
    }
}

/*******************************************************************
**功能：打开示波器
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::onOpenButtonClick()
{
    QString sName = m_pOsciNameEdit->toPlainText();
    m_pOsciDevCtrl->setDevName(sName);

    bool bRel = m_pOsciDevCtrl->openDev();
    if(bRel == true)
    {
        onRefreshButtonClick();//刷新参数显示
        showLog("打开设备成功!");
    }
    else
        showLog("打开设备失败!");
}

/*******************************************************************
**功能：刷新参数
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::onRefreshButtonClick()
{
    bool bRel =m_pOsciDevCtrl->getDevParam();

    if(!bRel)
    {
        showLog("获取参数失败!");
        return;
    }
    else
        showLog("获取参数成功!");


    m_pTriggerModeCombox->setCurrentIndex(m_pOsciDevCtrl->m_nTriggerMode);
    m_pTriggerChannelCombox->setCurrentIndex(m_pOsciDevCtrl->m_nTriggerChannel);
    m_pTriggerSlopeCombox->setCurrentIndex(m_pOsciDevCtrl->m_nTriggerSlope[m_pOsciDevCtrl->m_nTriggerChannel]);

    //电压档位 基线位置 触发电平
    for(int i = 0; i < M_OSCI_CHANNEL_NUM;i++)
    {
        m_pVoltDivCombox[i]->setCurrentIndex(m_pOsciDevCtrl->m_nVoltDiv[i]);
        m_pOsciTableStatusTable->item(i,1)->setText(QString::number(m_pOsciDevCtrl->m_lfOffset[i]*1000,'g',2));
        m_pOsciTableStatusTable->item(i,2)->setText(QString::number(m_pOsciDevCtrl->m_lfTriggerlevel[i]*1000,'g',2));
    }
    //时间分辨
    m_pOsciParamTable->item(3,0)->setText(QString::number(m_pOsciDevCtrl->m_lfDivTime,'g',1));//ns

    //外触发电平
    m_pOsciParamTable->item(4,0)->setText(QString::number(m_pOsciDevCtrl->m_lfTriggerlevelEx*1000,'g',1));

    //触发延时
    m_pOsciParamTable->item(5,0)->setText(QString::number(m_pOsciDevCtrl->m_lfTriggerDelay* 1e9,'g',1));

    //采样频率
    m_pOsciParamTable->item(6,0)->setText(QString::number(m_pOsciDevCtrl->m_lfSampleRate,'g',1));//ps

}

/*******************************************************************
**功能：设置参数
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::onSetParamButtonClick()
{
    bool bOk = false;

    if(!m_pOsciDevCtrl->openDev())
    {
        showLog("打开示波器失败!");
      //  return;
    }

    //触发模式
    int nTriggerMode = m_pTriggerModeCombox->currentIndex();
    m_pOsciDevCtrl->setTriggerMode(nTriggerMode);

    //触发通道设置
    int nChannel = m_pTriggerChannelCombox->currentIndex();
    if(m_pOsciDevCtrl->setTriggerChannel(nChannel))
    {
        showLog("设置触发通道成功");
    }
    else
       showLog("设置触发通道失败");

    //触发模式
    int nSlope = m_pTriggerSlopeCombox->currentIndex();
    if(m_pOsciDevCtrl->setTriggerSlope(nChannel,nSlope))
    {
        showLog("设置触发模式成功");
    }
    else
       showLog("设置触发模式失败");


    //时间分辨
    float lfDivTime =  m_pOsciParamTable->item(3,0)->text().toFloat(&bOk);
    if(m_pOsciDevCtrl->setTimeDiv(lfDivTime/1e9))//ns->s
    {
        showLog("设置时间分辨成功");
    }
    else
       showLog("设置时间分辨失败");

    //外触发电平
    float lfTriggerlevelEx = m_pOsciParamTable->item(4,0)->text().toFloat(&bOk);
    if(m_pOsciDevCtrl->setTirggerLevelEx(lfTriggerlevelEx/1000.0))//ns->s
    {
        showLog("设置外触发电平成功");
    }
    else
       showLog("设置外触发电平失败");

    //触发延时
    float lfTriggerDelay =m_pOsciParamTable->item(5,0)->text().toFloat(&bOk);
    if(m_pOsciDevCtrl->setTirggerDelay(lfTriggerDelay/1e9))//ns->s
    {
        showLog("设置触发延时成功");
    }
    else
       showLog("设置触发延时失败");

    //采样频率
    float lfSampleRate = m_pOsciParamTable->item(6,0)->text().toFloat(&bOk);
    if( m_pOsciDevCtrl->setSampleRate(1e12/lfSampleRate))//ps->s
    {
        showLog("设置采样频率成功");
    }
    else
       showLog("设置采样频率失败");

    //电压档位 基线位置 触发电平
    for(int i = 0; i < M_OSCI_CHANNEL_NUM;i++)
    {
        int nIndex = m_pVoltDivCombox[i]->currentIndex();
        if( m_pOsciDevCtrl->setChannelLevel(i,nIndex))
        {
            showLog("设置电压档位成功");
        }
        else
           showLog("设置电压档位失败");

        float lfOffset = m_pOsciTableStatusTable->item(i,1)->text().toFloat(&bOk);
        if(m_pOsciDevCtrl->setOffset(i,lfOffset/1000.0))
        {
            showLog("设置基线位置成功");
        }
        else
           showLog("设置基线位置失败");


        float lfTriggerlevel = m_pOsciTableStatusTable->item(i,2)->text().toFloat(&bOk);
        if(m_pOsciDevCtrl->setTirggerLevel(i,lfTriggerlevel/1000.0))
        {
            showLog("设置触发电平成功");
        }
        else
           showLog("设置触发电平失败");
    }
}

/*******************************************************************
**功能：捕获数据
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::onCapButtonClick()
{
    QVector<double> lfPulseWidth;
    QString slog;
    QTime time = QTime::currentTime();  //获取当前时间，用于新建文件夹的名字，保存一组通道数据
    QString str = time.toString("hhmmss");
    QDate date = QDate::currentDate();
    QString str2 = date.toString("yyMMdd");
    str = str2 + "_" + str;

    int AverNum = m_pOsciParamTable->item(7,0)->text().toInt();
    for (int j=0; j<AverNum; j++)
    {
        for(int i = 0; i < M_OSCI_CHANNEL_NUM;i++)
        {
            if(m_pOsciDevCtrl->capChannelDataFile(i,str))
            {
                QString sFilePath =QApplication::applicationDirPath() + QString("/TimeWave/C%1.txt").arg(i+1);
                 slog = QString("采集通道C%1数据成功，数据存储位置:%2").arg(i+1).arg(sFilePath);
                 //显示触发通道的波形
//                 showWaveData(QApplication::applicationDirPath() + QString("/TimeWave/%1.txt").arg(m_pOsciParamTable->item(1,0)->text()));

            }
            else
                slog = QString("采集通道C%1数据失败").arg(i+1);
            showLog(slog);
        }

        lfPulseWidth.push_back(m_pOsciDevCtrl->getMeasureParam(0x01));//yhy0321
        showLog(QString("第%1次脉宽采集:%2").arg(j+1).arg(lfPulseWidth.last()));
        Sleep(300);
    }
    double AverPulseWidth = 0.0;
    for (int i=0; i<AverNum; i++)
    {
        AverPulseWidth += lfPulseWidth[i];
    }
    showLog(QString("%1次采集平均脉宽:%2").arg(AverNum).arg(AverPulseWidth/(double)AverNum));


}
/*******************************************************************
**功能：显示波形数据
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::onSelectFileButtonClick()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("波形数据文件"),"",tr("Txt (*.txt)")); //选择路径
    if(filename.length() == 0 )
    {
       return;
    }
    m_pFilePathEdit->setText(filename);
    showWaveData(filename,0,"离线波形");

}

/*******************************************************************
**功能：显示波形数据
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::showWaveData(QString sFilePath,int nChannelNum,QString sChannelName)
{
    QFile file(sFilePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","文件打开错误");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
        return;
    }

    QStringList datalist;
    int nlineCnt = 0;
    bool bOk = false;
    m_nRecordLen = 0;
    m_pTimeData.clear();
    m_pWaveData.clear();
    m_lfMaxWaveData = -1000.0;
    m_lfMinWaveData = 1000.0;

    while(!file.atEnd())//读取并解析数据
    {
        nlineCnt ++;

        QByteArray line = file.readLine();
        QString str(line);

        datalist = str.split(",");
//        if(nlineCnt == 1 )
//        {
//            m_nRecordLen = datalist[1].toInt(&bOk);
//        }

        if(nlineCnt == 6 )  //从第六行开始读取
        {
            m_lfRecordSampleRate = datalist[0].toFloat(&bOk);
        }

        if(nlineCnt >= 6)
        {
            //float lfTimedata = datalist[datalist.size()-2].toFloat(&bOk);
//            float lfTimedata = (nlineCnt-2)*m_lfRecordSampleRate* 1e9;//使用相对时间
            float lfTimedata = (datalist[0].toFloat(&bOk) - m_lfRecordSampleRate)*1e9;//使用相对时间
            float lfWavedata = datalist[datalist.size()-1].toFloat(&bOk)*1000;//转换为mv
            m_pTimeData.append(lfTimedata);
            m_pWaveData.append(lfWavedata);

            m_lfMaxWaveData = qMax(m_lfMaxWaveData,lfWavedata);
            m_lfMinWaveData = qMin(m_lfMinWaveData,lfWavedata);
            m_lfMaxTimeData = lfTimedata;
        }


    }

    //绘图显示数据
    m_pCustomPlot->xAxis->setRange(0,m_lfMaxTimeData);
    m_pCustomPlot->yAxis->setRange(m_lfMinWaveData-0.01,m_lfMaxWaveData+0.01);
    if(nChannelNum <= M_OSCI_CHANNEL_NUM)
    {
        m_pCustomPlot->graph(nChannelNum)->setData(m_pTimeData,m_pWaveData);
        m_pCustomPlot->graph(nChannelNum)->setName(sChannelName);
        m_pCustomPlot->replot();
    }

}

/*******************************************************************
**功能：显示日志信息
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciCtrlWidget::showLog(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_pLogListWidget->addItem(str);
    m_pLogListWidget->setCurrentRow(m_pLogListWidget->count()-1);
}
