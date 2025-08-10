#include "qexcel.h"
#include <QAxObject>
#include <QFileDialog>
#include <QString>
#include <QDebug>
QExcel::QExcel()
{
    m_pExcel = NULL;
    m_pExcel = new QAxObject("Excel.Application");
}

QExcel::~QExcel()
{
    if (m_pExcel != NULL)
    {
        delete m_pExcel;
        m_pExcel = NULL;
    }
}


/******************************
*功能:在指定单元格插入内容
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QExcel::insertContent(int nrow, int ncol, QString sContent, QString sDataType)
{
    QString tem_pos = transColumn(ncol) + QString::number(nrow);
    QAxObject *cell = m_pActiveSheet->querySubObject("Range(QVariant,QVariant)",tem_pos);
    cell->dynamicCall("SetValue(const QVariant&)",QVariant(sContent));
    //ydc21427
    cell->setProperty("NumberFormatLocal",sDataType);
}

/******************************
*功能:初始化sheet
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QExcel::initSheet()
{
    QAxObject *myworks = m_pExcel->querySubObject("WorkBooks");
    myworks->dynamicCall("Add");
    m_pWorkbook = m_pExcel->querySubObject("ActiveWorkbook");
    QAxObject *mysheets = m_pWorkbook->querySubObject("Sheets");
    mysheets->dynamicCall("Add");
    m_pActiveSheet = m_pWorkbook->querySubObject("ActiveSheet");
}

/******************************
*功能:保存sheet
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QExcel::saveSheet(QString sPath)
{
//    QString path = QFileDialog::getSaveFileName();
    sPath.replace('/','\\');
    m_pWorkbook->dynamicCall("SaveAs(const QString&)",sPath);
}


/******************************
*功能:关闭sheet
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QExcel::closeSheet()
{
    m_pWorkbook->dynamicCall("Close()");
    m_pExcel->dynamicCall("Quit()");
}

/******************************
*功能:设置sheet名称
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QExcel::setSheetName(QString sName)
{
    m_pActiveSheet->setProperty("Name",sName);
}


/******************************
*功能:转换数字和字母
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
QString QExcel::transColumn(int nCol)
{
    if (nCol < 26){return QString('A'+nCol);}
    if (nCol >=26 && nCol <52){return QString('A')+QString('A'+nCol-26);}
    if (nCol >=52 && nCol <78){return QString('B')+QString('A'+nCol-52);}
    if (nCol >=78 && nCol <104){return QString('C')+QString('A'+nCol-78);}
    if (nCol >=104 && nCol <130){return QString('D')+QString('A'+nCol-104);}
}
