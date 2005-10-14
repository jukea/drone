include (../../../../../config.pro)

TEMPLATE=lib

SOURCES+=Gear_TA_TravelAgent.cpp
HEADERS+=Gear_TA_TravelAgent.h

INCLUDEPATH+=../../../../core/ ../../core/ ../../../../core/types
unix:!macx:LIBS+=-L../../../../../lib/ -ldroneCore -ldroneTACore
unix:!macx:TARGET=../../../../../gears/Gear_TA_TravelAgent										  

#osx
macx:LIBS+=-L../../../../../drone.app/Contents/Frameworks -ldroneCore -ldroneTACore -L../../../../../lib/
macx:TARGET=../../../../../drone.app/Contents/PlugIns/Gear_TA_TravelAgent
macx:QMAKE_LFLAGS+=-read_only_relocs suppress