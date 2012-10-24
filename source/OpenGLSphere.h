//#########################################################################################################
//## OpenGLSphere.h
//#########################################################################################################
//##
//## OpenGL Sphere Object
//## Inherited from OpenGLObject
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef OPENGL_SPHERE_H
#define OPENGL_SPHERE_H

#include "OpenGLObject.h"

#include <QString>
#include <QStringList>
#include <QGLWidget>


class OpenGLSphere : public OpenGLObject
{  
	public:
		OpenGLSphere();
		virtual ~OpenGLSphere();

		void createSphere(QStringList items);
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

		float _posX;
		float _posY;
		float _posZ;
		float _rad;
};


#endif