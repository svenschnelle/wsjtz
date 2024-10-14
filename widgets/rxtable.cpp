#include "rxtable.h"

#include <QTableWidget>
#include <QHeaderView>
#include "Decoder/decodedtext.h"
#include "models/DecodeHighlightingModel.hpp"
#include "Network/LotWUsers.hpp"
#include <QMouseEvent>
#include <QMenu>

RxTable::RxTable(QWidget *parent)
    : QTableWidget(parent)
  , m_config {nullptr}
{

    verticalHeader()->hide();
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void RxTable::init(int x) {
    paneId = x;
    setFont(m_config->decoded_text_font());
    setSortingEnabled(false);
    QFont f = m_config->decoded_text_font();
    QFontMetrics fm(f);
    verticalHeader()->setMaximumSectionSize(fm.height()+2);
    setStyleSheet("QTableWidget::item:hover {  }");
    setFocusPolicy(Qt::NoFocus);

    QVector<bool> cols;
    if (x==0) {
        cols = m_config->columnsLeft();
    } else {
        cols = m_config->columnsRight();
    }

    QVectorIterator<bool> colIter(cols);

    for (int c = 0; c< cols.size(); c++) {
        setColumnHidden(c, !cols[c]);
    }
}

extern "C" {
void azdist_(char* MyGrid, char* HisGrid, double* utch, int* nAz, int* nEl,
             int* nDmiles, int* nDkm, int* nHotAz, int* nHotABetter,
             fortran_charlen_t, fortran_charlen_t);
}

namespace
{
  using Highlight = DecodeHighlightingModel::Highlight;
  using highlight_types = std::vector<Highlight>;
    Highlight set_colours (Configuration const * config, QColor * bg, QColor * fg, highlight_types const& types)
    {
      Highlight result = Highlight::CQ;
      if (config)
        {
          QListIterator<DecodeHighlightingModel::HighlightInfo> it {config->decode_highlighting ().items ()};
          // iterate in reverse to honor priorities
          it.toBack ();
          while (it.hasPrevious ())
            {
              auto const& item = it.previous ();
              auto const& type = std::find (types.begin (), types.end (), item.type_);
              if (type != types.end () && item.enabled_)
                {
                  if (item.background_.style () != Qt::NoBrush)
                    {
                      *bg = item.background_.color ();
                    }
                  if (item.foreground_.style () != Qt::NoBrush)
                    {
                      *fg = item.foreground_.color ();
                    }
                  result = item.type_;
                }
            }
        }
      return result;            // highest priority enabled highlighting
    }
}

void RxTable::setItemAttributes(QTableWidgetItem * i, QString message, QColor bg, QColor fg, int alignment, bool bold) {
    i->setToolTip(message);
    if (bg.isValid()) i->setBackground(QBrush(bg));
    if (fg.isValid()) i->setForeground(QBrush(fg));
    i->setTextAlignment(alignment);
    QFont f = m_config->decoded_text_font();
    if (bold) {
        f.setBold(true);
    }
    i->setFont(f);
}

void RxTable::addSeparator(QString s) {
    if (separatorLast) return;
    insertRow(rowCount());
    QColor bg(m_config->separatorColor());
    QFont f = m_config->decoded_text_font();
    f.setBold(true);
    QTableWidgetItem * i = new QTableWidgetItem(s);
    i->setFont(f);
    i->setTextAlignment(Qt::AlignCenter);
    if (bg.isValid()) i->setBackground(QBrush(bg));
    setSpan(rowCount()-1, 0, 1, columnCount());
    setItem(rowCount()-1, 0, i);
    scrollToBottom();

    separatorLast=true;
}

void RxTable::addTxRow(QString t, int txFreq) {
    QColor bg;
    QColor fg;
    highlight_types types {Highlight::Tx};
    set_colours (m_config, &bg, &fg, types);
    insertRow(rowCount());


    for (int c = 0; c < 7; c++) {
        QString content;
        if (c==3) content = QString::number(txFreq);
        QTableWidgetItem * i = new QTableWidgetItem(content);
        setItemAttributes(i, t, bg, fg, Qt::AlignLeft);
        if (bg.isValid()) i->setBackground(QBrush(bg));
        setItem(rowCount()-1, c, i);
    }

    QTableWidgetItem * i = new QTableWidgetItem(" " + t);
    setItemAttributes(i, t, bg, fg, Qt::AlignLeft);
    setItem(rowCount()-1, 6, i);

    i = new QTableWidgetItem(QDateTime::currentDateTimeUtc().toString("hhmmss"));
    setItemAttributes(i, t, bg, fg, Qt::AlignLeft);
    setItem(rowCount()-1, 0, i);

    scrollToBottom();

}

bool RxTable::isCQ(QString message, bool incl73) {
    DecodedText dt{message};
    return isCQ(dt, incl73);
}

bool RxTable::isCQ(DecodedText decodedText, bool incl73) {
    auto is_73 = decodedText.messageWords().filter (QRegularExpression {"^(73|RR73)$"}).size();
    return (decodedText.string ().contains (" CQ ")
        || decodedText.string ().contains (" CQDX ")
        || decodedText.string ().contains (" QRZ ")
        || (incl73 && is_73));
}

bool RxTable::isForUs(DecodedText decodedText) {
    QString myCall = m_config->my_callsign();
    return (myCall != "" && (decodedText.indexOf (" " + myCall + " ") >= 0
                          or decodedText.indexOf (" " + myCall + "/") >= 0
                          or decodedText.indexOf ("<" + myCall + "/") >= 0
                          or decodedText.indexOf ("/" + myCall + " ") >= 0
                          or decodedText.indexOf ("/" + myCall + ">") >= 0
                          or decodedText.indexOf ("<" + myCall + " ") >= 0
                          or decodedText.indexOf ("<" + myCall + ">") >= 0
                          or decodedText.indexOf (" " + myCall + ">") >= 0));
}

void RxTable::addRow(DecodedText dt, LogBook const& logBook, QString const& currentBand, QString const& mode, bool bold, bool CQOnly, bool incl73, bool isFiltered, QString state) {
    if (CQOnly && !isCQ(dt, incl73) && !isForUs(dt)) return;

    separatorLast = false;
    QString message = dt.clean_string();
    QString call,grid;
    QString style = styleSheet();
    dt.deCallAndGrid(call, grid);
    QColor bg;
    QColor fg;



    QString dxcc = getDXCCandColor(message, call, grid, &bg, &fg
                              , logBook, currentBand, mode, incl73);

    insertRow(rowCount());
    QTableWidgetItem * i = new QTableWidgetItem(dt.clean_string().mid(0, 6));
    setItemAttributes(i, message, bg, fg, Qt::AlignLeft, false);
    setItem(rowCount()-1, 0, i);

    i = new QTableWidgetItem(" " + QString::number(dt.snr()));
    setItemAttributes(i, message, bg, fg, Qt::AlignRight, false);
    setItem(rowCount()-1, 1, i);

    i = new QTableWidgetItem(" " + QString::number(dt.dt()));
    setItemAttributes(i, message, bg, fg, Qt::AlignRight, false);
    setItem(rowCount()-1, 2, i);

    i = new QTableWidgetItem(" " + QString::number(dt.frequencyOffset()));
    setItemAttributes(i, message, bg, fg, Qt::AlignRight, false);
    setItem(rowCount()-1, 3, i);

    i = new QTableWidgetItem(" " + dxcc);
    setItemAttributes(i, message, bg, fg, Qt::AlignLeft, false);
    setItem(rowCount()-1, 4, i);


    i = new QTableWidgetItem(" " + getDistance(grid));
    setItemAttributes(i, message, bg, fg, Qt::AlignLeft, false);
    setItem(rowCount()-1, 5, i);

    auto msg = " " + message.mid(23).trimmed();
    QString appendage;
    if (state.length() > 0) {
        appendage = " [" + state + "]";
    }

    if (isFiltered) appendage += " [F]";

    if (appendage.length() > 0) msg = msg.leftJustified(22 , ' ') + appendage;

    if (bold) {
        bg.setRgb(255, 0, 0);
        fg.setRgb(255, 255, 255);
    }

    i = new QTableWidgetItem(msg);
    setItemAttributes(i, message, bg, fg, Qt::AlignLeft, false);
    setItem(rowCount()-1, 6, i);

    //resizeRowsToContents();
    scrollToBottom();
}

void RxTable::insertRow(int x) {
    if (rowCount() > m_config->maxRows()) {
        removeRow(0);
        setRowCount(m_config->maxRows());
        x = rowCount();
    }

    QTableWidget::insertRow(x);
}



QString RxTable::getDistance(QString grid) {
    if (grid.length() != 4 || grid == "RR73" || grid.startsWith("R-") || grid.startsWith("R+")) return "";

    QString distance;

    double utch=0.0;
    int nAz,nEl,nDmiles,nDkm,nHotAz,nHotABetter;
    azdist_(const_cast <char *> ((m_config->my_grid () + "      ").left (6).toLatin1().constData()),
            const_cast <char *> ((grid + "      ").left (6).toLatin1().constData()),&utch,
            &nAz,&nEl,&nDmiles,&nDkm,&nHotAz,&nHotABetter,6,6);

    if (m_config->showDistance()) {
        int nd=nDkm;
        if(m_config->miles()) nd=nDmiles;
        distance = QString::number(nd);
        if(m_config->miles())
            distance += " mi";
        else
            distance += " km";
    }

    if (m_config->showBearing()) {
        if (distance.length()) distance += " / ";
        distance += QString::number(nAz) + "°";
    }

    return distance;
}

QString RxTable::getDXCCandColor (QString message, QString call, QString const& grid,
                                     QColor * bg, QColor * fg, LogBook const& logBook,
                                     QString const& currentBand, QString const& currentMode,
                                    bool incl73)
{
      QString countryName;
      bool callB4;
      bool callB4onBand;
      bool countryB4;
      bool countryB4onBand;
      bool gridB4;
      bool gridB4onBand;
      bool continentB4;
      bool continentB4onBand;
      bool CQZoneB4;
      bool CQZoneB4onBand;
      bool ITUZoneB4;
      bool ITUZoneB4onBand;

      if(call.length()==2) {
        int i0=message.indexOf("CQ "+call);
        call=message.mid(i0+6,-1);
        i0=call.indexOf(" ");
        call=call.mid(0,i0);
      }
      if(call.length()<3) return "";
      if(!call.contains(QRegExp("[0-9]|[A-Z]"))) return "";

      QString lookupGrid;
      if (message.contains (" CQ ")) {
          lookupGrid = grid;
      }

      auto const& looked_up = logBook.countries ()->lookup (call);
      logBook.match (call, currentMode, lookupGrid, looked_up, callB4, countryB4, gridB4, continentB4, CQZoneB4, ITUZoneB4);
      logBook.match (call, currentMode, lookupGrid, looked_up, callB4onBand, countryB4onBand, gridB4onBand,
                     continentB4onBand, CQZoneB4onBand, ITUZoneB4onBand, currentBand);
      if(grid=="") {
        gridB4=true;
        gridB4onBand=true;
      }

      message = message.trimmed ();

      highlight_types types;
      // no shortcuts here as some types may be disabled
      if (!countryB4) {
        types.push_back (Highlight::DXCC);
      }
      if(!countryB4onBand) {
        types.push_back (Highlight::DXCCBand);
      }
      if(!gridB4) {
        types.push_back (Highlight::Grid);
      }
      if(!gridB4onBand) {
        types.push_back (Highlight::GridBand);
      }
      if (!callB4) {
        types.push_back (Highlight::Call);
      }
      if(!callB4onBand) {
        types.push_back (Highlight::CallBand);
      }
      if (!continentB4) {
        types.push_back (Highlight::Continent);
      }
      if(!continentB4onBand) {
        types.push_back (Highlight::ContinentBand);
      }
      if (!CQZoneB4) {
        types.push_back (Highlight::CQZone);
      }
      if(!CQZoneB4onBand) {
        types.push_back (Highlight::CQZoneBand);
      }
      if (!ITUZoneB4) {
        types.push_back (Highlight::ITUZone);
      }
      if(!ITUZoneB4onBand) {
        types.push_back (Highlight::ITUZoneBand);
      }
      if (m_config && m_config->lotw_users().user (call))
        {
          types.push_back (Highlight::LotW);
        }

      if (isCQ(message, incl73)) types.push_back (Highlight::CQ);

      if (message.contains(m_config->my_callsign())) {
         types.push_back (Highlight::MyCall);
       }

      set_colours (m_config, bg, fg, types);

      countryName = looked_up.entity_name;

      // do some obvious abbreviations
      countryName.replace ("Islands", "Is.");
      countryName.replace ("Island", "Is.");
      countryName.replace ("North ", "N. ");
      countryName.replace ("Northern ", "N. ");
      countryName.replace ("South ", "S. ");
      countryName.replace ("East ", "E. ");
      countryName.replace ("Eastern ", "E. ");
      countryName.replace ("West ", "W. ");
      countryName.replace ("Western ", "W. ");
      countryName.replace ("Central ", "C. ");
      countryName.replace (" and ", " & ");
      countryName.replace ("Republic", "Rep.");
      countryName.replace ("United States of America", "U.S.A.");
      countryName.replace ("United States", "U.S.A.");
      countryName.replace ("Fed. Rep. of ", "");
      countryName.replace ("French ", "Fr.");
      countryName.replace ("Asiatic", "AS");
      countryName.replace ("European", "EU");
      countryName.replace ("African", "AF");

      return countryName;

}

void RxTable::mousePressEvent(QMouseEvent *event) {
    Q_EMIT clicked(paneId);

    if (Qt::LeftButton == event->button() && (event->modifiers() & Qt::ControlModifier)) {
    QTableWidgetItem * it = this->itemAt(event->pos());
        if (it != NULL) {
            QString message = it->toolTip();
            Q_EMIT ctrlClicked(message);
        }
    }

    QTableWidget::mousePressEvent(event);
}

void RxTable::mouseDoubleClickEvent(QMouseEvent *event) {
    QTableWidgetItem * it = this->itemAt(event->pos());
    if (it != NULL) {
        QString message = it->toolTip();
        Q_EMIT messageClicked(message);
    }
}

