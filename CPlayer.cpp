/**
 * \file CPlayer.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2015-2016 Thorsten Roth <elthoro@gmx.de>
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
 * Player.
 */

#include <QDebug>
#include <QMessageBox>

#include "./CPlayer.h"

CPlayer::CPlayer(bool bActive, bool bIsHuman, QString sName, quint8 nMaxStones)
    : m_bIsActive(bActive),
      m_bIsHuman(bIsHuman),
      m_sName(sName),
      m_nMaxStones(nMaxStones),
      m_nStonesLeft(nMaxStones),
      m_nWonTowers(0) {
    qDebug() << "Generated player" << m_sName;
}

CPlayer::~CPlayer() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void CPlayer::setActive(bool bActive) {
    m_bIsActive = bActive;
}

bool CPlayer::getIsActive() const {
    return m_bIsActive;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool CPlayer::getIsHuman() const {
    return m_bIsHuman;
}

QString CPlayer::getName() const {
    return m_sName;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void CPlayer::setStonesLeft(quint8 nStones) {
    if (nStones <= m_nMaxStones) {
        m_nStonesLeft = nStones;
    } else {
        m_nStonesLeft = m_nMaxStones;
        qWarning() << "Stones > MaxStones!" << nStones << ">" << m_nMaxStones;
        QMessageBox::warning(NULL, "Warning", "Something went wrong!");
    }
}

quint8 CPlayer::getStonesLeft() const {
    return m_nStonesLeft;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void CPlayer::increaseWonTowers() {
    m_nWonTowers++;
}

quint8 CPlayer::getWonTowers() const {
    return m_nWonTowers;
}