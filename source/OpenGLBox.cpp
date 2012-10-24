//#########################################################################################################
//## OpenGLBox.cpp
//#########################################################################################################
//##
//## OpenGL Box Object
//## Inherited from OpenGLObject
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#include "OpenGLBox.h"

// ==> OpenGLBox()
// Constructor
//--------------------------------------------------------------------
OpenGLBox::OpenGLBox() : OpenGLObject()
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
OpenGLBox::~OpenGLBox()
{
}

// ==> draw()
// Draws the OpenGL box (polymorfism for the toplevel OpenGLObject)
//--------------------------------------------------------------------
void OpenGLBox::draw()
{
	glColor3f(220.0/255.0, 136.0/255.0, 55.0/255.0);
	
	glPushMatrix();
	glBegin(GL_QUADS);
		// Bottom Face
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(_xMin, _yMin, _zMin);	// Top Right Of The Texture and Quad
		glVertex3f( _xMax, _yMin, _zMin);	// Top Left Of The Texture and Quad
		glVertex3f( _xMax, _yMin,  _zMax);	// Bottom Left Of The Texture and Quad
		glVertex3f(_xMin, _yMin,  _zMax);	// Bottom Right Of The Texture and Quad
		
		// Top Face
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(_xMin, _yMax, _zMin);	// Top Right Of The Texture and Quad
		glVertex3f( _xMax, _yMax, _zMin);	// Top Left Of The Texture and Quad
		glVertex3f( _xMax, _yMax,  _zMax);	// Bottom Left Of The Texture and Quad
		glVertex3f(_xMin, _yMax,  _zMax);	// Bottom Right Of The Texture and Quad

		// Front Face
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(_xMin, _yMin,  _zMax);	// Bottom Left Of The Texture and Quad
		glVertex3f( _xMax, _yMin,  _zMax);	// Bottom Right Of The Texture and Quad
		glVertex3f( _xMax,  _yMax,  _zMax);	// Top Right Of The Texture and Quad
		glVertex3f(_xMin,  _yMax,  _zMax);	// Top Left Of The Texture and Quad

		// Back Face
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(_xMin, _yMin, _zMin);	// Bottom Right Of The Texture and Quad
		glVertex3f(_xMin,  _yMax, _zMin);	// Top Right Of The Texture and Quad
		glVertex3f( _xMax,  _yMax, _zMin);	// Top Left Of The Texture and Quad
		glVertex3f( _xMax, _yMin, _zMin);	// Bottom Left Of The Texture and Quad

		// Right face
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f( _xMax, _yMin, _zMin);	// Bottom Right Of The Texture and Quad
		glVertex3f( _xMax,  _yMax, _zMin);	// Top Right Of The Texture and Quad
		glVertex3f( _xMax,  _yMax,  _zMax);	// Top Left Of The Texture and Quad
		glVertex3f( _xMax, _yMin,  _zMax);	// Bottom Left Of The Texture and Quad

		// Left Face
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(_xMin, _yMin, _zMin);	// Bottom Left Of The Texture and Quad
		glVertex3f(_xMin, _yMin,  _zMax);	// Bottom Right Of The Texture and Quad
		glVertex3f(_xMin,  _yMax,  _zMax);	// Top Right Of The Texture and Quad
		glVertex3f(_xMin,  _yMax, _zMin);	// Top Left Of The Texture and Quad

	glEnd();
	glPopMatrix;
}

// ==> createBox(items)
//   Create the box based on input coordinates
//--------------------------------------------------------------------
void OpenGLBox::createBox(QStringList items)
{
	QString direction;

	_xMin = items.at(0).toFloat();
	_xMax = items.at(3).toFloat();
	_yMin = items.at(1).toFloat();
	_yMax = items.at(4).toFloat();
	_zMin = items.at(2).toFloat();
	_zMax = items.at(5).toFloat();

	float dummy;
	if (_xMin > _xMax)
	{
		dummy = _xMin;
		_xMin = _xMax;
		_xMax = dummy;
	}
	if (_yMin > _yMax)
	{
		dummy = _yMin;
		_yMin = _yMax;
		_yMax = dummy;
	}
	if (_zMin > _zMax)
	{
		dummy = _zMin;
		_zMin = _zMax;
		_zMax = dummy;
	}
}

