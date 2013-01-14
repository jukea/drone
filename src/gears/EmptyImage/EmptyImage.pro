include (../../../config.pro)

TEMPLATE=lib
SOURCES+=Gear_EmptyImage.cpp
HEADERS+=Gear_EmptyImage.h
INCLUDEPATH+=../../core/ ../../core/types

#linux
unix:!macx:LIBS+=-L../../../lib/ -ldroneCore
unix:!macx:TARGET=../../../gears/drone/Gear_EmptyImage

#osx
macx:TARGET=../../../drone.app/Contents/PlugIns/drone/Gear_EmptyImage
macx:LIBS+=-L../../../drone.app/Contents/Frameworks -ldroneCore


#The following line was inserted by qt3to4
QT += xml  opengl qt3support 
