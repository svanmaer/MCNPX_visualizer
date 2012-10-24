//#########################################################################################################
//## PythonBinder.h
//#########################################################################################################
//##
//## Handles the callback of a python method
//## It starts a python subprocess and emits a signal when the process is finished
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################


#ifndef PYTHON_BINDER_H
#define PYTHON_BINDER_H

#include <QString>
#include <QTimer>
#include <QProcess>
#include <iostream>
#include <fstream>

using namespace std;

class PythonBinder : public QObject
{
	Q_OBJECT
	public:
		PythonBinder();
		~PythonBinder();

		void call(QString method, QStringList args); // calls a python method with a list of arguments

	private:
		QProcess* _process;
		QString _method;

	private slots:
		void finished( int exitCode, QProcess::ExitStatus exitStatus);
		void error(QProcess::ProcessError error);
		void started();
		void stateChanged(QProcess::ProcessState newState);
		void displayOutputMsg();
		void displayErrorMsg();

	signals:
		// emitted when the python call is finished
		void pythonCallFinished(QString method);
		// emitted when there is standard output from the python subprocess
		void pythonCallOutput(QString output, QString method, bool isError = true);
		
};

#endif