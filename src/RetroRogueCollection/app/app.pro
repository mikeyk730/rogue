QT += qml quick widgets sql
TARGET = cool-retro-term 

DESTDIR = $$OUT_PWD/../

HEADERS += \
    fileio.h \
    ../import/utility.h

SOURCES = main.cpp \
    fileio.cpp \
    ../import/utility.cpp \
    ../import/utility_qml.cpp

macx:ICON = icons/crt.icns

RESOURCES += qml/resources.qrc

win32:LIBS += "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/x64/User32.Lib"

#########################################
##              INTALLS
#########################################

target.path += /usr/bin/

INSTALLS += target

# Install icons
unix {
    icon32.files = icons/32x32/cool-retro-term.png
    icon32.path = /usr/share/icons/hicolor/32x32/apps
    icon64.files = icons/64x64/cool-retro-term.png
    icon64.path = /usr/share/icons/hicolor/64x64/apps
    icon128.files = icons/128x128/cool-retro-term.png
    icon128.path = /usr/share/icons/hicolor/128x128/apps
    icon256.files = icons/256x256/cool-retro-term.png
    icon256.path = /usr/share/icons/hicolor/256x256/apps

    INSTALLS += icon32 icon64 icon128 icon256
}