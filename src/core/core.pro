######################################################################
# Automatically generated by qmake (1.07a) Sat May 22 15:57:15 2004
######################################################################

CONFIG = qt warn_on thread agg
include (../../config.pro)
 
message(Core : Builing Makefile with the following config flags : $$CONFIG)
 
TEMPLATE = lib
INCLUDEPATH += . types ../contrib/frei0r/spec composite

#mac

macx:INCLUDEPATH += $$QMAKE_MAC_SDK/System/Library/Frameworks/CoreFoundation.framework/Headers
#macx:LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
#macx:LIBS += /System/Library/Frameworks/Carbon.framework/Carbon
macx:LIBS += -lz
macx:LIBS += -framework OpenGL -framework CoreAudio -framework CoreMidi -framework CoreFoundation
macx:QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
macx:TARGET = ../../drone.app/Contents/Frameworks/droneCore
macx:QMAKE_CXXFLAGS += -D__MACOSX_CORE__

#linux
unix:!macx:LIBS += -L/usr/X11R6/lib -lGL -lasound -lpthread
unix:!macx:TARGET = ../../lib/droneCore
unix:!macx:QMAKE_CXXFLAGS += -D__LINUX_ALSASEQ__


HEADERS += config.h \
error.h \
Array.h \
Array2D.h \
CircularBuffer.h \
ColorSpace.h \
ConnectionItem.h \
ControlPanel.h \
Control.h \
DroneCore.h \
Engine.h \
Gear.h \
GearControl.h \
GearFrei0r.h \
types/AbstractType.h \
types/AreaArrayType.h \
types/AreaType.h \
types/FFTType.h \
types/StringType.h \
types/OscMessageType.h \
types/ValueType.h \
types/VideoRGBAType.h \
types/SignalType.h \
types/DisplaceGrid.h \
types/ArrayType.h \
types/Array2DType.h \
types/VideoChannelType.h \
types/VideoBinaryType.h \
types/PaletteRGBAType.h \
types/EnumType.h \
GearClassification.h \
GearGui.h \
GearConverter.h \
GearMaker.h \
ISchemaEventListener.h \
DroneMath.h \
Media.h \
MediaPool.h \
MediaMovie.h \
MetaGear.h \
MetaGearMaker.h \
MidiEngine.h \
AbstractPlug.h \
Plug.h \
PlugBox.h \
Properties.h \
Project.h \
Rasterer.h \
Random.h \
SummedAreaTable.h \
Schema.h \
SchemaGui.h \
SignalInfo.h \
StringUtils.h \
TextureGl.h \
ThreadUtil.h \
Timing.h \
Typedefs.h \
Utils.h \
VideoInfo.h \
XMLHelper.h \
RtMidi/RtMidi.h \
RtMidi/RtError.h \
composite/Composite.h \
composite/CompositeGeneric.h \
composite/AlphaComposite.h
 
SOURCES += error.cpp \
ConnectionItem.cpp \
ControlPanel.cpp \
Control.cpp \
DroneCore.cpp \
Engine.cpp \
Gear.cpp \
GearControl.cpp \
GearFrei0r.cpp \
types/AreaType.cpp \
types/EnumType.cpp \
types/VideoChannelType.cpp \
types/VideoRGBAType.cpp \
types/ValueType.cpp \
types/StringType.cpp \
GearGui.cpp \
GearMaker.cpp \
Math.cpp \
MediaPool.cpp \
MediaMovie.cpp \
MetaGear.cpp \
MetaGearMaker.cpp \
MidiEngine.cpp \
AbstractPlug.cpp \
PlugBox.cpp \
Properties.cpp \
Project.cpp \
Rasterer.cpp \
Random.cpp \
Schema.cpp \
SchemaGui.cpp \
SignalInfo.cpp \
StringUtils.cpp \
TextureGl.cpp \
Timing.cpp \
VideoInfo.cpp \
XMLHelper.cpp  \
RtMidi/RtMidi.cpp \
composite/Composite.cpp \
composite/CompositeGeneric.cpp \
composite/AlphaComposite.cpp

agg {

INCLUDEPATH += ../../lib/agg23/include ../../lib/agg23/svg
LIBS += -L../../lib/agg23/src/ -lagg
HEADER += AggWrapper.h types/VectorialType.h
SOURCES += AggWrapper.cpp types/VectorialType.cpp

}
  
#The following line was inserted by qt3to4
QT += xml  qt3support 
