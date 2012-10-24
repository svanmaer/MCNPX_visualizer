//#########################################################################################################
//## PythonBinder.cpp
//#########################################################################################################
//##
//## Handles the callback of a python method
//## It starts a python subprocess and emits a signal when the process is finished
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "PythonBinder.h"

#include <QStringList>
#include <QList>
#include <iostream>
#include "Config.h"

// ==> PythonBinder()
// Constructor
//--------------------------------------------------------------------
PythonBinder::PythonBinder()
{
	_process = new QProcess(this);
	_process->setWorkingDirectory (QString::fromStdString(Config::getSingleton().PYTHON) );

	connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished( int, QProcess::ExitStatus)));
	connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
	connect(_process, SIGNAL(started()), this, SLOT(started()));
	connect(_process, SIGNAL(stateChanged(QProcess::ProcessState newState)), this, SLOT(stateChanged(QProcess::ProcessState newState)));
	connect(_process, SIGNAL(readyReadStandardOutput()),this, SLOT(displayOutputMsg()));
	connect(_process, SIGNAL(readyReadStandardError()),this, SLOT(displayErrorMsg()));
}

// ==> ~PythonBinder()
// Destructor
//--------------------------------------------------------------------
PythonBinder::~PythonBinder()
{
	disconnect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished( int, QProcess::ExitStatus)));
	disconnect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
	disconnect(_process, SIGNAL(started()), this, SLOT(started()));
	disconnect(_process, SIGNAL(stateChanged(QProcess::ProcessState newState)), this, SLOT(stateChanged(QProcess::ProcessState newState)));
	disconnect(_process, SIGNAL(readyReadStandardOutput()),this, SLOT(displayOutputMsg()));
	disconnect(_process, SIGNAL(readyReadStandardError()),this, SLOT(displayErrorMsg()));

	if (_process != NULL)
		delete _process;
}

// ==> displayOutputMsg()
// Called when there is standard output of the subprocess
//--------------------------------------------------------------------
void PythonBinder::displayOutputMsg(){
	_process->setReadChannel(QProcess::StandardOutput);
	QByteArray msg = _process->readAllStandardOutput();
	emit pythonCallOutput(QString(msg.data()), _method, false);
}

// ==> displayErrorMsg()
// Called when there is standard output error of the subprocess
//--------------------------------------------------------------------
void PythonBinder::displayErrorMsg(){
  QByteArray msg = _process->readAllStandardError();
  emit pythonCallOutput(QString(msg.data()), _method, true);
}

// ==> call(method, args)
// Start the python subprocess
//		method: python method to be called
//		args: python arguments for the method
//--------------------------------------------------------------------
void PythonBinder::call(QString method, QStringList args)
{	
	_method = method;
	QString PYTHON_PATH = "python";
	QStringList pythonArgs;
	// Use the right directory of the python functions
	QString pythonFile = QString::fromStdString(Config::getSingleton().PYTHON);
	pythonFile = pythonFile + method + ".py";
	pythonArgs.push_back(pythonFile);
	for (int i=0; i<args.size(); i++)
	{
		pythonArgs.push_back(args[i]);
	}
	
	_process->setReadChannel(QProcess::StandardOutput);
	_process->start(PYTHON_PATH, pythonArgs);
}


// ==> finished(exitCode, exitStatus)
// Called when the subprocess is finished
//--------------------------------------------------------------------
void PythonBinder::finished( int exitCode, QProcess::ExitStatus exitStatus)
{
	std::cout << "QProcess Finished" << std::endl;
	std::cout << "\tExit Code: " << exitCode << std::endl;
	if (exitStatus == QProcess::NormalExit)
	{
		emit pythonCallFinished(_method);
		std::cout << "\tExit Status: NormalExit" << std::endl;
	}
	else
		std::cout << "\tExit Status: CrashExit " << std::endl;
}

// ==> error(error)
// Called when there is an error in the subprocess
//--------------------------------------------------------------------
void PythonBinder::error(QProcess::ProcessError error)
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
void PythonBinder::started()
{
	std::cout << "QProcess Started" << std::endl;
}


// ==> stateChanged(newState)
// Called when the state of the subprocess is changed
//--------------------------------------------------------------------
void PythonBinder::stateChanged(QProcess::ProcessState newState)
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