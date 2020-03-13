QT += core gui widgets

TEMPLATE = app
CONFIG += app c++1z

SOURCES += \
        main.cpp \
    janela.cpp

QMAKE_CXXFLAGS_DEBUG += -g -Og -dA -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wdiv-by-zero -Wendif-labels -Wformat-extra-args -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wlogical-op -Wmissing-declarations -Wno-missing-format-attribute -Wmissing-include-dirs -Wmultichar -Wpacked -Wpointer-arith -Wreturn-type -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Werror=undef -Wno-unused -Wvariadic-macros -Wwrite-strings  -Werror=declaration-after-statement -Werror=implicit-function-declaration -Werror=nested-externs -Werror=strict-prototypes -fno-strict-aliasing -static #-Werror=missing-braces

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3

QMAKE_CXXFLAGS_RELEASE += -Ofast -fopenmp -fomit-frame-pointer -fmodulo-sched -fmodulo-sched-allow-regmoves -fgcse-sm -fgcse-las -fgcse-after-reload -funsafe-loop-optimizations -flive-range-shrinkage -fsched-spec-load-dangerous -fsched2-use-superblocks -floop-nest-optimize -floop-parallelize-all -ftree-parallelize-loops=8 -fprefetch-loop-arrays -ffinite-math-only -march=native -mtune=native -mfpmath="387+sse" -std=c++1z -static

INCLUDEPATH += /usr/local/include/opencv4
INCLUDEPATH += /usr/local/include


LIBS += -L/usr/lib \
-lblas \
-llapack

LIBS += -L/usr/lib -ltesseract
LIBS += -ldl -lrt
LIBS += -L/usr/lib -lomp


LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgproc \
-lopencv_imgcodecs \
-lopencv_objdetect \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_xfeatures2d \
-lopencv_photo \
-lopencv_xphoto \
-lopencv_highgui

FORMS += \
    janela.ui

HEADERS += \
    janela.h


