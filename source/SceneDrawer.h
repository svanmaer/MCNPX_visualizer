//#######################################################################################################################
//## SceneDrawer.h
//#######################################################################################################################
//##
//## OpenGL widget to render the inner and outer case of the MXNPX file together with the section planes
//## The user can rotate the object with the mouse and keyboard input
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#######################################################################################################################

#include <QGLWidget>
#include <QString>
#include <QTimer>

#include "OpenGLObject.h"
#include "Sections3D.h"

class SceneDrawer : public QGLWidget
{
	Q_OBJECT

	public:
		SceneDrawer(QWidget *parent);
		virtual ~SceneDrawer(){};

		void setAzimuth(float azimuth);
		void setElevation(float elevation);
		void setDistance(float distance);

		float getAzimuth(){ return _azimuth; }
		float getElevation(){ return _elevation; }
		float getDistance(){ return _distance; }

		void setZPlaneMin(float pos);
		void setZPlaneMax(float pos);
		void setXPlaneMin(float pos);
		void setXPlaneMax(float pos);
		void setYPlaneMin(float pos);
		void setYPlaneMax(float pos);

		float getZPlaneMin(){ return _zPlaneMin; }
		float getZPlaneMax(){ return _zPlaneMax; }
		float getXPlaneMin(){ return _xPlaneMin; }
		float getXPlaneMax(){ return _xPlaneMax; }
		float getYPlaneMin(){ return _yPlaneMin; }
		float getYPlaneMax(){ return _yPlaneMax; }

		void setStrafeX( float strafe ){ _camStrafeX = strafe; this->updateCamera(); }
		void setStrafeY( float strafe ){ _camStrafeY = strafe; this->updateCamera(); }
		void setStrafeZ( float strafe ){ _camStrafeZ = strafe; this->updateCamera(); }

		float getStrafeX(){ return _camStrafeX; }
		float getStrafeY(){ return _camStrafeY; }
		float getStrafeZ(){ return _camStrafeZ; }

		void setSectionsEnabled(bool enabled){ _sectionsEnabled = enabled; emit informationChanged();}
		void setLimitedPlanes(bool enabled){ _limitedPlanes = enabled; emit informationChanged();}
		bool getLimitedPlanes(){ return _limitedPlanes; }
		bool getSectionsEnabled() { return _sectionsEnabled; }

		void updateCamera();

		void mousePressEvent(QMouseEvent* e);
		void mouseMoveEvent(QMouseEvent* e);
		void mouseReleaseEvent(QMouseEvent* e);
		void wheelEvent(QWheelEvent *e);

		void addScene(QString typeScene, QStringList data);
		void clearScene();

		Sections3D* getSections(){ return _sections; }
		

	protected:
		GLvoid   initializeGL();
		GLvoid   resizeGL(int width, int height);
		GLvoid   paintGL();

		int selection(int mouseX, int mouseY);	// Pick on position (mouseX, mouseY)

		Sections3D* _sections;

		bool _sectionsEnabled;		// render sectiosn or not
		bool _limitedPlanes;		// sections are limited by the OpenGL bounding box or not

		bool _alt;					// Alt pressed
		bool _ctrl;					// Ctrl pressed

		
		float _verticalOffset;		// Offset between the vertical mouse position between two frames
		float _horizontalOffset;	// Offset between the horizontal mouse position between two frames

		float _sensitivity;			// Sensitivity of the mouse

		// 2D section planes
		float _xPlaneMin;			
		float _xPlaneMax;
		float _yPlaneMin;
		float _yPlaneMax;
		float _zPlaneMin;
		float _zPlaneMax;

		// Positions of the camera
		float _camPosX;
		float _camPosY;
		float _camPosZ;
		float _camLookX;
		float _camLookY;
		float _camLookZ;

		// Strafe of the camera
		float _camStrafeX;
		float _camStrafeY;
		float _camStrafeZ;

		// Polar coordinates for camera around the object
		float _azimuth;
		float _elevation;
		float _distance;

		// Timer for the frame updater
		QTimer *_frameUpdateTimer;

		std::vector<OpenGLObject*> _objects;


	private slots:
		void slotOnFrameUpdate();	// draw new frame

	signals:
		void cameraPositionChanged(float posX, float posY, float posZ);			// emits when the position of the camera is changed
		void cameraLookAtChanged(float lookAtX, float lookAtY, float lookAtZ);	// emits when the lookat of the camera is changed
		void cameraStrafeChanged(float strafeX, float strafeY, float strafeZ);	// emits when the strafe of the camera is changed
		void statusChanged(QString status, int timeout);						// emits when there needs to be information printed to the status bar
		void informationChanged();												// emits when there is an important change of internal information
 };
