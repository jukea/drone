include (../../../config.pro)

TEMPLATE=lib
SOURCES+=Gear_OscFilter.cpp
HEADERS+=Gear_OscFilter.h
INCLUDEPATH+=../../core/ ../../core/types ../../../lib/liblo

#linux
unix:!macx:LIBS+=-L../../../lib/ -ldroneCore -L../../../lib/liblo/src/.libs -llo
unix:!macx:TARGET=../../../gears/drone/Gear_OscFilter
unix:!macx:QMAKE_RPATH = -Wl,-rpath,lib/liblo/src/.libs/,-rpath,


#osx
macx:TARGET=../../../drone.app/Contents/PlugIns/drone/Gear_OscFilter
macx:LIBS+=-L../../../drone.app/Contents/Frameworks -ldroneCore -llo



