TARGET = StuffHacker
TEMPLATE = app
QT = core network
CONFIG += console c++11

HEADERS += \
           src/requestmapper.h \
    src/genericcontroller.h

SOURCES += src/main.cpp \
           src/requestmapper.cpp \
    src/genericcontroller.cpp

unix {
    message("unix")

    for(FILE, OTHER_FILES) {
        message($${FILE})
        mQMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
    }
}

include(httpserver/httpserver.pri)

RESOURCES += \
    resources/Resources.qrc


