/**
 * \file settings.cpp
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
 * Settings dialog.
 */

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QIcon>
#include <QMessageBox>

#include "./settings.h"
#include "ui_settings.h"

Settings::Settings(const QString &sSharePath, const QString &userDataDir,
                   QWidget *pParent)
  : QDialog(pParent),
    m_pUi(new Ui::SettingsDialog()),
    m_sSharePath(sSharePath) {
  m_pUi->setupUi(this);
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  this->setModal(true);

#if defined _WIN32
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  m_pUi->cbGuiLanguage->addItems(this->searchTranslations());
  this->searchCpuScripts(userDataDir);

  QStringList sListStartPlayer;
  sListStartPlayer << trUtf8("Random")
                   << trUtf8("Player 1")
                   << trUtf8("Player 2");
  m_pUi->cbStartPlayer->addItems(sListStartPlayer);

  connect(m_pUi->buttonBox, SIGNAL(accepted()),
          this, SLOT(accept()));
  connect(m_pUi->buttonBox, SIGNAL(rejected()),
          this, SLOT(reject()));

  this->readSettings();
}

Settings::~Settings() {
  if (m_pUi) {
    delete m_pUi;
    m_pUi = NULL;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QStringList Settings::searchTranslations() const {
  QStringList sList;

  // Translations build in resources
  QDirIterator it(":", QStringList() << "*.qm",
                  QDir::NoDotAndDotDot | QDir::Files);
  while (it.hasNext()) {
    it.next();
    QString sTmp = it.fileName();
    // qDebug() << sTmp;

    if (sTmp.startsWith(qApp->applicationName().toLower() + "_") &&
        sTmp.endsWith(".qm")) {
      sList << sTmp.remove(
                 qApp->applicationName().toLower() + "_").remove(".qm");
    }
  }

  // Check for additional translation files in share folder
  QDirIterator it2(m_sSharePath + "/lang", QStringList() << "*.qm",
                   QDir::NoDotAndDotDot | QDir::Files);
  while (it2.hasNext()) {
    it2.next();
    QString sTmp = it2.fileName();
    // qDebug() << sTmp;

    if (sTmp.startsWith(qApp->applicationName().toLower() + "_")) {
      sTmp = sTmp.remove(
               qApp->applicationName().toLower() + "_") .remove(".qm");
      if (!sList.contains(sTmp)) {
        sList << sTmp;
      }
    }
  }

  sList << "en";
  sList.sort();
  sList.push_front("auto");
  return sList;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::searchCpuScripts(const QString &userDataDir) {
  QStringList sListAvailableCpu;
  sListAvailableCpu << "Human";
  m_sListCPUs.clear();
  m_sListCPUs << "Human";
  QDir cpuDir = m_sSharePath;

  // Cpu scripts in share folder
  if (cpuDir.cd("cpu")) {
    foreach (QFileInfo file, cpuDir.entryInfoList(QDir::Files)) {
      if ("js" == file.suffix().toLower()) {
        sListAvailableCpu << file.baseName();
        m_sListCPUs << file.absoluteFilePath();
      }
    }
  }
  m_pUi->cbP1HumanCpu->addItems(sListAvailableCpu);
  m_pUi->cbP2HumanCpu->addItems(sListAvailableCpu);

  // Cpu scripts in user folder
  cpuDir = userDataDir;
  if (cpuDir.cd("cpu")) {
    foreach (QFileInfo file, cpuDir.entryInfoList(QDir::Files)) {
      if ("js" == file.suffix().toLower()) {
        sListAvailableCpu << file.baseName();
        m_pUi->cbP1HumanCpu->addItem(QIcon(":/images/user.png"),
                                     sListAvailableCpu.last());
        m_pUi->cbP2HumanCpu->addItem(QIcon(":/images/user.png"),
                                     sListAvailableCpu.last());
        m_sListCPUs << file.absoluteFilePath();
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::accept() {
  QString sOldGuiLang = m_sGuiLanguage;
  m_sGuiLanguage = m_pUi->cbGuiLanguage->currentText();
  m_pSettings->setValue("GuiLanguage", m_sGuiLanguage);

  if (sOldGuiLang != m_sGuiLanguage) {
    emit changeLang(this->getLanguage());
  }

  m_sNameP1 = m_pUi->leNameP1->text();
  if (m_sNameP1.trimmed().isEmpty()) {
    m_sNameP1 = trUtf8("Player 1");
    m_pUi->leNameP1->setText(m_sNameP1);
  }
  m_pSettings->setValue("NameP1", m_sNameP1);
  m_sNameP2 = m_pUi->leNameP2->text();
  if (m_sNameP2.trimmed().isEmpty()) {
    m_sNameP2 = trUtf8("Player 2");
    m_pUi->leNameP2->setText(m_sNameP2);
  }
  m_pSettings->setValue("NameP2", m_sNameP2);

  m_nStartPlayer = m_pUi->cbStartPlayer->currentIndex();
  m_pSettings->setValue("StartPlayer", m_nStartPlayer);

  m_bShowPossibleMoveTowers = m_pUi->checkShowPossibleMoves->isChecked();
  m_pSettings->setValue("ShowPossibleMoveTowers", m_bShowPossibleMoveTowers);

  m_pSettings->beginGroup("Colors");
  m_pSettings->setValue("BgColor", m_bgColor.name());
  m_pSettings->setValue("HighlightColor", m_highlightColor.name());
  m_pSettings->setValue("HighlightBorderColor", m_highlightBorderColor.name());
  m_pSettings->setValue("SelectedColor", m_selectedColor.name());
  m_pSettings->setValue("SelectedBorderColor", m_selectedBorderColor.name());
  m_pSettings->setValue("AnimateColor", m_animateColor.name());
  m_pSettings->setValue("AnimateBorderColor", m_animateBorderColor.name());
  m_pSettings->setValue("BgBoardColor", m_bgBoardColor.name());
  m_pSettings->setValue("OutlineBoardColor", m_outlineBoardColor.name());
  m_pSettings->setValue("GridBoardColor", m_gridBoardColor.name());
  m_pSettings->setValue("NeighboursColor", m_neighboursColor.name());
  m_pSettings->setValue("NeighboursBorderColor",
                        m_neighboursBorderColor.name());
  m_pSettings->endGroup();

  QString sNewP1HumanCpu(m_pUi->cbP1HumanCpu->currentText());
  QString sNewP2HumanCpu(m_pUi->cbP2HumanCpu->currentText());
  int nNewWinTowers(m_pUi->spinNumToWin->value());

  if (sNewP1HumanCpu != m_sP1HumanCpu ||
      sNewP2HumanCpu != m_sP2HumanCpu ||
      nNewWinTowers != m_nWinTowers) {
    int nRet = QMessageBox::question(
                 0, this->windowTitle(),
                 trUtf8("Main game settings had been changed.<br>"
                        "Do you want to start a new game?"));
    if (nRet == QMessageBox::Yes) {
      m_sP1HumanCpu = sNewP1HumanCpu;
      m_sP2HumanCpu = sNewP2HumanCpu;
      m_nWinTowers = nNewWinTowers;
      m_pSettings->setValue("P1HumanCpu", m_sP1HumanCpu);
      m_pSettings->setValue("P2HumanCpu", m_sP2HumanCpu);
      m_pSettings->setValue("NumWinTowers", m_nWinTowers);
      emit this->newGame();
    } else {
      this->readSettings();
      return;
    }
  }

  QDialog::accept();
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::reject() {
  this->readSettings();
  QDialog::reject();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::readSettings() {
  m_sGuiLanguage = m_pSettings->value("GuiLanguage", "auto").toString();
  if (-1 != m_pUi->cbGuiLanguage->findText(m_sGuiLanguage)) {
    m_pUi->cbGuiLanguage->setCurrentIndex(
          m_pUi->cbGuiLanguage->findText(m_sGuiLanguage));
  } else {
    m_pUi->cbGuiLanguage->setCurrentIndex(
          m_pUi->cbGuiLanguage->findText("auto"));
  }
  m_sGuiLanguage = m_pUi->cbGuiLanguage->currentText();

  m_sNameP1 = m_pSettings->value("NameP1", trUtf8("Player 1")).toString();
  m_pUi->leNameP1->setText(m_sNameP1);
  m_sNameP2 = m_pSettings->value("NameP2", trUtf8("Player 2")).toString();
  m_pUi->leNameP2->setText(m_sNameP2);

  m_sP1HumanCpu = m_pSettings->value("P1HumanCpu", "Human").toString();
  if (-1 != m_pUi->cbP1HumanCpu->findText(m_sP1HumanCpu)) {
    m_pUi->cbP1HumanCpu->setCurrentIndex(
          m_pUi->cbP1HumanCpu->findText(m_sP1HumanCpu));
  } else {
    m_pUi->cbP1HumanCpu->setCurrentIndex(
          m_pUi->cbP1HumanCpu->findText("Human"));
  }
  m_sP1HumanCpu = m_pUi->cbP1HumanCpu->currentText();

  m_sP2HumanCpu = m_pSettings->value("P2HumanCpu", "Human").toString();
  if (-1 != m_pUi->cbP2HumanCpu->findText(m_sP2HumanCpu)) {
    m_pUi->cbP2HumanCpu->setCurrentIndex(
          m_pUi->cbP2HumanCpu->findText(m_sP2HumanCpu));
  } else {
    m_pUi->cbP2HumanCpu->setCurrentIndex(
          m_pUi->cbP2HumanCpu->findText("Human"));
  }
  m_sP2HumanCpu = m_pUi->cbP2HumanCpu->currentText();

  m_nStartPlayer = m_pSettings->value("StartPlayer", 1).toUInt();
  if (m_nStartPlayer < m_pUi->cbStartPlayer->count()) {
    m_pUi->cbStartPlayer->setCurrentIndex(m_nStartPlayer);
  } else {
    m_pUi->cbStartPlayer->setCurrentIndex(1);
  }
  m_nStartPlayer = m_pUi->cbStartPlayer->currentIndex();

  m_nWinTowers = m_pSettings->value("NumWinTowers", 1).toUInt();
  m_pUi->spinNumToWin->setValue(m_nWinTowers);

  m_bShowPossibleMoveTowers = m_pSettings->value("ShowPossibleMoveTowers",
                                                 true).toBool();
  m_pUi->checkShowPossibleMoves->setChecked(m_bShowPossibleMoveTowers);

  m_bgColor = this->readColor("BgColor", "#EEEEEC");
  m_highlightColor = this->readColor("HighlightColor", "#8ae234");
  m_highlightBorderColor = this->readColor("HighlightBorderColor", "#888A85");
  m_selectedColor = this->readColor("SelectedColor", "#fce94f");
  m_selectedBorderColor = this->readColor("SelectedBorderColor", "#000000");
  m_animateColor = this->readColor("AnimateColor", "#fce94f");
  m_animateBorderColor = this->readColor("AnimateBorderColor", "#000000");
  m_bgBoardColor = this->readColor("BgBoardColor", "#FFFFFF");
  m_outlineBoardColor = this->readColor("OutlineBoardColor", "#2E3436");
  m_gridBoardColor = this->readColor("GridBoardColor", "#888A85");
  m_neighboursColor = this->readColor("NeighboursColor", "#ad7fa8");
  m_neighboursBorderColor = this->readColor("NeighboursBorderColor", "#000000");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

QColor Settings::readColor(const QString sKey, const QString sFallback) const {
  QString sValue = m_pSettings->value("Colors/" + sKey, sFallback).toString();
  QColor color(sFallback);

  color.setNamedColor(sValue);
  if (!color.isValid()) {
    color.setNamedColor(sFallback);
    qWarning() << "Found invalid color for key" << sKey;
  }
  return color;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::updateUiLang() {
  m_pUi->retranslateUi(this);

  QStringList sListStartPlayer;
  sListStartPlayer << trUtf8("Random")
                   << trUtf8("Player 1")
                   << trUtf8("Player 2");
  m_pUi->cbStartPlayer->clear();
  m_pUi->cbStartPlayer->addItems(sListStartPlayer);
  m_pUi->cbStartPlayer->setCurrentIndex(m_nStartPlayer);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Settings::getLanguage() {
  if ("auto" == m_sGuiLanguage) {
#ifdef Q_OS_UNIX
    QByteArray lang = qgetenv("LANG");
    if (!lang.isEmpty()) {
      return QLocale(lang).name();
    }
#endif
    return QLocale::system().name();
  } else if (!QFile(":/" + qApp->applicationName().toLower() +
                    "_" + m_sGuiLanguage + ".qm").exists() &&
             !QFile(m_sSharePath + "/lang/" +
                    qApp->applicationName().toLower() +
                    "_" + m_sGuiLanguage + ".qm").exists()) {
    m_sGuiLanguage = "en";
    m_pSettings->setValue("GuiLanguage", m_sGuiLanguage);
    return m_sGuiLanguage;
  }
  return m_sGuiLanguage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Settings::getNameP1() const {
  return m_sNameP1;
}
QString Settings::getNameP2() const {
  return m_sNameP2;
}
quint8 Settings::getStartPlayer() const {
  return m_nStartPlayer;
}
quint8 Settings::getWinTowers() const {
  return m_nWinTowers;
}
bool Settings::getShowPossibleMoveTowers() const {
  return m_bShowPossibleMoveTowers;
}

QString Settings::getP1HumanCpu() const {
  if (-1 != m_pUi->cbP1HumanCpu->findText(m_sP1HumanCpu)) {
    return m_sListCPUs[m_pUi->cbP1HumanCpu->findText(m_sP1HumanCpu)];
  } else {
    return "Human";
  }
}

QString Settings::getP2HumanCpu() const {
  if (-1 != m_pUi->cbP2HumanCpu->findText(m_sP2HumanCpu)) {
    return m_sListCPUs[m_pUi->cbP2HumanCpu->findText(m_sP2HumanCpu)];
  } else {
    return "Human";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QColor Settings::getBgColor() const {
  return m_bgColor;
}
QColor Settings::getHighlightColor() const {
  return m_highlightColor;
}
QColor Settings::getHighlightBorderColor() const {
  return m_highlightBorderColor;
}
QColor Settings::getSelectedColor() const {
  return m_selectedColor;
}
QColor Settings::getSelectedBorderColor() const {
  return m_selectedBorderColor;
}
QColor Settings::getAnimateColor() const {
  return m_animateColor;
}
QColor Settings::getAnimateBorderColor() const {
  return m_animateBorderColor;
}
QColor Settings::getBgBoardColor() const {
  return m_bgBoardColor;
}
QColor Settings::getOutlineBoardColor() const {
  return m_outlineBoardColor;
}
QColor Settings::getGridBoardColor() const {
  return m_gridBoardColor;
}
QColor Settings::GetNeighboursColor() const {
  return m_neighboursColor;
}
QColor Settings::GetNeighboursBorderColor() const {
  return m_neighboursBorderColor;
}
