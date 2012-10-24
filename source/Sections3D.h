//#########################################################################################################
//## Sections3D.h
//#########################################################################################################
//##
//## Contains all the information for defining 2D or 3D sections
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef SECTIONS_3D_H
#define SECTIONS_3D_H

#include <QObject>

struct Sections3D : public QObject
{
	Q_OBJECT
	public:
		Sections3D()
		{
			_xPlaneMinPos = 0.0f;
			_xPlaneMaxPos = 0.0f;
			_yPlaneMinPos = 0.0f;
			_yPlaneMaxPos = 0.0f;
			_zPlaneMinPos = 0.0f;
			_zPlaneMaxPos = 0.0f;

			_typeSections = Sections3D::SECTIONS_RECTANGULAR;

			 _height = 100;
			_angleMin = 0;
			_angleMax = 45;
			_radius = 100;

			_strafeX = 0.0;
			_strafeY = 0.0;
			_strafeZ = 0.0;

			_shortCutBase = 0.0;

			_useShortCut = false;
		}
		~Sections3D(){}

		// Type of the 3D section (3D cutout or pie piece)
		enum {SECTIONS_RECTANGULAR, SECTIONS_XY, SECTIONS_YZ, SECTIONS_XZ};

		// Type of the shortcut
		enum {SHORTCUT_X, SHORTCUT_X_INVERSE,  SHORTCUT_Y, SHORTCUT_Y_INVERSE, SHORTCUT_Z, SHORTCUT_Z_INVERSE};

		void setTypeSections(int type){_typeSections = type; }
		void setShortCut(int type) {_typeShortCut = type; }

	public slots:
		void onZPlaneMinChanged(float pos)
		{
			_zPlaneMinPos = pos;
		}

		void onZPlaneMaxChanged(float pos)
		{
			_zPlaneMaxPos = pos;
		}
		void onXPlaneMinChanged(float pos)
		{
			_xPlaneMinPos = pos;
		}

		void onXPlaneMaxChanged(float pos)
		{
			_xPlaneMaxPos = pos;
		}

		void onYPlaneMinChanged(float pos)
		{
			_yPlaneMinPos = pos;
		}

		void onYPlaneMaxChanged(float pos)
		{
			_yPlaneMaxPos = pos;
		}

		void angleMinChanged(float angle)
		{
			_angleMin = angle;
		}

		void angleMaxChanged(float angle)
		{
			_angleMax = angle;
		}

		void heightChanged(float height)
		{
			_height = height;
		}

		void radiusChanged(float radius)
		{
			_radius = radius;
		}

		void strafeXChanged(float strafe)
		{
			_strafeX = strafe;
		}

		void strafeYChanged(float strafe)
		{
			_strafeY = strafe;
		}

		void strafeZChanged(float strafe)
		{
			_strafeZ = strafe;
		}

	public:
		 float _xPlaneMinPos;	// min x plane of bounding box
		 float _xPlaneMaxPos;	// max x plane of bounding box
		 float _yPlaneMinPos;	// min y plane of bounding box
		 float _yPlaneMaxPos;	// max y plane of bounding box
		 float _zPlaneMinPos;	// min z plane of bounding box
		 float _zPlaneMaxPos;	// max z plane of bounding box

		 float _height;			// pie piece height
		 float _angleMin;		// pie piece angle 1
		 float _angleMax;		// pie piece angle 2
		 float _radius;			// pie piece radius

		 float _strafeX;		// strafe X of the pie piece
		 float _strafeY;		// strafe Y of the pie piece
		 float _strafeZ;		// strafe Z of the pie piece

		 bool _useShortCut;		// use of a shortcut
		 int _typeShortCut;		// type of the shortcut
		 float _shortCutBase;	// plane of the shortcut

		 int _typeSections;		// type of the cross section
};

#endif
