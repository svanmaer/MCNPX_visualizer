//#########################################################################################################
//## Config.h
//#########################################################################################################
//##
//## Contains the basic configuration for the MCNPX Visualizer
//## It defines the basic paths used in the application
//##	=> TEMP: temporal files like color map are stored in this directory
//##	=> PYTHON: contains the python source code for the parser
//##	=> MXNPC: contains some basis MCNPX input examples
//##	=> DATA: contains some data files for the random colors or standard colors
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef CONFIG_H
#define CONFIG_H

#include <QDir>	
#include <iostream>

#include "Singleton.h"


class Config : public Singleton<Config>
{
	public:
	Config()
	{
	#ifdef _WIN32 || _WIN64
		
		std::cout << QDir::root().currentPath().toStdString() << std::endl;
		TEMP = QDir::root().currentPath().append( "/temp/").replace("/", "\\").toStdString();
        PYTHON = QDir::root().currentPath().append( "/python/").replace("/", "\\").toStdString();
        MCNPX = QDir::root().currentPath().append( "/mcnpx/").replace("/", "\\").toStdString();
		POVRAY = QDir::root().currentPath().append( "/povray/").replace("/", "\\").toStdString();
		DATA =  QDir::root().currentPath().append( "/data/").replace("/", "\\").toStdString();
		IMAGES =  QDir::root().currentPath().append( "/images/").replace("/", "\\").toStdString();

	/*	TEMP = "D:\\UHasselt\\2e Master\\Stage\\SVN\\temp\\";
		PYTHON = "D:\\UHasselt\\2e Master\\Stage\\SVN\\python\\MCNPXToPOV\\src\\";
		MCNPX = "D:\\UHasselt\\2e Master\\Stage\\SVN\\python\\MCNPXToPOV\\src\\mcnpx\\";
		POVRAY = "D:\\UHasselt\\2e Master\\Stage\\SVN\\povray\\";
		DATA = "D:\\UHasselt\\2e Master\\Stage\\SVN\\data\\";*/

	#endif
	#ifdef NICK
		TEMP = "D:\\UHasselt\\2e Master\\Stage\\SVN\\temp\\";
		PYTHON = "D:\\UHasselt\\2e Master\\Stage\\SVN\\python\\MCNPXToPOV\\src\\";
		MCNPX = "D:\\UHasselt\\2e Master\\Stage\\SVN\\python\\MCNPXToPOV\\src\\mcnpx\\";
		POVRAY = "D:\\UHasselt\\2e Master\\Stage\\SVN\\povray\\";
		DATA = "D:\\UHasselt\\2e Master\\Stage\\SVN\\data\\";
		IMAGES = "D:\\UHasselt\\2e Master\\Stage\\SVN\\images\\";

	#endif
    #ifdef NORMALLY
		std::cout << "RootPath: " << QDir::root().currentPath().toStdString() << std::endl;
		TEMP = QDir::root().currentPath().append( "/temp/").toStdString();
        PYTHON = QDir::root().currentPath().append( "/python/MCNPXToPOV/src/").toStdString();
        MCNPX = QDir::root().currentPath().append( "/python/MCNPXToPOV/src/mcnpx/").toStdString();
		POVRAY = "";
		DATA =QDir::root().currentPath().append( "/data/").toStdString();
	#endif
    #ifdef unix
		TEMP = QDir::currentDirPath().append( "/temp/").toStdString();
        PYTHON = QDir::currentDirPath().append( "/python/").toStdString();
        MCNPX = QDir::currentDirPath().append( "/python/").toStdString();
		POVRAY = "";
		DATA =QDir::currentDirPath().append(  "/data/").toStdString();
		IMAGES =QDir::currentDirPath().append(  "/images/").toStdString();

    #endif

	}
	virtual ~Config(){};
	
	std::string TEMP;
	std::string PYTHON;
	std::string MCNPX;
	std::string POVRAY;
	std::string DATA;
	std::string IMAGES;
	
	static Config* getSingletonPtr(void);
	static Config& getSingleton(void);
	
};



#endif

