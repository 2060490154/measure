/***************************************************************************
**                                                                                               **
**QOsciDevCtrl为示波器设备控制类                                     **
** 提供的功能函数为：
** getDevStatus
**openDev                    建立连接
**closeDev                    断开连接
**setTriggerMode         设置触发模式
**setSampleRate            设置采样率
**setTimeDiv                  设置时间分辨率
**setTirggerDelay           设置触发延时
**setTirggerLevel            设置触发电平
**setTriggerChannel       设置触发通道和触发沿类型  默认触发沿类型为pos
**setChannelLevel          设置通道的电压档位
** capChannelData         采集数据
**
****************************************************************************
**创建人：李刚
**创建时间：2018.08.01
**修改记录：
**
****************************************************************************/
#include "QOsciDevCtrl.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <windows.h>
#include <QMessageBox>
QOsciDevCtrl::QOsciDevCtrl(QString sIP,QList<tOsciiChannel> vaildChannellist)
{
    _sIpAddress = sIP;
    _sVisaName = "IP:"+sIP;
    _bOpenStatus = false;

    _vaildChannellist = vaildChannellist;

    m_tTriggerMode.insert("AUTO",0);
    m_tTriggerMode.insert("NORM",1);
    m_tTriggerMode.insert("SINGLE",2);
    m_tTriggerMode.insert("STOP",3);

    m_tTriggerSlope.insert("POS",0);
    m_tTriggerSlope.insert("NEG",1);

    m_tTriggerChannel.insert("C1",0);
    m_tTriggerChannel.insert("C2",1);
    m_tTriggerChannel.insert("C3",2);
    m_tTriggerChannel.insert("C4",3);
    m_tTriggerChannel.insert("Ext",4);

    m_tVoltDiv.insert("2",0);
    m_tVoltDiv.insert("1",1);
    m_tVoltDiv.insert("0.5",2);
    m_tVoltDiv.insert("0.2",3);
    m_tVoltDiv.insert("0.1",4);
    m_tVoltDiv.insert("0.05",5);
    m_tVoltDiv.insert("0.02",6);
    m_tVoltDiv.insert("0.01",7);
    m_tVoltDiv.insert("0.005",8);
    m_tVoltDiv.insert("0.002",9);

    m_tdisplayVoltDiv.insert("2.0 V",0);
    m_tdisplayVoltDiv.insert("1.0 V",1);
    m_tdisplayVoltDiv.insert("500 mV",2);
    m_tdisplayVoltDiv.insert("200 mV",3);
    m_tdisplayVoltDiv.insert("100 mV",4);
    m_tdisplayVoltDiv.insert("50 mV",5);
    m_tdisplayVoltDiv.insert("20 mV",6);
    m_tdisplayVoltDiv.insert("10 mV",7);
    m_tdisplayVoltDiv.insert("5 mV",8);
    m_tdisplayVoltDiv.insert("2 mV",9);
}

QOsciDevCtrl::~QOsciDevCtrl()
{
    closeDev();
}


/*******************************************************************
**功能：返回当前设备状态 用于检测是否连接正常
**输入：无
**输出：无
**返回值：是否打开  true--打开成功 false--打开失败
*******************************************************************/
bool QOsciDevCtrl::  getDevStatus()
{
   return _bOpenStatus;
}

/*******************************************************************
**功能：打开设备
**输入：无
**输出：无
**返回值：true-打开成功 false--打开失败
*******************************************************************/
 bool QOsciDevCtrl::openDev()
 {
     if(_bOpenStatus == true)
     {
         return true;
     }

    if(!_OsciDso.MakeConnection(_sVisaName))
    {
        return false;
    }
    _OsciDso.SetRemoteLocal(1);
    _bOpenStatus = true;

    return true;
 }

 /*******************************************************************
 **功能：关闭设备连接
 **输入：无
 **输出：无
 **返回值：true-打开成功 false--打开失败
 *******************************************************************/
  bool QOsciDevCtrl::closeDev()
  {
      bool bRel = false;
      if(!_bOpenStatus)
      {
          return true;
      }

      if(_OsciDso.Disconnect())
      {
          _bOpenStatus = false;
          bRel = true;
      }

       return bRel;
  }



  /*******************************************************************
**功能：获取示波器当前工作参数
**输入：
**输出：
**返回值：
*******************************************************************/
 bool QOsciDevCtrl::getDevParam()
  {
     bool bOk = false;

     if(!_bOpenStatus)
     {
         return false;
     }

     QString  sReplyString;
     QString sCmdString;

     //触发模式  
     _OsciDso.WriteString("TRIG_MODE?",1);
     sReplyString = _OsciDso.ReadString(20);
     m_nTriggerMode=m_tTriggerMode.value(sReplyString);

     //触发延时
     _OsciDso.WriteString("TRIG_DELAY?",1);
     sReplyString = _OsciDso.ReadString(20);
     m_lfTriggerDelay =sReplyString.toFloat(&bOk) ;

     //触发通道
     _OsciDso.WriteString("VBS? 'Return = app.Acquisition.Trigger.Edge.Source'", 1);
     sReplyString = _OsciDso.ReadString(30);
     m_nTriggerChannel =m_tTriggerChannel.value(sReplyString);

     //触发类型
     for(int i = 0; i < 4; i++)
     {
          sCmdString = QString("C%1:TRIG_SLOPE?").arg(i+1);
           _OsciDso.WriteString(sCmdString,1);
           sReplyString = _OsciDso.ReadString(20);
           m_nTriggerSlope[i] = m_tTriggerSlope.value(sReplyString);
     }

     //获取外触发电平
    sCmdString = QString("EX:TRLV?");
    _OsciDso.WriteString(sCmdString,1);
    sReplyString = _OsciDso.ReadString(20);
    m_lfTriggerlevelEx = sReplyString.toFloat(&bOk) ;


     //时间分辨率  ns
     _OsciDso.WriteString("TIME_DIV?",1);
     sReplyString = _OsciDso.ReadString(20);
     m_lfDivTime = sReplyString.toFloat(&bOk) ;
     m_lfDivTime = m_lfDivTime*1e9;

     //基线位置
     for(int i = 0; i < 4; i++)
     {
            sCmdString = QString("C%1:OFFSET?").arg(i+1);
            _OsciDso.WriteString(sCmdString,1);
            sReplyString = _OsciDso.ReadString(20);
            m_lfOffset[i] = sReplyString.toFloat(&bOk) ;
     }

     //电压档位
     for(int i = 0; i < 4; i++)
     {
            sCmdString = QString("C%1:VOLT_DIV?").arg(i+1);
            _OsciDso.WriteString(sCmdString,1);
            sReplyString = _OsciDso.ReadString(20);
            m_nVoltDiv[i] = m_tVoltDiv.value(sReplyString);
     }

     //采样率
     _OsciDso.WriteString("VBS? 'Return = app.Acquisition.Horizontal.SamplingRate'", 1);
     sReplyString = _OsciDso.ReadString(30);
     m_lfSampleRate=sReplyString.toFloat(&bOk);//G/s M/s
     if(m_lfSampleRate != 0.0)
     {
         m_lfSampleRate = 1/m_lfSampleRate *1e12;//采样率 ps
     }

     //触发电平
     for(int i = 0; i < 4; i++)
     {
            sCmdString = QString("C%1:TRLV?").arg(i+1);
            _OsciDso.WriteString(sCmdString,1);
            sReplyString = _OsciDso.ReadString(20);
            m_lfTriggerlevel[i] = sReplyString.toFloat(&bOk) ;
     }

     return true;
  }



  /*******************************************************************
**功能：设置触发模式
**输入：nTriggerMode--0 表示auto 1--表示normal模式 2--表示signal模式
**输出：无
**返回值：true成功 false失败
*******************************************************************/
bool QOsciDevCtrl::setTriggerMode(int nTriggerMode)
{

    QString  sMode = m_tTriggerMode.key(nTriggerMode);

    QString strCmd;
    strCmd = QString("TRMD %1").arg(sMode);

    return _OsciDso.WriteString(strCmd,1);//设置模式
}

/*******************************************************************
**功能：设置采样率
**输入：lfSampleRate 采样率  单位为每秒多少次
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setSampleRate(float lfSampleRate)
{
    QString sSampleRate = QString::number(lfSampleRate,'e',1);
    QString  strCmd = QString("VBS 'app.Acquisition.Horizontal.SampleRate = %1'").arg(sSampleRate);
    return _OsciDso.WriteString(strCmd, 1);
}

 /*******************************************************************
 **功能：设置时间分辨率
 **输入：lfTimeDiv 时间采样率  单位为S
 **输出：无
 **返回值：true 成功 false 失败
 *******************************************************************/
bool QOsciDevCtrl::setTimeDiv(float lfTimeDiv)
{
    QString sTimeDiv = QString::number(lfTimeDiv,'e');
    QString  strCmd = QString("TIME_DIV %1").arg(sTimeDiv);
    return _OsciDso.WriteString(strCmd,1);
}

  /*******************************************************************
  **功能：设置外部触发电平
  **输入：lfTriggerLevel 触发电平  单位为v
  **输出：无
  **返回值：true 成功 false 失败
  *******************************************************************/
bool QOsciDevCtrl::setTirggerLevelEx(float lfTriggerLevel)
{
    QString sTriggerLevel = QString::number(lfTriggerLevel,'g',3);
    QString  strCmd = QString("EX:TRIG_LEVEL %1 V").arg(sTriggerLevel);
    return _OsciDso.WriteString(strCmd,1);
}

/*******************************************************************
**功能：设置触发电平
**输入：nChannel通道号 lfOffset为为触发电平 单位为V
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setTirggerLevel(int nChannel,float lfTriggerLevel)
{
    QString sTriggerLevel = QString::number(lfTriggerLevel,'f',3);
    QString  strCmd = QString("C%1:TRIG_LEVEL %2 V").arg(nChannel+1).arg(sTriggerLevel);
    return _OsciDso.WriteString(strCmd,1);

}

/*******************************************************************
**功能：设置基线位置
**输入：nChannel通道号 lfOffset为为基线位置 单位为V
**输出：
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setOffset(int nChannel,float lfOffset)
{
    QString sTriggerLevel = QString::number(lfOffset,'g',3);
    QString  strCmd = QString("C%1:OFFSET %2 V").arg(nChannel+1).arg(sTriggerLevel);
    return _OsciDso.WriteString(strCmd,1);
}

/*******************************************************************
**功能：设置触发延时
**输入：lfDelayTime 触发延时 单位为s
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setTirggerDelay(float lfDelayTime)
{
    QString  strCmd = QString("TRIG_DELAY %1").arg(lfDelayTime);
     return _OsciDso.WriteString(strCmd,1);//触发延时 ok 单位秒
}

/*******************************************************************
**功能：设置触发通道
**输入：nChannelNum 通道号 从0开始
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setTriggerChannel(int nChannelNum)
{
    QString strCmd;
    strCmd = QString("VBS 'app.Acquisition.Trigger.Edge.Source = %1'").arg(nChannelNum);
     return _OsciDso.WriteString(strCmd,1);
}

/*******************************************************************
**功能：设置通道的触发沿类型
**输入：nChannelNum 通道号 从0开始
**          nTriggerSlope触发沿类型  0--pos 1-neg
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setTriggerSlope(int nChannelNum,int nTriggerSlope)
{
    QString sTriggerSlope = "POS";
    if(nTriggerSlope == 1)
    {
        sTriggerSlope = "NEG";
    }
    QString  strCmd = QString("C%1:TRIG_SLOPE %2").arg(nChannelNum+1).arg(sTriggerSlope);
     return _OsciDso.WriteString(strCmd,1);
}

/*******************************************************************
**功能：设置通道的电压档位
**输入：nChannelNum 触发通道号 从0开始
**          lfLevel电压档位  单位V
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::setChannelLevel(int nChannelNum,int nLevel)
{
    QString  strCmd = QString("C%1:VOLT_DIV %2 V").arg(nChannelNum+1).arg(m_tVoltDiv.key(nLevel));
     return _OsciDso.WriteString(strCmd,1);
}

/*******************************************************************
**功能：采集数据
**输入：nChannelNum--通道号 从0开始
**输出：波形文件 默认存储在app当前目录下 TimeWave文件夹，命名为C1-C4.txt
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::capChannelData(int nChannelNum)
{
    bool bRel = false;
    bool bOk = false;
    QString replyString = "";
    QString strAmplitudeVal="";//接收的数据信息
    QString sSaveData;
    QString sTimeData;
    QString sValueData;
    float lfTimeData;


    QString strChannel = QString("C%1").arg(nChannelNum+1);
    int nMaxRcvDataLen = 6000;//maximum length of each transfer

    //创建所要保存的文件
    QString sDirPath =  QApplication::applicationDirPath()+"\\TimeWave";
    QDir targetDir(sDirPath);
    if(!targetDir.exists())
    {
        if(!targetDir.mkdir(targetDir.absolutePath()))//创建目录
        return false;
    }

    QString sFilePath = QApplication::applicationDirPath()+"\\TimeWave\\" +strChannel+".txt";
     QFile saveFile(sFilePath);
    if(!saveFile.open(QFile::ReadWrite))
    {
        return false;
    }
    QTextStream outFile(&saveFile);

    //发送接收数据命令
   bRel =  _OsciDso.WriteString("WFSU NP,0,SP,0,FP,0,SN,0\n",1);
   if(bRel == false)
   {
       return false;
   }
    QString strCmd = strChannel +":inspect? simple\n";
    bRel = _OsciDso.WriteString(strCmd,1);
    if(bRel == false)
    {
        return false;
    }

    //获取采样数据
#if 0
    //;
    replyString="temple";
    while(replyString != NULL)
    {
        replyString = _OsciDso.ReadString(6000);
        strAmplitudeVal += replyString.replace("\r\n","").replace("\"","").replace("  "," ");
//        nMaxRcvDataLen = replyString.size();
    }
#else
    strAmplitudeVal.clear();
    while(nMaxRcvDataLen == 6000)
    {
        replyString = _OsciDso.ReadString(6000);
        strAmplitudeVal += replyString;
        nMaxRcvDataLen = replyString.size();
    }
    strAmplitudeVal = strAmplitudeVal.replace("\"","").replace("\r\n","").replace("  "," ");	//ytry

    QString sCmd = QString("VBS 'app.SaveRecall.Waveform.DoSave'");//执行保存数据
    bRel =  _OsciDso.WriteString(sCmd,1);
    if(bRel == false)
    {
        return false;
    }
#endif
    if(nMaxRcvDataLen == 0 )
    {
        return false;
    }

    //获取采样率 计算时标  ps
    _OsciDso.WriteString("VBS? 'Return = app.Acquisition.Horizontal.SamplingRate'", 1);
    QString sReplyString = _OsciDso.ReadString(30);
    float lfSampleRate=sReplyString.toFloat(&bOk) ;
    lfSampleRate = 1/lfSampleRate ;//采样率 单位为S   每秒多少次 转换成 每次的时间

    if (strAmplitudeVal=="")
    {
        return false;
    }

    QStringList valueDatalist = strAmplitudeVal.split(' ');
    int nMaxlenNum =valueDatalist.size();

    //兼容以前的格式 添加文件头
    outFile<<" "<<"\n";
    outFile<<"\"Sample Interval\""<<"\t"<<QString::number(lfSampleRate,'e',2)<<"\t S\t";

    for(int i = 0; i < nMaxlenNum;i++)
    {
        //解析时间数据  计算时标
        lfTimeData = lfSampleRate * i;
        sTimeData = QString::number(lfTimeData,'e',2);
        //解析数据
        sValueData = valueDatalist.at(i);

        //保存数据
        sSaveData = QString("%1,%2").arg(sTimeData).arg(sValueData);
        outFile<<sSaveData<<"\n";
    }

    for(int i = 0; i < _vaildChannellist.size();i++)
    {
        tOsciiChannel channelparam = _vaildChannellist.at(i);

        if(channelparam.m_nChannelNum == nChannelNum)
        {
            emit signal_showOsciData(sFilePath,nChannelNum,channelparam.m_sChannelName);
            break;
        }
    }

    return true;
}


/*******************************************************************
**功能：采集数据 文件的方式获取
**输入：nChannelNum--通道号 从0开始
**输出：波形文件 默认存储在app当前目录下 TimeWave文件夹，命名为C1-C4.txt
**返回值：true 成功 false 失败
*******************************************************************/
bool QOsciDevCtrl::capChannelDataFile(int nChannelNum, QString curtime)
{
    bool bRel = false;
    QString sCmd;
    QString replyString = "";
    QString strAmplitudeVal="";//接收的数据信息
    QString strChannel = QString("C%1").arg(nChannelNum+1);

    //创建所要保存的文件
    QString sDirPath =  QApplication::applicationDirPath()+"\\TimeWave";
    QString sFilePath = sDirPath +"\\" +strChannel+".txt";//本地文件路径
//    QString sTempFilePath = QString("SaveData\\%1.txt").arg(strChannel);




    QString sTempFilePath = QString("SaveData\\%1\\").arg(curtime);
    QString sDataFilePath = sTempFilePath + QString("%1_00000.csv").arg(strChannel);
//    QString sDataFilePath = QString("SaveData\\%1_00000.csv").arg(strChannel);
    QString  sRemoteFilePath = QString("D:\\%1").arg(sTempFilePath);//示波器文件路径



    QDir targetDir(sDirPath);
    if(!targetDir.exists())
    {
        if(!targetDir.mkdir(targetDir.absolutePath()))//创建目录
        return false;
    }

    if(QFile::exists(sFilePath))    //清理上一次采集的数据
    {
        QFile::remove(sFilePath);
    }

    //发送存储命令
   sCmd = QString("VBS 'app.SaveRecall.Waveform.SaveTo = \"File\"'");//设置存储格式为文件
   bRel =  _OsciDso.WriteString(sCmd,1);
   if(bRel == false)
   {
       return false;
   }

   sCmd = QString("VBS 'app.SaveRecall.Waveform.SaveSource = \"%1\"'").arg(strChannel);//设置保存通道
   bRel =  _OsciDso.WriteString(sCmd,1);
   if(bRel == false)
   {
       return false;
   }

//   sCmd = QString("VBS 'app.SaveRecall.Waveform.SaveDestination = \"%1\"'").arg(sRemoteFilePath);//设置保存数据路径
   sCmd = QString("VBS 'app.SaveRecall.Waveform.WaveformDir = \"%1\"'").arg(sRemoteFilePath);//设置保存数据路径
   bRel =  _OsciDso.WriteString(sCmd,1);
   if(bRel == false)
   {
       return false;
   }

   sCmd = QString("VBS 'app.SaveRecall.Waveform.WaveFormat = \"Excel\"'");//保存文件格式
   bRel =  _OsciDso.WriteString(sCmd,1);
   if(bRel == false)
   {
       return false;
   }

   sCmd = QString("VBS 'app.SaveRecall.Waveform.DoSave'");//执行保存数据
   bRel =  _OsciDso.WriteString(sCmd,1);
   if(bRel == false)
   {
       return false;
   }

   //获取数据
  QString sScrFilePath;
  sScrFilePath="\\\\"+_sIpAddress+"\\"+sDataFilePath;
  Sleep(200);
  bool copytask = QFile::copy(sScrFilePath,sFilePath);
  if(!copytask)
  {
      return false;
  }

  for(int i = 0; i < _vaildChannellist.size();i++)
  {
      tOsciiChannel channelparam = _vaildChannellist.at(i);

      if(channelparam.m_nChannelNum == nChannelNum)
      {
          emit signal_showOsciData(sFilePath,nChannelNum,channelparam.m_sChannelName);
          break;
      }
  }

    return true;
}


/*******************************************************************
**功能：获取设备名称
**输入：
**输出：
**返回值：
*******************************************************************/

QString QOsciDevCtrl::getDevName()
{
    return _sVisaName;

}

/*******************************************************************
**功能：设置设备名称
**输入：
**输出：
**返回值：
*******************************************************************/
void QOsciDevCtrl::setDevName(QString sName)
{
    _sVisaName = sName;
}


/*******************************************************************
**功能：获取参数
**输入：nPxIndex-1 幅值 2为脉宽
**输出：
**返回值：
*******************************************************************/
double QOsciDevCtrl::getMeasureParam(int nParamIndex)
{
    double lfRel = 0.0;
    bool bOk = false;
    QString sTemp;

    sTemp = QString("VBS? 'Return = app.Measure.P%1.last.Result.Value'").arg(nParamIndex);
    _OsciDso.WriteString(sTemp, 1);//获取测量结果
    sTemp = _OsciDso.ReadString(30);
    lfRel = sTemp.toFloat(&bOk);
//    QMessageBox::warning(nullptr, "執行",QString("%1").arg(lfRel));
    return lfRel;
}
