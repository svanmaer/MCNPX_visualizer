//#########################################################################################################
//## IniManager.h
//#########################################################################################################
//##
//## Creates an "ini-file" for POV-Ray
//## It's possible to define some basic option like input file, output file, quality, antialiasing, resolution, ...
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################


#ifndef INI_MANAGER_H
#define INI_MANAGER_H
	
#include <iostream>
#include "Singleton.h"
#include <QString>
#include "Config.h"


class IniManager : public Singleton<IniManager>
{
	public:
		IniManager(QString inputFile=(QString::fromStdString(Config::getSingleton().TEMP) + "combined.pov"), QString outputFile=(QString::fromStdString(Config::getSingleton().TEMP)) + "output.png", int width=400, int height=300, int nprocesses=1);
		virtual ~IniManager();
	
		void setInputFileName(QString fileName){ _inputFileName = fileName;}
		void setOutputFileName(QString fileName){ _outputFileName = fileName;}
		void setOutputFileType(QString fileType){ _outputFileType = fileType;}
		void setQuality(int quality){ _quality = quality;}
		void setAnitalias(bool antialias){ _antialias = antialias;}
		void setWidth(int width){ _width = width;}
		void setHeight(int height){ _height = height;}

		bool createIniFile(QString outputFile, int startColumn = 0, int endColumn = 0, int startRow = 0, int endRow = 0);

		static IniManager* getSingletonPtr(void);
		static IniManager& getSingleton(void);

	private:
		QString _inputFileName;		// Input POV-Ray file
		QString _outputFileName;	// Output image file
		QString _outputFileType;	// Output image type

		int _nProcess;				// Number of processors to use
		
		int _quality;				// Quality of the rendered image [0:11]
		bool _antialias;			// Turn on/off antialiasing
		int _width;					// Width of the rendered image
		int _height;				// Height of the rendered image
};



#endif

