#-------------------------------------------------
#
# Project created by QtCreator 2011-11-15T16:12:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = skyrim_config
TEMPLATE = app

INCLUDEPATH += include src resource locale ui

#CODECFORTR = UTF-8

TRANSLATIONS =  locale/skrmcnf_de.ts \
                locale/skrmcnf_en.ts \
                locale/skrmcnf_sp.ts \
                locale/default.ts

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/cskyrimconfig.cpp \
    src/cini_parse.cpp \
    src/backups.cpp \
    src/clog.cpp \
    src/cpreviewparser.cpp \
    src/exporter.cpp \
    src/importer.cpp \
    src/about.cpp \
    src/helpers.cpp

HEADERS  += include/mainwindow.h \
    include/cskyrimconfig.h \
    include/version.h \
    include/cini_parse.h \
    include/backups.h \
    include/clog.h \
    include/cpreviewparser.h \
    include/exporter.h \
    include/importer.h \
    include/about.h \
    include/helpers.h

FORMS    += ui/mainwindow.ui \
    ui/backups.ui \
    ui/exporter.ui \
    ui/importer.ui \
    ui/about.ui



win32: LIBS += -lshell32

OTHER_FILES += \
    resource/icon.rc

RESOURCES += \
    resource/logo.qrc \
    resource/tab_icons.qrc \
    resource/default_preview.qrc \
    resource/logo.qrc

RC_FILE = resource/icon.rc

DEFINES += __PREVIEWS_EX__

previews {
    INCLUDEPATH += previews
    RESOURCES += previews/previews.qrc
    DEFINES -= __PREVIEWS_EX__
    message("packing previews")
}

static {
    QTPLUGIN += qjpeg
    DEFINES += STATIC
    message("Static build.")
}

# request admin priviliges upon start. Sometimes necessary to write to the config files
#win32 {
#    CONFIG += embed_manifest_exe
#    QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
#}



































