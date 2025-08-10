#include "qDatabaseProcess.h"
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qdebug.h>
#include <qstringlist.h>
#include <QSqlTableModel>

QDatabaseProcess::QDatabaseProcess()
{
    m_bOpendb = false;

}
QDatabaseProcess::~QDatabaseProcess()
{
    if(m_db.isOpen())
    {
        m_db.close();
    }

}

bool QDatabaseProcess::connectDatabase(QString sDataBaseName,int nDatabaseType,QString sServerName,QString sUserName,QString sPassword)
{
    QString dsn;
    QString sDataBaseType;

    if(M_DATABASE_ACCESS == nDatabaseType)
    {
        dsn = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(sDataBaseName);//连接字符串
        sDataBaseType = "QODBC";
    }
    if(M_DATABASE_SQLSERVER == nDatabaseType)
    {
        dsn = QString::fromLocal8Bit("QTDSN");//连接字符串
        sDataBaseType = "QODBC";
    }

    m_db = QSqlDatabase::addDatabase(sDataBaseType);//设置数据库驱动

    m_db.setHostName(sServerName);//设置服务器名称

    if(!sUserName.isEmpty())
    {
         m_db.setUserName(sUserName);//用户名
    }

    if(!sPassword.isEmpty())
    {
         m_db.setPassword(sPassword);//密码
    }

    m_db.setDatabaseName(dsn);//设置连接字符串  sql sever中不是直接连接数据库名称 而是DSN

    m_bOpendb = m_db.open();
    return m_bOpendb;
}

//返回查询后的结果 结果以stringlist形式返回
QStringList QDatabaseProcess::QueryTable(QString sSql)
{
    QString str;
    QStringList datalist;
    if(m_bOpendb == false)
    {
        return datalist;
    }

    QSqlQuery sqlQuery(m_db);
    bool bOk = sqlQuery.exec(sSql);
    if(bOk == false)
    {
       return datalist;
    }


    while(sqlQuery.next())
    {
        str = "";
        for(int i = 0;i < sqlQuery.record().count(); i++)
        {
             str = str + sqlQuery.value(i).toString();
             if(i != sqlQuery.record().count() - 1)
             {
                 str = str + "|";
             }
        }
        datalist.push_back(str);
     }

    return datalist;
}



//插入数据  sFieldName 以逗号隔开 sValue 以逗号隔开
bool QDatabaseProcess::InsertDataToTable(QString sTbaleName,QString sFieldName,QString sValue)
{
    QString sSql;
    QString sSqlValue;
    if(m_bOpendb == false)
    {
        return false;
    }

    QStringList filedlist = sFieldName.split(",");
    QStringList valuelist = sValue.split(",");

    if(filedlist.size() != valuelist.size())
    {
        return false;
    }

    QSqlTableModel model;
    model.setTable(sTbaleName);

    QSqlRecord record = model.record();

    for(int i = 0; i < filedlist.size();i++)
    {
        record.setValue(filedlist[i],valuelist[i]);
    }

    bool brel = model.insertRecord(-1,record);

    return brel;
}

//更新数据
bool QDatabaseProcess::UpdateItem(QString sTbaleName,QString sSql)
{

    if(m_bOpendb == false)
    {
        return false;
    }

    QSqlQuery sqlQuery(m_db);

    bool bOk = sqlQuery.exec(sSql);

    return bOk;


}
