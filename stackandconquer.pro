#  This file is part of StackAndConquer.
#  Copyright (C) 2015-2018 Thorsten Roth
#
#  StackAndConquer is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  StackAndConquer is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with StackAndConquer.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE      = app

unix: !macx {
       TARGET = stackandconquer
} else {
       TARGET = StackAndConquer
}

VERSION       = 0.8.0
QMAKE_TARGET_PRODUCT     = "StackAndConquer"
QMAKE_TARGET_DESCRIPTION = "Challenging tower conquest board game"
QMAKE_TARGET_COPYRIGHT   = "(C) 2015-2018 Thorsten Roth"

DEFINES      += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
                APP_VERSION=\"\\\"$$VERSION\\\"\" \
                APP_DESC=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\" \
                APP_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui svg qml widgets

SOURCES      += main.cpp\
                stackandconquer.cpp \
                game.cpp \
                board.cpp \
                player.cpp \
                settings.cpp \
                opponentjs.cpp

HEADERS      += stackandconquer.h \
                game.h \
                board.h \
                player.h \
                settings.h \
                opponentjs.h

FORMS        += stackandconquer.ui \
                settings.ui

RESOURCES    += res/stackandconquer_resources.qrc \
                res/translations.qrc
win32:RC_FILE = res/stackandconquer_win.rc

TRANSLATIONS += lang/stackandconquer_de.ts

macx {
  ICON               = res/images/icon.icns
  QMAKE_INFO_PLIST   = res/Info.plist

  CPU_DATA.path      = Contents/Resources
  CPU_DATA.files    += data/cpu
  QMAKE_BUNDLE_DATA += CPU_DATA
}

unix: !macx {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    isEmpty(BINDIR) {
        BINDIR = bin
    }

    target.path     = $$PREFIX/$$BINDIR/

    data.path       = $$PREFIX/share/stackandconquer
    data.files     += data/cpu

    desktop.path    = $$PREFIX/share/applications
    desktop.files  += data/stackandconquer.desktop

    pixmap.path     = $$PREFIX/share/pixmaps
    pixmap.files   += res/images/stackandconquer_64x64.png \
                      res/images/stackandconquer.xpm

    #icons.path      = $$PREFIX/share/icons
    #icons.files    += res/images/hicolor
    
    man.path        = $$PREFIX/share
    man.files      += man

    INSTALLS       += target \
                      data \
                      desktop \
                      pixmap \
                      #icons \
                      man
}
