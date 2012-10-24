//#########################################################################################################
//## CameraManager.h
//#########################################################################################################
//##
//## This manager contains all the information for creating a POV-Ray Camera and Light object
//## If there are 3D sections enabled in the Sections3D, this manager adds it to the builden POV-Ray Scene
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################


#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H
	
#include <QString>
#include <QObject>

#include <iostream>

#include "Singleton.h"
#include "Config.h"
#include "Camera.h"
#include "Sections3D.h"


class CameraManager : public Singleton<CameraManager>
{
	public:
		CameraManager(QString inputFile=(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov"));
		virtual ~CameraManager();
		
		static CameraManager* getSingletonPtr(void);
		static CameraManager& getSingleton(void);

		Camera* getCamera(){ return _camera; }

		// Create the output povray file based on the internal class information
		bool createPovRayFile(QString outputPath);

		// Change the input file that contains the POV-Ray Scene
		void setInputFileName(QString fileName){ _inputFileName = fileName;}
		QString getInputFileName() const { return _inputFileName;}

		// Change the depth of recursion of the POV-Ray renderer
		void setMaxTraceLevel(int level){ _maxTraceLevel = level; } 
		int getMaxTraceLevel(){ return _maxTraceLevel; }

		// Define the 3D sections object, used to define the section planes in POV-Ray
		void setSections(Sections3D* sections){ _sections = sections; }
		Sections3D* getSections(){ return _sections; }

		// Enable/Disable cross sections
		void setSectionsEnabled(bool enabled){ _sectionsEnabled = enabled;}
		// Enable/Disable extra clipping of the imp:n=0
		void setClippedByImp0(bool isClipped){_clippedByImp0 = isClipped;}
		// Enable/Disbale orthographic projection
		void setOrthographicProjection(bool orthographic) { _useOrthographicProjection = orthographic; }

		// Overwrite the camera output object
		void setCameraString(QString cameraString) { _cameraString = cameraString; }
		// Overwrite the light output object
		void setLightString(QString lightString) { _lightString = lightString; }

	private:
		QString _inputFileName;		// POV-Ray file of the scene
		Camera* _camera;			// contains the camera position and lookat
		bool _useOrthographicProjection;
		int _maxTraceLevel;
		
		// Section
		Sections3D *_sections;		// contains all the 3D section information
		bool _sectionsEnabled;
		bool _clippedByImp0;

		// Background color of the rendered scene
		float _backgroundColorRed;
		float _backgroundColorGreen;
		float _backgroundColorBlue;

		QString _cameraString;		// String overwrite of the POV-Ray camera object
		QString _lightString;		// String overwrite of the POV-Ray light object	
};

#endif