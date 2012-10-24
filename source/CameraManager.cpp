//#########################################################################################################
//## CameraManager.cpp
//#########################################################################################################
//##
//## This manager contains all the information for creating a POV-Ray Camera and Light object
//## If there are 3D sections enabled in the Sections3D, this manager adds it to the builden POV-Ray Scene
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "CameraManager.h"

#include <QFile>
#include <QTextStream>

#include <cmath>

// ==> CameraManager(inputFile)
// Constructor
//--------------------------------------------------------------------
CameraManager::CameraManager(QString inputFile)
{
	_inputFileName = inputFile;
	_camera = new Camera();
	_sections = new Sections3D();

	_backgroundColorRed = 0.4196f;
	_backgroundColorGreen = 0.5451f;
	_backgroundColorBlue = 0.6667f;

	_sectionsEnabled = true;

	_maxTraceLevel = 5;

	_clippedByImp0 = false;
}

// ==> ~CameraManager()
// Destructor
//--------------------------------------------------------------------
CameraManager::~CameraManager()
{
	if (_camera != NULL)
		delete _camera;
	if (_sections != NULL)
		delete _sections;
}

// ==> createPovRayFile(outputPath)
//  Build a POV-Ray output file based on the camera/light/sections properties
//  The POV-Ray file is always a wrapper for the scene written to a chosen output path under the name "combined.pov"
//  It checks if a section needs to be added and check for the the type of the cross section
//--------------------------------------------------------------------
bool CameraManager::createPovRayFile(QString outputPath)
{
	QFile file(outputPath + "combined.pov");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;

    QTextStream out(&file);
	out << "// POVRAY file created by MCNPXVisualizer\n";
	out << "\n";
		
	out << "#include \"colors.inc\"\n";
	out << "#include \"stones.inc\"\n";
	out << "#include \"camera.pov\"\n";
	out << "#include \"lights.pov\"\n"; 
	out << "\n";
	out << "global_settings\n";
	out << "{\n";
	out << "\tmax_trace_level " << _maxTraceLevel << "\n";
	out << "}\n\n";

	out << "background   { color rgb <" << _backgroundColorRed << ", " << _backgroundColorGreen << ", " << _backgroundColorBlue << "> }\n";
	
	out << "difference{ \n";
	
	out << "\t#include \"" << _inputFileName << "\"\n";
	if (_clippedByImp0)
		out << "\t#include \"" << _inputFileName << "_imp0.pov" << "\"\n";
	out << "\n";

	if (_sections->_useShortCut)
	{
		if (_sections->_typeShortCut == Sections3D::SHORTCUT_X)
		{
			out << "\tplane {x, " << _sections->_shortCutBase << " inverse}\n";
		}
		else if (_sections->_typeShortCut == Sections3D::SHORTCUT_X_INVERSE)
		{
			out << "\tplane {x, " << _sections->_shortCutBase << "}\n";
		}
		else if (_sections->_typeShortCut == Sections3D::SHORTCUT_Y)
		{
			out << "\tplane {y, " << _sections->_shortCutBase << " inverse}\n";
		}
		else if (_sections->_typeShortCut == Sections3D::SHORTCUT_Y_INVERSE)
		{
			out << "\tplane {y, " << _sections->_shortCutBase << "}\n";
		}
		else if (_sections->_typeShortCut == Sections3D::SHORTCUT_Z)
		{
			out << "\tplane {z, " << _sections->_shortCutBase << " inverse}\n";
		}
		else if (_sections->_typeShortCut == Sections3D::SHORTCUT_Z_INVERSE)
		{
			out << "\tplane {z, " << _sections->_shortCutBase << "}\n";
		}
		else
		{
			std::cout << "ERROR (CameraManager::createPovRayFile)" << std::endl;
		}
	}
	else
	{
		if (_sectionsEnabled)
		{
			if (_sections->_typeSections == Sections3D::SECTIONS_RECTANGULAR)
			{
				out << "\tplane {z, " << _sections->_zPlaneMinPos-1 << "}\n";
				out << "\tplane {z, " << _sections->_zPlaneMaxPos-1 << " inverse}\n";
				out << "\tplane {y, " << _sections->_yPlaneMinPos-1 << "}\n";
				out << "\tplane {y, " << _sections->_yPlaneMaxPos-1 << " inverse}\n";
				out << "\tplane {x, " << _sections->_xPlaneMinPos-1 << "}\n";
				out << "\tplane {x, " << _sections->_xPlaneMaxPos-1 << " inverse}\n";
			}
			else if (_sections->_typeSections == Sections3D::SECTIONS_XY)
			{
				bool greaterThan180 = std::abs(_sections->_angleMax - _sections->_angleMin) > 180;
				if (!greaterThan180)
				{
				out << "\tplane {y, 0\n";
				out << "\t\trotate <0,0," << _sections->_angleMin << "> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
				out << "\t}\n";
				out << "\tplane {-y, 0\n";
				out << "\t\trotate <0,0," << _sections->_angleMax << "> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
				out << "\t }\n";
				}
				out << "\tcylinder {\n";
				out << "\t<0,0," << -_sections->_height/2.0 << ">, <0,0," << _sections->_height/2.0 << ">, " << _sections->_radius << " translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << "> inverse}\n";
				
				out << "\t\n";
				if (greaterThan180)
				{
					out << "\tclipped_by {\n";
					out << "\tunion {\n";
					out << "\tplane {-y, 0\n";
					out << "\t\trotate <0,0," << _sections->_angleMin << "> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t }\n";
					out << "\tplane {y, 0\n";
					out << "\t\trotate <0,0," << _sections->_angleMax << "> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t }\n";
					out << "\t }\n";
					out << "\t }\n";
				}
				
			}
			else if (_sections->_typeSections == Sections3D::SECTIONS_YZ)
			{
				bool greaterThan180 = std::abs(_sections->_angleMax - _sections->_angleMin) > 180;
				if (!greaterThan180)
				{
				out << "\tplane {z, 0\n";
				out << "\t\trotate <" << _sections->_angleMin << ",0,0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
				out << "\t}\n";
				out << "\tplane {-z, 0\n";
				out << "\t\trotate <" << _sections->_angleMax << ",0,0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
				out << "\t}\n";
				}
				out << "\tcylinder {\n";
				out << "\t<" << -_sections->_height/2.0 << ",0,0>, <" << _sections->_height/2.0 << ",0,0>, " << _sections->_radius << " translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << "> inverse}\n";
				
				out << "\t\n";
				if (greaterThan180)
				{
					out << "\tclipped_by {\n";
					out << "\tunion {\n";
					out << "\tplane {-z, 0\n";
					out << "\t\trotate <" << _sections->_angleMin << ",0,0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t }\n";
					out << "\tplane {z, 0\n";
					out << "\t\trotate <" << _sections->_angleMax << ",0,0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t }\n";
					out << "\t }\n";
					out << "\t }\n";
				}
			}
			else if (_sections->_typeSections == Sections3D::SECTIONS_XZ)
			{
				bool greaterThan180 = std::abs(_sections->_angleMax - _sections->_angleMin) > 180;
				if (!greaterThan180)
				{
					out << "\tplane {x, 0\n";
					out << "\t\trotate <0," << _sections->_angleMin << ",0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t}\n";
					out << "\tplane {-x, 0\n";
					out << "\t\trotate <0," << _sections->_angleMax << ",0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t}\n";		
				}
				out << "\tcylinder {\n";
				out << "\t<0," << -_sections->_height/2.0 << ",0>, <0," << _sections->_height/2.0 << ",0>, " << _sections->_radius << " translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << "> inverse}\n";
				
				out << "\t\n";
				if (greaterThan180)
				{
					out << "\tclipped_by {\n";
					out << "\tunion {\n";
					out << "\tplane {-x, 0\n";
					out << "\t\trotate <0," << _sections->_angleMin << ",0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t }\n";
					out << "\tplane {x, 0\n";
					out << "\t\trotate <0," << _sections->_angleMax << ",0> translate <" << _sections->_strafeX << "," << _sections->_strafeY << "," << _sections->_strafeZ << ">\n";
					out << "\t }\n";
					out << "\t }\n";
					out << "\t }\n";
				}
			}
		
		}
	}
	
	out << "\n";
	out << "\tcutaway_textures\n";
	out << "} \n";
	
	file.close();

	QFile file2(outputPath + "camera.pov");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;

    QTextStream out2(&file2);
	out2 << "// POVRAY file created by MCNPXVisualizer\n";
	out2 << "\n";
	
	// Writes the camera object to file
	if (_cameraString == "")
	{
		out2 << "camera\n";
		out2 << "{   \n";   
		if (_useOrthographicProjection)
			out << "\torthographic\n";
		out2 << "\tlook_at <" << _camera->_camStrafeX << "," << _camera->_camStrafeY << "," << _camera->_camStrafeZ << ">\n";
		out2 << "\tlocation <" << _camera->_camPosX + _camera->_camStrafeX << "," << _camera->_camPosY + _camera->_camStrafeY << "," << _camera->_camPosZ + _camera->_camStrafeZ << "> \n";   
		out2 << "\tright  <-4/3,0,0>  \n"; 
		out2 << "\tangle 45.0\n";
		
		out2 << "}\n";
	}
	else
	{
		out2 << _cameraString;
	}

	file2.close();

	// Put the light object at the same position as the camera
	QFile file3(outputPath + "lights.pov");
    if (!file3.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;

    QTextStream out3(&file3);
	out3 << "// POVRAY file created by MCNPXVisualizer\n";
	out3 << "\n";
	if (_lightString == "")
	{
		out3 << "light_source\n";
		out3 << "{\n";
		  out3 << "\t<" << _camera->_camPosX << "," << _camera->_camPosY << "," << _camera->_camPosZ << "> \n";   
		  out3 << "\tcolor White\n";
		out3 << "}\n";
	}
	else
	{
		out3 << _lightString;
	}

	file3.close();
    return 1;
}


template<> CameraManager* Singleton<CameraManager>::ms_Singleton = 0;
CameraManager* CameraManager::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
CameraManager& CameraManager::getSingleton(void)
	{  
		return ( *ms_Singleton );  
	}
