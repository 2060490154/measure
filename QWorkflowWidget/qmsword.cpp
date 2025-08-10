#include "qmsword.h"
#include <QDebug>
QMSWord::QMSWord(QObject *parent) : QObject(parent) {
    m_bOpened = false;
    m_WordDoc = NULL;
    m_WordWidget = NULL;
}



QMSWord::QMSWord(const QString &strFilePath, QObject *parent) {
    m_bOpened = false;
    m_sFilePath = strFilePath;
    m_WordDoc = NULL;
    m_WordWidget = NULL;
}


QMSWord::~QMSWord() {
    close();
}


/******************************
*功能:open
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::open(bool bVisible) {
    //New a word application
    m_WordWidget = new QAxObject();
    if (m_WordWidget->setControl("word.Application")) {
        m_WordWidget->setProperty("Visible",bVisible);
    } else {
        return false;
    }

    //Fetch the pointer of the Documents object
    QAxObject *document = m_WordWidget->querySubObject("Documents");
    if (!document) {
        return false;
    }

    //new a doc based on the .dot file
    document->dynamicCall("Add(QString)", m_sFilePath);

    //Fetch the pointer of the active document
    m_WordDoc = m_WordWidget->querySubObject("ActiveDocument");
    m_bOpened = true;
    return m_bOpened;
}



/******************************
*功能:open override
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::open(const QString &strFile, bool bVisible) {
    m_sFilePath = strFile;
    close();
    return (open(bVisible));
}


/******************************
*功能:close
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::close() {
    if (m_bOpened) {
        if (m_WordDoc) {
            m_WordDoc->dynamicCall("Close(bool)",true);
            delete m_WordDoc;
            m_WordDoc = NULL;
        }
        if (m_WordWidget) {
            m_WordWidget->dynamicCall("Quit()");
            delete m_WordWidget;
            m_WordWidget = NULL;
        }
        m_bOpened = false;
    }
    return m_bOpened;
}


/******************************
*功能:IsOpen
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::isOpen() {
    return m_bOpened;
}


/******************************
*功能:save
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::save() {
    return m_WordDoc->dynamicCall("Save()").toBool();
}


/******************************
*功能:saveAs
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::saveAs(const QString &strFilePath) {
    return m_WordDoc->dynamicCall("SaveAs(const QString&)", strFilePath).toBool();
}


/******************************
*功能:setMarkContent
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::setMarkContent(const QString &strMark, const QString &strContent) {
    QAxObject *bookMark = m_WordDoc->querySubObject("BookMarks(Variant)", strMark);
    //select the bookmark and set content into it
    if (bookMark) {
        bookMark->dynamicCall("Select(void)");
        bookMark->querySubObject("Range")->setProperty("Text", strContent);
        return true;
    } else {
        return false;
    }
}



/******************************
*功能:插入表格(暂时无法实现Range定位)
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QMSWord::insertTable(int nStart, int nEnd, int nRow, int nCol) {
    QAxObject *Position = m_WordDoc->querySubObject("Range(long, long)", nStart, nEnd);
    QAxObject *Table = m_WordDoc->querySubObject("Tables");
    QVariantList Paras;
    Paras.append(Position->asVariant());
    Paras.append(nRow);
    Paras.append(nCol);
    if (Table) {
        Table->dynamicCall("Add(QAxObject*, long, long)",Paras);
        return true;
    }
}


/******************************
*功能:标签处插入表格
*输入:nLineStyle: 1->solid line; 2->dot line; 3->dash line
*输出:
*返回值:
*修改记录:无
*******************************/
QAxObject* QMSWord::insertTable(QString strMark, int nRow, int nCol, bool bVisible, int nLineStyle) {
    QAxObject *bookMark = m_WordDoc->querySubObject("BookMarks(Variant)",strMark);
    if (bookMark) {
        bookMark->dynamicCall("Select(void)");
        QAxObject *Selection = m_WordWidget->querySubObject("Selection");
        QAxObject *Range = Selection->querySubObject("Range");
        QAxObject *Tables = m_WordDoc->querySubObject("Tables");
        QAxObject *Table = Tables->querySubObject("Add(QVariant, int, int)",Range->asVariant(),nRow, nCol);
        if (bVisible) {
            for (int i=0; i<6; i++) {
                QString str = QString("Borders(-%1)").arg(i+1);
                QAxObject *Borders = Table->querySubObject(str.toLocal8Bit().constData());
                Borders->dynamicCall("SetLineStyle(int)",nLineStyle);
            }
        }

        return Table;
    } else {
        return NULL;
    }
}

/******************************
*功能:给表格增加行(在第nRow行下方插入：nType = 0；在第nRow行上方插入：nType = 1；)
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QMSWord::addTableRow(QAxObject *Table, int nRow, int nRowCount, int nType) {
    QAxObject *Rows = Table->querySubObject("Rows");
    int Count = Rows->dynamicCall("Count").toInt();
    if (nRow >= 0 && nRow <= Count) {
        for (int i=0; i<nRowCount; i++) {
            QString strPos = QString("Item(%1)").arg(nRow + i);
            QAxObject *Curntrow = Rows->querySubObject(strPos.toStdString().c_str());
            Curntrow->dynamicCall("Select(void)");
            QAxObject *Selection = m_WordWidget->querySubObject("Selection");
            if (nType == 0) {
                Selection->dynamicCall("InsertRowsBelow(int)",1);
            } else {
                Selection->dynamicCall("InsertRowsAbove(int)",1);
            }

        }
    }
}



/******************************
*功能:在文档已有表格中的标签处增加行
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QMSWord::addTableRow(QString strMark, int nRowCount, int nType) {
    QAxObject *bookMark = m_WordDoc->querySubObject("BookMarks(Variant)",strMark);
    if (bookMark) {
        bookMark->dynamicCall("Select(void)");
        QAxObject *Selection = m_WordWidget->querySubObject("Selection");

        for (int i=0; i<nRowCount; i++) {
            if (nType == 0) {
                Selection->dynamicCall("InsertRowsBelow(int)",1);
            } else {
                Selection->dynamicCall("InsertRowsAbove(int)",1);
            }
        }
    }
}


/******************************
*功能:设置表格单元内容（表格对象由新建表格函数返回）
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QMSWord::setCellText(QAxObject *Table, int nRow, int nCol, const QString &strText) {
    if (Table) {
        Table->querySubObject("Cell(int, int)", nRow, nCol)->querySubObject("Range")
                ->dynamicCall("SetText(QString)",strText);
    }
}


/******************************
*功能:设置表格单元内容（根据表格在模板中的index，几是全文第几个表格）
*输入:
*输出:
*返回值:
*修改记录:需要对nRow和col合法性进行判断
*******************************/
void QMSWord::setCellText(int TableIndex, int nRow, int nCol, const QString &strText) {
    QAxObject *ActiveDocument = m_WordWidget->querySubObject("ActiveDocument");
    QAxObject *Table = ActiveDocument->querySubObject("Tables(int)",TableIndex);
    if (Table) {
        Table->querySubObject("Cell(int, int)", nRow, nCol)->querySubObject("Range")
                ->dynamicCall("SetText(QString)",strText);
    }
}

/******************************
*功能:插入一幅图片
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QMSWord::insertPic(QString strMark, const QString &picPath) {
    QAxObject *bookMark = m_WordDoc->querySubObject("BookMarks(QVariant)",strMark);
    if (bookMark) {
        bookMark->dynamicCall("Select(void)");
        QAxObject *Selection = m_WordWidget->querySubObject("Selection");
        Selection->querySubObject("ParagraphFormat")->dynamicCall("Alignment","wdAlignParagraphCenter");
        QAxObject *Range = bookMark->querySubObject("Range");

        //para
        QList<QVariant> para;
        //参数依次是图片路径，LinkToFile判断，SaveWithFile判断，range对象
        para<<QVariant(picPath)<<QVariant(false)<<QVariant(true)<<(Range->asVariant());
        QAxObject *InlineShapes = m_WordDoc->querySubObject("InlineShapes");
        InlineShapes->dynamicCall("AddPicture(const QString&, QVariant, QVariant, QVariant)",para);
    }
}
