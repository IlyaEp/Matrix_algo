TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        labelgraph.cpp \
        main.cpp \
        rsa.cpp \
        tensor_algo.cpp \
        tensor_paths.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GraphBLAS/build/release/ -lgraphblas
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GraphBLAS/build/debug/ -lgraphblas
else:unix: LIBS += -L$$PWD/../../GraphBLAS/build/ -lgraphblas

INCLUDEPATH += $$PWD/../../GraphBLAS/Include
DEPENDPATH += $$PWD/../../GraphBLAS/Include

HEADERS += \
    labelgraph.h \
    rsa.h \
    tensor_algo.h \
    tensor_paths.h
