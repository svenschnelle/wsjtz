#ifndef RXTABLE_H
#define RXTABLE_H

#include <QWidget>
#include <QMenu>
#include <QTableWidget>
#include <QDateTime>
#include "Decoder/decodedtext.h"
#include "logbook/logbook.h"
#include "Configuration.hpp"

class DecodedText;
class Configuration;
class LogBook;

class RxTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit RxTable(QWidget *parent = nullptr);
    void set_configuration (Configuration const * configuration)
    {
      m_config = configuration;
    }
    void addRow(DecodedText dt, LogBook const& logBook, QString const& currentBand, QString const& mode, bool bold = false, bool CQOnly = false, bool incl73 = false, bool isFiltered = false, QString state ="") ;
    QString getDXCCandColor (QString message, QString call, QString const& grid,
                            QColor * bg, QColor * fg, LogBook const& logBook,
                            QString const& currentBand, QString const& currentMode, bool incl73);
    void init(int x);
    void addSeparator(QString s = "");
    void addTxRow(QString t, int txFreq);

    Q_SIGNAL void ctrlClicked (QString message) const;
    Q_SIGNAL void messageClicked (QString message) const;
    Q_SIGNAL void clicked (int paneId) const;


private:
    QString leftJustifyAppendage(QString message, QString appendage);
    QString getDistance(QString grid);
    void setItemAttributes(QTableWidgetItem * i, QString message, QColor bg, QColor fg, int alignment = Qt::AlignLeft, bool bold = false);
    bool separatorLast;
    Configuration const * m_config;
    int paneId;
    bool isForUs(DecodedText decodedText);
    bool isCQ(DecodedText decodedText, bool incl73 = false);
    bool isCQ(QString message, bool incl73);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void insertRow(int x);


signals:

};

#endif // RXTABLE_H

