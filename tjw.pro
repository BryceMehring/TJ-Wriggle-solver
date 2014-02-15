HEADERS += source/BFTSWrigglerGrid.h \
			source/GBFGSWrigglerGrid.h \
			source/IDDFTSWrigglerGrid.h \
			source/PriorityQueue.h \
			source/Timer.h \
			source/Tree.h \
			source/vec2.h \
			source/Wriggler.h \ 
			source/WrigglerGrid.h

SOURCES = source/BFTSWrigglerGrid.cpp \
		  source/GBFGSWrigglerGrid.cpp \
		  source/IDDFTSWrigglerGrid.cpp \
		  source/Timer.cpp \
		  source/Tjw.cpp \
		  source/Wriggler.cpp \
		  source/WrigglerGrid.cpp

QMAKE_CXXFLAGS += -std=c++0x -pedantic
QT     -= gui core
LIBS   -= -lQtGui -lQtCore

TARGET = tjw
		
