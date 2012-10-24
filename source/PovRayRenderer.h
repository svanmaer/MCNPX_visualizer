//#########################################################################################################
//## PovRayRenderer.h
//#########################################################################################################
//##
//## Handles the rendering of a POV-Ray instance, based on an inputfile and a rendering area
//## It starts a povray subprocess and emits a signal when the rendering is finished
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef POV_RAY_RENDERER_H
#define POV_RAY_RENDERER_H

#include <QString>
#include <QProcess>
#include <QTimer>

// Contains all the basis rendering information and state
struct PovRayRendererInformation
{
	PovRayRendererInformation(QString outputFile="", int renderNumber=0, int startColumn=0, int endColumn=0, int startRow=0, int endRow=0) 
		: outputFile(outputFile), startColumn(startColumn), endColumn(endColumn), startRow(startRow), endRow(endRow), progress(0)
		  , parseSec(0), parseMin(0), parseHour(0), parseProgress(0){} 
	
	QString outputFile;
	int startColumn;
	int endColumn;
	int startRow;
	int endRow;

	int renderSec;
	int renderMin;
	int renderHour;
	int progress;
	
	int parseSec;
	int parseMin;
	int parseHour;
	int parseProgress;
};

class PovRayRenderer : public QObject
{
	Q_OBJECT
	public:
		PovRayRenderer(QString povFile="../temp/mcnpx.pov", QString initFile="../temp/mcnpx.ini");
		~PovRayRenderer();

		void render();	// start rendering

		void setInfo(PovRayRendererInformation info){_info = info;}
		PovRayRendererInformation getInfo(){ return _info; }

		// search for the parsing and rendering progress in the std of POV-Ray
		void parseOutputMessage(QString output); 

	private:	
		QProcess _process;					// Subprocess for POV-Ray
		QString _init;						// input POV-Ray "ini-file"
		PovRayRendererInformation _info;	// Basic Rendering information (rendering area, progress, ...)

	private slots:
		void finished( int exitCode, QProcess::ExitStatus exitStatus);
		void error(QProcess::ProcessError error);
		void started();
		void stateChanged(QProcess::ProcessState newState);
		void displayOutputMsg();
		void displayErrorMsg();

	signals:
		// emitted when the rendering is finished
		void finishedRendering(PovRayRendererInformation param); 
		// emitted when there is standard output from the POV-Ray subprocess
		void rendererCallOutput(QString output, PovRayRendererInformation param, bool isError = true);
		
};
#endif
