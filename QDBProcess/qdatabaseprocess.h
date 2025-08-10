#ifndef DATABASEPROCESS_H
#define DATABASEPROCESS_H
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqldatabase.h>

enum DataBaseType
{
    M_DATABASE_ACCESS = 1,
    M_DATABASE_SQLSERVER = 2
};


class QDatabaseProcess
{
public:
    QDatabaseProcess();
    ~QDatabaseProcess();

public:
    bool connectDatabase(QString sDataBaseName,int nDatabaseType,QString sServerName="",QString sUserName="",QString sPassword="");
    QStringList QueryTable(QString sSql);
    bool InsertDataToTable(QString sTbaleName,QString sFieldName,QString sValue);
    bool UpdateItem(QString sTbaleName,QString sSql);

public:
    QSqlDatabase m_db;
private:


    bool m_bOpendb;
};

#endif // DATABASEPROCESS_H
