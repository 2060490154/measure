#include "QErrCodeProcess.h"
#include <QMessageBox>

QErrCodeProcess::QErrCodeProcess(QObject *parent) : QObject(parent)
{
    initErrCode();
}

/*******************************************************************
**功能：messagebox显示错误信息
**输入：nerrCode 错误代码
**输出：错误信息
**返回值：无
*******************************************************************/
void QErrCodeProcess::showErrCode(int nerrCode)
{
     QString sDespcrip = _tErrCodeMap.find(nerrCode).value();

    QMessageBox messageBox(QMessageBox::Warning,"提示",sDespcrip);
    messageBox.setStandardButtons (QMessageBox::Ok);
    messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
    messageBox.exec ();
}


 /*******************************************************************
**功能：初始化错误列表
**输入：
**输出：
**返回值：
*******************************************************************/
 void QErrCodeProcess::initErrCode()
 {
    _tErrCodeMap.insert(M_ERRCODE_INSERTDB,"插入数据库失败");
    _tErrCodeMap.insert(M_ERRCODE_UPDATEDB,"更新数据库失败");

 }
