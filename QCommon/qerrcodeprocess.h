/***************************************************************************
**                                                                        **
** 文件描述：错误信息处理
**                                                                        **
****************************************************************************
** 创建人：李刚
** 创建时间：2018-08-01
** 修改记录：
**
****************************************************************************/
#ifndef QErrCodeProcess_H
#define QErrCodeProcess_H

#include <QObject>
#include <QHash>

//错误码
enum ERR_CODE {
    M_ERRCODE_INSERTDB=1,//插入数据库失败
    M_ERRCODE_UPDATEDB,//更新数据库失败
};



class QErrCodeProcess : public QObject
{
    Q_OBJECT
public:
    explicit QErrCodeProcess(QObject *parent = 0);

signals:

public slots:

public:
     void showErrCode(int nerrCode);


private:
     void initErrCode();
    QHash<int,QString>_tErrCodeMap;

};

#endif // QErrCodeProcess_H
