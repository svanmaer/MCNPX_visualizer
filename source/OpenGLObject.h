//#########################################################################################################
//## OpenGLObject.h
//#########################################################################################################
//##
//## Abstract class for drawing OpenGL Objects
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef OPENGLOBJECT_H
#define OPENGLOBJECT_H

class OpenGLObject
{  
	public:
		OpenGLObject(){}
		virtual ~OpenGLObject(){}

		virtual void draw() = 0; 

		virtual float getXMin() const = 0;
		virtual float getXMax() const = 0;
		virtual float getYMin() const = 0;
		virtual float getYMax() const = 0;
		virtual float getZMin() const = 0;
		virtual float getZMax() const = 0;
};


#endif