//#########################################################################################################
//## OpenGLBox.h
//#########################################################################################################
//##
//## OpenGL Box Object
//## Inherited from OpenGLObject
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef OPENGL_BOX_H
#define OPENGL_BOX_H

#include "OpenGLObject.h"

#include <QString>
#include <QStringList>
#include <QGLWidget>


class OpenGLBox : public OpenGLObject
{  
	public:
		OpenGLBox();
		virtual ~OpenGLBox();

		void createBox(QStringList items);
		virtual void draw(); // Draws the box
		
		virtual float getXMin() const { return _xMin; }
		virtual float getXMax() const { return _xMax; }
		virtual float getYMin() const { return _yMin; }
		virtual float getYMax() const { return _yMax; }
		virtual float getZMin() const { return _zMin; }
		virtual float getZMax() const { return _zMax; }

	protected:
		float _xMin;
		float _xMax;
		float _yMin;
		float _yMax;
		float _zMin;
		float _zMax;
};


#endif