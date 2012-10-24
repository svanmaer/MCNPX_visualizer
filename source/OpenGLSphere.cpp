//#########################################################################################################
//## OpenGLSphere.cpp
//#########################################################################################################
//##
//## OpenGL Sphere Object
//## Inherited from OpenGLObject
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "OpenGLSphere.h"

// ==> OpenGLBox()
// Constructor
//--------------------------------------------------------------------
OpenGLSphere::OpenGLSphere() : OpenGLObject()
{
	_xMin = 0.0;
	_xMax = 0.0;
	_yMin = 0.0;
	_yMax = 0.0;
	_zMin = 0.0;
	_zMax = 0.0;
}

// ==> ~OpenGLBox()
// Destructor
//--------------------------------------------------------------------
OpenGLSphere::~OpenGLSphere()
{
}

// ==> draw()
// Draws the OpenGL box (polymorfism for the toplevel OpenGLObject)
//--------------------------------------------------------------------
void OpenGLSphere::draw()
{
	glColor3f(220.0/255.0, 136.0/255.0, 55.0/255.0);
	
	glPushMatrix();
	glTranslatef(_posX, _posY, _posZ);
	
	GLUquadricObj *quadraticSph =gluNewQuadric();
	gluSphere(quadraticSph, _rad, 64, 64);


	glPopMatrix();

	
	gluDeleteQuadric(quadraticSph);
}

// ==> createBox(items)
//   Create the box based on input coordinates
//--------------------------------------------------------------------
void OpenGLSphere::createSphere(QStringList items)
{
	QString direction;

	_posX = items.at(0).toFloat();
	_posY = items.at(1).toFloat();
	_posZ = items.at(2).toFloat();
	_rad = items.at(3).toFloat();

	_xMin = _posX - _rad;
	_xMax = _posX + _rad;
	_yMin = _posY - _rad;
	_yMax = _posY + _rad;
	_zMin = _posZ - _rad;
	_zMax = _posZ + _rad;
}	

