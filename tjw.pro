HEADERS += source/BFTSGrid.h \
			source/Grid.h \
			source/Timer.h \
			source/Tree.h

SOURCES = source/BFTSGrid.cpp \
          source/Grid.cpp \
		  source/Timer.cpp \
		  source/Tjw.cpp

QMAKE_CXXFLAGS += -std=c++0x
QT     -= gui core
LIBS   -= -lQtGui -lQtCore

TARGET = tjw
		
