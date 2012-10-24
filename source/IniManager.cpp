//#########################################################################################################
//## IniManager.cpp
//#########################################################################################################
//##
//## Creates an "ini-file" for POV-Ray
//## It's possible to define some basic option like input file, output file, quality, antialiasing, resolution, ...
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "IniManager.h"

#include <QFile>
#include <QTextStream>

// ==> IniManager(inputFile, outputFile, width, height, nprocesses)
// Constructor
//--------------------------------------------------------------------
IniManager::IniManager(QString inputFile, QString outputFile, int width, int height, int nprocesses)
{
	_inputFileName = inputFile;
	_outputFileName = outputFile;
	_outputFileType = "N"; // PNG
	_quality = 1;
	_antialias = false;
	_width = 400;
	_height = 300;
}

// ==> ~IniManager()
// Destructor
//--------------------------------------------------------------------
IniManager::~IniManager()
{
	;
}

// ==> ~createIniFile(outputFile, startColumn, endColumn, startRow, endRow)
// Create an inifile (outputFile) and specify the rendering area from (startRow,startColumn) to (endRow, endColumn)
//--------------------------------------------------------------------
bool IniManager::createIniFile(QString outputFile, int startColumn, int endColumn, int startRow, int endRow)
{
	QFile file(outputFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;

    QTextStream out(&file);
	out << "# INI file created by MCNPXVisualizer\n";
	out << "###############################################\n";
	out << "#\n";
	
	out << "Input_File_Name=" << _inputFileName << "\n";
	out << "Output_File_Name=" << _outputFileName << "\n";
	out << "Output_File_Type=" << _outputFileType << "\n";
	out << "#\n";
	out << "# SIZE PARAMETERS\n";
	out << "Height=" << _height << "\t\t# Sets screen height to n pixels\n";	
	out << "Width=" << _width << "\t\t# Sets screen width to n pixels\n";
	if (startColumn <= 0)
		out << "Start_Column=" << 1 << "\t\t# Set first column to n pixels\n";
	else
		out << "Start_Column=" << startColumn << "\t\t# Set first column to n pixels\n";
	if (endColumn <= 0 || endColumn > _width)
		out << "End_Column=" << _width << "\t\t# Set last column to n pixels\n";
	else
		out << "End_Column=" << endColumn << "\t\t# Set last column to n pixels\n";
	if (startRow <= 0)
		out << "Start_Row=" << 1 << "\t\t# Set first row to n pixels\n";
	else
		out << "Start_Row=" << startRow << "\t\t# Set first row to n pixels\n";
	if (endRow <= 0 || endRow > _height)
		out << "End_Row=" << _height << "\t\t# Set last row to n pixels\n";
	else
		out << "End_Row=" << endRow << "\t\t# Set last row to n pixels\n";
	out << "#\n";
	out << "# QUALITY PARAMETERS\n";
	out << "Quality=" << _quality << "\n";
	if (_antialias)
		out << "Antialias=" << "on" << "\n";
	else
		out << "Antialias=" << "off" << "\n";
	out << "#\n";
	out << "# OTHER PARAMETERS\n";
	out << "All_Console=Off\n";
	out << "Split_Unions=On\n";
	out << "Remove_Bounds=On\n";
	out << "Display_Gamma=1.0 \n";
	out << "+D \n";
	out << "+MB \n";
	out << "-GW\n";
	out << "-WL0\n";
	out << "-V      # give information in the command line \n";
	out << "Vista_Buffer=on   \n";
	out << "Draw_Vistas=on \n";
	out << "Light_Buffer=on   \n";
	out << "#\n";
	out << "# LIBRARY PARAMETERS\n";
	out << "Library_Path=include\n";
	out << "[Permitted Input Paths]\n";
    out << "\t1=%INSTALLDIR%\n";
    out << "\t2=../povray/\n";
	out << "\t3=../temp/ \n";
	out << "\n";
	out << "[Permitted Output Paths]\n";
	out << "\t1=%INSTALLDIR%\n";
	out << "\t2=../povray/ \n";
	out << "\t3=../temp/ \n";
	file.close();
}



template<> IniManager* Singleton<IniManager>::ms_Singleton = 0;
IniManager* IniManager::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
IniManager& IniManager::getSingleton(void)
	{  
		return ( *ms_Singleton );  
	}
