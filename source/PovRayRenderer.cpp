//#########################################################################################################
//## PovRayRenderer.cpp
//#########################################################################################################
//##
//## Handles the rendering of a POV-Ray instance, based on an inputfile and a rendering area
//## It starts a povray subprocess and emits a signal when the rendering is finished
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "PovRayRenderer.h"
#include "Config.h"

#include <QStringList>
#include <QList>
#include <QByteArray>
#include <iostream>

// ==> PovRayRenderer(povFile, initFile)
// Constructor
//		povFile: POV-Ray file to be rendered
//		initFile: POV-Ray ini file to specify the properties of the rendering
//--------------------------------------------------------------------
PovRayRenderer::PovRayRenderer(QString povFile, QString initFile)
{
	_init = initFile;
	_process.setWorkingDirectory(QString::fromStdString(Config::getSingleton().POVRAY) );

	connect(&_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished( int, QProcess::ExitStatus)));
	connect(&_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
	connect(&_process, SIGNAL(started()), this, SLOT(started()));
	connect(&_process, SIGNAL(stateChanged(QProcess::ProcessState newState)), this, SLOT(stateChanged(QProcess::ProcessState newState)));

	// Call-back for the standard output of the subprocess
	connect(&_process, SIGNAL(readyReadStandardOutput()),this, SLOT(displayOutputMsg()));
	connect(&_process, SIGNAL(readyReadStandardError()),this, SLOT(displayErrorMsg()));


}

// ==> ~PovRayRenderer()
// Destructor
//--------------------------------------------------------------------
PovRayRenderer::~PovRayRenderer()
{
	disconnect(&_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished( int, QProcess::ExitStatus)));
	disconnect(&_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
	disconnect(&_process, SIGNAL(started()), this, SLOT(started()));
	disconnect(&_process, SIGNAL(stateChanged(QProcess::ProcessState newState)), this, SLOT(stateChanged(QProcess::ProcessState newState)));

	disconnect(&_process, SIGNAL(readyReadStandardOutput()),this, SLOT(displayOutputMsg()));
	disconnect(&_process, SIGNAL(readyReadStandardError()),this, SLOT(displayErrorMsg()));
}


// ==> render()
// Startup the subprocess to render
//--------------------------------------------------------------------
void PovRayRenderer::render()
{	

	QStringList args;
	
	#ifndef unix
		args.push_back(QString("/EXIT"));
		args.push_back(QString("/RENDER"));
	#endif

	#ifdef unix
		QString prevdir = QDir::currentDirPath();
		QDir::setCurrent(Config::getSingletonPtr()->TEMP.c_str());
	#endif   

	args.push_back(_init);
	#ifdef unix
		//args.push_back( QString("+WL0"));
	#endif
		_process.setReadChannel(QProcess::StandardOutput);
    #ifdef _WIN32
		_process.start(QString::fromStdString(Config::getSingleton().POVRAY) + "pvengine64.exe", args);
    #endif
	#ifdef _WIN64
		_process.start("../povray/pvengine64.exe", args);
    #endif

    #ifdef unix
		_process.start("povray", args);
    #endif

	#ifdef unix
		QDir::setCurrent( prevdir );
	#endif
}

// ==> finished(exitCode, exitStatus)
// Called when the subprocess is finished
//--------------------------------------------------------------------
void PovRayRenderer::finished( int exitCode, QProcess::ExitStatus exitStatus)
{
	QByteArray msg = _process.readAllStandardOutput();
	std::cout << "output: " << msg.data();
	this->_info.progress = this->_info.endRow;
	if (exitStatus == QProcess::NormalExit)
	{
		emit finishedRendering(this->_info);
	}
	else
		;
}

// ==> displayOutputMsg()
// Called when there is standard output of the subprocess
//--------------------------------------------------------------------
void PovRayRenderer::displayOutputMsg(){
	_process.setReadChannel(QProcess::StandardOutput);
	QByteArray msg = _process.readAllStandardOutput();
	parseOutputMessage(QString(msg.data()));
	emit rendererCallOutput(QString(msg.data()), this->_info, false);
}


// ==> parseOutputMessage(output)
// Seek for progress information for parsing or for rendering
//--------------------------------------------------------------------
void PovRayRenderer::parseOutputMessage(QString output)
{
	QRegExp re("\\((\\d+),(\\d+)\\)\\s+to\\s+\\((\\d+),(\\d+)\\)\\s*\\w*(\\d+)\\:(\\d+)\\:(\\d+)\\s+Rendering\\s+line\\s+(\\d+)\\s+of\\s+(\\d+)", Qt::CaseInsensitive);
	QRegExp re2("(\\d+)\\:(\\d+)\\:(\\d+)\\s+Rendering\\s+line\\s+(\\d+)\\s+of\\s+(\\d+)", Qt::CaseInsensitive);
 	int pos = 0;	
	while ((pos = re2.indexIn(output, pos)) != -1) {
	    _info.renderHour = re2.cap(1).toInt();
	     _info.renderMin = re2.cap(2).toInt();
	     _info.renderSec = re2.cap(3).toInt();
	     _info.progress = re2.cap(4).toInt();
	     pos += re2.matchedLength();
	 }
	 
	QRegExp re3("(\\d+)\\:(\\d+)\\:(\\d+)\\s+Parsing\\s+(\\d+)K", Qt::CaseInsensitive);
 	int pos2 = 0;	
	while ((pos = re3.indexIn(output, pos2)) != -1) {
	     // 
	     _info.parseHour = re3.cap(1).toInt();
	     _info.parseMin = re3.cap(2).toInt();
	     _info.parseSec = re3.cap(3).toInt();
	     _info.parseProgress = re3.cap(4).toInt();
	     pos2 += re3.matchedLength();
	 }
}

// ==> displayErrorMsg()
// Called when there is standard output error of the subprocess
//--------------------------------------------------------------------
void PovRayRenderer::displayErrorMsg(){
	QByteArray msg = _process.readAllStandardError();
	parseOutputMessage(QString(msg.data()));
	emit rendererCallOutput(QString(msg.data()), this->_info, true);
}

// ==> error(error)
// Called when there is an error in the subprocess
//--------------------------------------------------------------------
void PovRayRenderer::error(QProcess::ProcessError error)
{
	std::cout << "QProcess Error!" << std::endl;
	std::cout << "\tError State: ";
	switch (error)
	{
		case QProcess::Crashed:
			std::cout << "Crashed";
			break;
		case QProcess::FailedToStart:
			std::cout << "FailedToStart";
			break;
		case QProcess::ReadError:
			std::cout << "ReadError";
			break;
		case QProcess::Timedout:
			std::cout << "Timedout";
			break;
		case QProcess::UnknownError:
			std::cout << "UnknownError";
			break;
		case QProcess::WriteError:
			std::cout << "WriteError";
			break;
		default:
			std::cout << "Unknown";
	}
	std::cout << std::endl;
}

// ==> started()
// Called when the subprocess is started
//--------------------------------------------------------------------
void PovRayRenderer::started()
{
	std::cout << "QProcess Started" << std::endl;
}

// ==> stateChanged(newState)
// Called when the state of the subprocess is changed
//--------------------------------------------------------------------
void PovRayRenderer::stateChanged(QProcess::ProcessState newState)
{
	std::cout << "QProcess State Changed!" << std::endl;
	std::cout << "\tState: ";
	switch (newState)
	{
	case QProcess::NotRunning:
			std::cout << "NotRunning";
			break;
	case QProcess::Running:
			std::cout << "Running";
			break;
	case QProcess::Starting:
			std::cout << "Starting";
			break;
		default:
			std::cout << "Unknown";
	}
	std::cout << std::endl;
}
