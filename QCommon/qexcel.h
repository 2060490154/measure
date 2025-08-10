#ifndef QEXCEL_H
#define QEXCEL_H

class QAxObject;
class QString;

class QExcel
{
public:
    QExcel();
    ~QExcel();
public:
    void insertContent(int nrow, int ncol, QString sContent, QString sDataType);
    void initSheet();
    void saveSheet(QString sPath);
    void closeSheet();
    void setSheetName(QString sName);
    QString transColumn(int nCol);
public:
    QAxObject *m_pExcel;
    QAxObject *m_pActiveSheet;
    QAxObject *m_pWorkbook;
};

#endif // QEXCEL_H
