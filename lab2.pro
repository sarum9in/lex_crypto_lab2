QT += widgets qml quick

SOURCES += main.cpp \
    CryptEngine.cpp

CONFIG += c++11

RESOURCES += \
    qml.qrc

OTHER_FILES += qml/main.qml

HEADERS += \
    CryptEngine.hpp
