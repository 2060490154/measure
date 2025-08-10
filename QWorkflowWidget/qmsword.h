#ifndef QMSWORD_H
#define QMSWORD_H

#include <QObject>
#include <QAxWidget>
#include <QAxObject>

class QMSWord : public QObject
{
    Q_OBJECT
public:
    explicit QMSWord(QObject *parent = 0);
    QMSWord(const QString &strFilePath, QObject *parent = 0);
    ~QMSWord();

public:
    bool open(bool bVisible = false);
    bool open(const QString &strFile, bool bVisible = false);
    bool close();
    bool isOpen();
    bool save();
    bool saveAs(const QString &strFilePath);

public:
    bool setMarkContent(const QString &strMark, const QString &strContent);
    bool insertTable(int nStart, int nEnd, int nRow, int nCol);
    QAxObject *insertTable(QString strMark, int nRow, int nCol, bool bVisible = true, int nLineStyle = 1);
    void addTableRow(QAxObject *Table, int nRow, int nRowCount, int nType = 0);
    void addTableRow(QString strMark, int nRowCount, int nType = 0);
    void setCellText(QAxObject *Table, int nRow, int nCol, const QString &strText);
    void setCellText(int TableIndex, int nRow, int nCol, const QString &strText);
    void insertPic(QString strMark, const QString &picPath);
signals:

public slots:

private:
    bool m_bOpened;
    QString m_sFilePath;
    QAxObject *m_WordDoc;
    QAxObject *m_WordWidget;
};

#endif // QMSWORD_H
