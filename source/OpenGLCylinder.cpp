//#########################################################################################################
//## OpenGLCylinder.cpp
//#########################################################################################################
//##
//## OpenGL Cylinder Object
//## Inherited from OpenGLObject
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "OpenGLCylinder.h"


// ==> OpenGLCylinder()
// Constructor
//--------------------------------------------------------------------
OpenGLCylinder::OpenGLCylinder() : OpenGLObject()
{
	_radius = 0.0;
	_height = 0.0;
	_min = 0.0;
	_max = 0.0;
	_direction = -1;
}

// ==> ~OpenGLCylinder()
// Destructor
//--------------------------------------------------------------------
OpenGLCylinder::~OpenGLCylinder()
{
}


// ==> draw()
// Draws the OpenGLCylinder box (polymorfism for the toplevel OpenGLObject)
//--------------------------------------------------------------------
void OpenGLCylinder::draw()
{
	//glDisable(GL_LIGHTING);
	glColor3f(220.0/255.0, 136.0/255.0, 55.0/255.0);
	//glEnable (GL_BLEND);

	//glScalef(0.2, 0.2, 0.2);
	
	if (_direction == DIRECTIONX)
	{
		glTranslatef(_min, 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	}
	else if (_direction == DIRECTIONY)
	{
		glTranslatef(0.0f, _min, 0.0f);
		glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
	}
	else if (_direction == DIRECTIONZ)
	{
		glTranslatef(0.0, 0.0, _min);
	}
	else
		return;
	
	GLUquadricObj *quadraticCyl =gluNewQuadric();
	gluCylinder(quadraticCyl, _radius, _radius, _height, 64, 64);
	//gluDisk( quadraticCyl, 0.0, _radius, 32, 1);
	//glPushMatrix();
	//glTranslatef(0.0, 0.0, _height);
	//glPopMatrix();


	


	gluDeleteQuadric(quadraticCyl);
	//glDisable (GL_BLEND);
}

// ==> createCylinder(items)
//   Create the cylinder based on input information
//--------------------------------------------------------------------
bool OpenGLCylinder::createCylinder(QStringList items)
{
	QString direction = items.at(0);

	if (direction == "X" || direction == "x")
	{
		_direction = DIRECTIONX;
		_radius = items.at(7).toFloat();
		_min = items.at(1).toFloat();
		_max = items.at(4).toFloat();
		_height = abs(_max-_min);
	}
	else if (direction == "Y" || direction == "y")
	{
		_direction = DIRECTIONY;
		_radius = items.at(7).toFloat();
		_min = items.at(2).toFloat();
		_max = items.at(5).toFloat();
		_height = abs(_max-_min);
	}
	else if(direction == "Z" || direction == "z")
	{
		_direction = DIRECTIONZ;
		_radius = items.at(7).toFloat();
		_min = items.at(3).toFloat();
		_max = items.at(6).toFloat();
		_height = abs(_max-_min);
	}
	else
		return false;

	return true;
}


float OpenGLCylinder::getXMin() const 
{
	if (_direction == DIRECTIONX )
	{
		return _min;
	}
	else if (_direction == DIRECTIONZ || _direction == DIRECTIONY)
	{
		return -_radius; 
	}
	else
		return 0.0;	
}

float OpenGLCylinder::getXMax() const 
{
	if (_direction == DIRECTIONX )
	{
		return _max;
	}
	else if (_direction == DIRECTIONZ || _direction == DIRECTIONY)
	{
		return _radius; 
	}
	else
		return 0.0;	
}

float OpenGLCylinder::getYMin() const
{
	if (_direction == DIRECTIONY )
	{
		return _min;
	}
	else if (_direction == DIRECTIONZ || _direction == DIRECTIONX)
	{
		return -_radius; 
	}
	else
		return 0.0;	
}

float OpenGLCylinder::getYMax() const 
{
	if (_direction == DIRECTIONY )
	{
		return _max;
	}
	else if (_direction == DIRECTIONZ || _direction == DIRECTIONX)
	{
		return _radius; 
	}
	else
		return 0.0;	
}

float OpenGLCylinder::getZMin() const 
{
	if (_direction == DIRECTIONZ )
	{
		return _min;
	}
	else if (_direction == DIRECTIONX || _direction == DIRECTIONY)
	{
		return -_radius; 
	}
	else
		return 0.0;	
}

float OpenGLCylinder::getZMax() const 
{
	if (_direction == DIRECTIONZ )
	{
		return _max;
	}
	else if (_direction == DIRECTIONX || _direction == DIRECTIONY)
	{
		return _radius; 
	}
	else
		return 0.0;	
}
