//#########################################################################################################
//## Camera.h
//#########################################################################################################
//##
//## Contains position, look at and strafe of a camera
//## => used for placement of the povray camera as well as the placement of the OpenGL Camera 
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef CAMERA_H
#define CAMERA_H
#include <QObject>

struct Camera : public QObject
{
	Q_OBJECT
	public:
		Camera()
		{
			_camPosX = 0.0;
			_camPosY = 0.0;
			_camPosZ = -140.0;
			_camLookAtX = 0.0;
			_camLookAtY = 0.0;
			_camLookAtZ = 0.0;
			_camStrafeX = 0.0;
			_camStrafeY = 0.0;
			_camStrafeZ = 0.0;
		}
		~Camera(){}

	public slots:
		void onCameraPositionChanged(float posX, float posY, float posZ)
		{
			_camPosX = posX;
			_camPosY = posY;
			_camPosZ = posZ;
		}

		void onCameraLookAtChanged(float lookAtX, float lookAtY, float lookAtZ)
		{
			_camLookAtX = lookAtX;
			_camLookAtY = lookAtY;
			_camLookAtZ = lookAtZ;
		}

		void onCameraStrafeChanged(float strafeX, float strafeY, float strafeZ)
		{
			_camStrafeX = strafeX;
			_camStrafeY = strafeY;
			_camStrafeZ = strafeZ;
		}

	public:
		float _camPosX;
		float _camPosY;
		float _camPosZ;
		float _camLookAtX;
		float _camLookAtY;
		float _camLookAtZ;
		float _camStrafeX;
		float _camStrafeY;
		float _camStrafeZ;
};

#endif