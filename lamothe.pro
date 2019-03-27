#-------------------------------------------------
#
# Project created by QtCreator 2019-03-09T17:26:09
#
#-------------------------------------------------

QT       += core gui widgets svg

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

# Support for this option is relatively new, older QtCreator installation may not have it. "c++1z" seems to
# work in those cases.
CONFIG += c++17

QMAKE_CXXFLAGS += -std=c++17 -Wall -Wextra -pedantic-errors

!defined(USE_SFML) {
message("Using SFML audio backend.")
LIBS += -lsfml-audio            # SFML audio shared library
LIBS += -lsfml-system           # SFML system shared library, needed for sf::Time
LIBS += -lopenal                # dependency
LIBS += -lFLAC                  # dependency
LIBS += -lvorbisenc             # dependency
LIBS += -lvorbisfile            # dependency
LIBS += -lvorbis                # dependency
LIBS += -logg                   # dependency

SOURCES += audio_backends/sfml_wrapper.cpp
HEADERS += audio_backends/sfml_wrapper.hpp
}

!defined(USE_TAGLIB) {
message("Using taglib metadata backend.")
LIBS += -ltag                   #Taglib audio metadata library

SOURCES += "metadata_backends/taglib_wrapper.cpp"
HEADERS += "metadata_backends/taglib_wrapper.hpp"
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    song_tile_model.cpp

HEADERS += \
        mainwindow.hpp \
    song_tile_model.hpp \
    music_metadata.hpp \
    user_desired_state.hpp \
    metadata_interface.hpp \
    metadata_wrapper.hpp \
    audio_wrapper.hpp \
    audio_interface.hpp

FORMS += \
        mainwindow.ui

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin

else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target

