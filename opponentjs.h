/**
 * \file opponentjs.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2015-2018 Thorsten Roth <elthoro@gmx.de>
 *
 * This file is part of StackAndConquer.
 *
 * StackAndConquer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StackAndConquer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StackAndConquer.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Interface to CPU script JS engine.
 */

#ifndef OPPONENTJS_H_
#define OPPONENTJS_H_

#include <QObject>
#include <QPoint>
#include <QJSEngine>

class OpponentJS : public QObject {
  Q_OBJECT

  public:
    explicit OpponentJS(const quint8 nID, const quint8 nNumOfFields,
                        const quint8 nHeightTowerWin, QObject *parent = 0);
    bool loadAndEvalCpuScript(const QString &sFilepath);

  public slots:
    void makeMoveCpu(const QList<QList<QList<quint8> > > board,
                     const quint8 nPossibleMove);
    void log(const QString &sMsg) const;

  signals:
    void setStone(QPoint field);
    void moveTower(QPoint tower, QPoint moveTo, quint8 nStones = 0);
    void scriptError();

  private:
    QJsonDocument convertBoardToJSON(const QList<QList<QList<quint8> > > board);
    QList<QPoint> evalMoveReturn(QString sReturn);

    const quint8 m_nID;
    const quint8 m_nNumOfFields;
    const quint8 m_nHeightTowerWin;
    QJSEngine *m_jsEngine;
    QJSValue m_obj;
    QList<QList<QList<quint8> > > m_board;
};

#endif  // OPPONENTJS_H_
