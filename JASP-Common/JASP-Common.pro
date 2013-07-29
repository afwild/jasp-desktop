
QT       -= gui

DESTDIR = ..
TARGET = JASP-Common
TEMPLATE = lib
CONFIG += staticlib

windows:CONFIG += c++11

unix:INCLUDEPATH += /opt/local/include
windows:INCLUDEPATH += C:/progra~1/boost/boost_1_53_0

windows:LIBS += -L.. -lJASP-Common -lole32 -loleaut32

resources.commands = make -C $$PWD/analyses
QMAKE_EXTRA_TARGETS += resources
PRE_TARGETDEPS += resources

SOURCES += \
    options.cpp \
    datasetloader.cpp \
    dataset.cpp \
    csvparser.cpp \
    analysisloader.cpp \
    analysis.cpp \
    option.cpp \
    columns.cpp \
    column.cpp \
    analyses.cpp \
    analysispart.cpp \
    lib_json/json_writer.cpp \
    lib_json/json_valueiterator.inl \
    lib_json/json_value.cpp \
    lib_json/json_reader.cpp \
    lib_json/json_internalmap.inl \
    lib_json/json_internalarray.inl \
    datablock.cpp \
    sharedmemory.cpp \
    options/optionboolean.cpp \
    options/optionfield.cpp \
    options/optionfields.cpp \
    options/optioni.cpp \
    options/optionintegerarray.cpp \
    options/optioninteger.cpp \
    options/optionlist.cpp \
    analyses/frequencies.cpp \
    analyses/ttestonesample.cpp \
    options/optionnumber.cpp

HEADERS +=\
    options.h \
    datasetloader.h \
    dataset.h \
    csvparser.h \
    analysisloader.h \
    analysis.h \
    option.h \
    columns.h \
    column.h \
    analyses.h \
    analysispart.h \
    lib_json/writer.h \
    lib_json/value.h \
    lib_json/reader.h \
    lib_json/json.h \
    lib_json/json_batchallocator.h \
    lib_json/forwards.h \
    lib_json/features.h \
    lib_json/config.h \
    lib_json/autolink.h \
    datablock.h \
    sharedmemory.h \
    options/optionboolean.h \
    options/optionfield.h \
    options/optionfields.h \
    options/optioni.h \
    options/optionintegerarray.h \
    options/optioninteger.h \
    options/optionlist.h \
    rinterface.h \
    analyses/frequencies.h \
    analyses/ttestonesample.h \
    options/optionnumber.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    analyses/frequencies.R \
    analyses/makefile \
    analyses/ttestonesample.R

RESOURCES +=
