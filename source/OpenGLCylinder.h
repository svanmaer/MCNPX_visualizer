//#########################################################################################################
//## OpenGLCylinder.h
//#########################################################################################################
//##
//## OpenGL Cylinder Object
//## Inherited from OpenGLObject
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef OPENGL_CYLINDER_H
#define OPENGL_CYLINDER_H

#include <QString>
#include <QStringList>
#include <QGLWidget>

#include "OpenGLObject.h"

#define DIRECTIONX 0
#define DIRECTIONY 1
#define DIRECTIONZ 2

// TODO: in beide richtingen
class OpenGLCylinder : public OpenGLObject
{  
	public:
		OpenGLCylinder();
		virtual ~OpenGLCylinder();

		//enum DIRECTION{X, Y, Z};

		bool createCylinder(QStringList items);
		virtual void draw();

		virtual float getXMin() const;
		virtual float getXMax() const;
		virtual float getYMin() const;
		virtual float getYMax() const;
		virtual float getZMin() const;
		virtual float getZMax() const;

		float getRadius() const { return _radius; }
		float getHeight() const { return _height; }

	protected:
		int _direction;
		float _radius;
		float _height;
		float _min;
		float _max;

};


#endif
