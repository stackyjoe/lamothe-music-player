#-------------------------------------------------
#
# Project created by QtCreator 2019-03-09T17:26:09
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = lamothe
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++1z

QMAKE_CXXFLAGS += -std=c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    music_library.cpp \
    sfml_player.cpp \
    song_tile_widget.cpp \
    daemon_watcher.tpp \
    taglib_handler.cpp

HEADERS += \
        mainwindow.hpp \
    music_library.hpp \
    music_player.hpp \
    sfml_player.hpp \
    song_tile_widget.hpp \
    tag_handler.hpp \
    taglib_handler.hpp

FORMS += \
        mainwindow.ui \
    song_tile_widget.ui

LIBS += -lsfml-audio            #SFML Static Module
LIBS += -lsfml-system
LIBS += -lopenal                #Dependency
LIBS += -lFLAC                  #Dependency
LIBS += -lvorbisenc             #Dependency
LIBS += -lvorbisfile            #Dependency
LIBS += -lvorbis                #Dependency
LIBS += -logg                   #Dependency
LIBS += -ltag                   #Dependency

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
