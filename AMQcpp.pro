QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cppAMQ.cpp \
        main.cpp

#SOURCES += \
#    $$PWD/../Rosco/BuildDependencies/TimeZoneDB/TimeZoneDB.cpp \
#    $$PWD/../Rosco/BuildDependencies/QBson/QBson.cpp \
#    $$PWD/../Rosco/BuildDependencies/QBson/bson.cpp \
#    $$PWD/../Rosco/BuildDependencies/QBson/encoding.cpp \
#    $$PWD/../Rosco/BuildDependencies/BsonProtocol/BsonProtocolEngine.cpp \
#    $$PWD/../Rosco/BuildDependencies/BsonProtocol/rijndael.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    cppAMQ.h

INCLUDEPATH += $$PWD/../Rosco/BuildDependencies/QBson
INCLUDEPATH += $$PWD/../Rosco/BuildDependencies/BsonProtocol

INCLUDEPATH += $$PWD/../Rosco/BuildDependencies/amq
INCLUDEPATH += $$PWD/../Rosco/BuildDependencies/amq/apr-1

#INCLUDEPATH += $$PWD/../Rosco/BuildDependencies/amq/decaf/util/logging

#INCLUDEPATH += $$PWD/../Rosco/RoscoFileServer/FileServer

#INCLUDEPATH += /opt/Qt/5.15.2/gcc_64/include/QtCore/5.15.2/QtCore/
#INCLUDEPATH += /opt/Qt/5.15.2/gcc_64/include/QtCore/5.15.2/QtCore/private
#INCLUDEPATH += /opt/Qt/5.15.2/gcc_64/include/QtCore/5.15.2/

LIBS += -L$$PWD/../dependencies -lactivemq-cpp -lapr-1
