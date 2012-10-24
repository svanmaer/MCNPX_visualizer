//#######################################################################################################################
//## SceneDrawer.cpp
//#######################################################################################################################
//##
//## OpenGL widget to render the inner and outer case of the MXNPX file together with the section planes
//## The user can rotate the object with the mouse and keyboard input
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#######################################################################################################################

#include "SceneDrawer.h"
#include "OpenGLCylinder.h"
#include "OpenGLBox.h"
#include "OpenGLSphere.h"
#include "CameraManager.h"

#define PI 3.14159265

#include <QWheelEvent>
#include <iostream>
#include <math.h>

// ==> SceneDrawer(parent)
// Constructor
//--------------------------------------------------------------------
SceneDrawer::SceneDrawer(QWidget *parent)
         : QGLWidget(parent)
{
	// Create and connect the frame updater
	_frameUpdateTimer = new QTimer();
	connect( _frameUpdateTimer, SIGNAL(timeout()), this, SLOT(slotOnFrameUpdate()) );
	_frameUpdateTimer->start(100);

	// Get the right sections information from the camera manager
	_sections = CameraManager::getSingletonPtr()->getSections();
	_sectionsEnabled = true;
	_limitedPlanes = true;
	_sections->_typeSections = Sections3D::SECTIONS_RECTANGULAR;

	_alt = false;
	_ctrl = false;

	_camPosX = 0.0f;
	_camPosY = 0.0f;
	_camPosZ = 0.0f;
	_camLookX = 0.0f;
	_camLookY = 0.0f;
	_camLookZ = 0.0f;

	_camStrafeX = 0.0f;
	_camStrafeY = 0.0f;
	_camStrafeZ = 0.0f;

	_verticalOffset = 0.0;
	_horizontalOffset = 0.0;

	_azimuth = 45.0;
	_elevation = 90.0;
	_distance = 20.0;

	_sensitivity = 0.25;

	_xPlaneMin = 0.0f;
	_xPlaneMax = 0.0f;
	_yPlaneMin = 0.0f;
	_yPlaneMax = 0.0f;
	_zPlaneMin = 0.0f;
	_zPlaneMax = 0.0f;

	_sections->_xPlaneMinPos = 0.0f;
	_sections->_xPlaneMaxPos = 0.0f;
	_sections->_yPlaneMinPos = 0.0f;
	_sections->_yPlaneMaxPos = 0.0f;
	_sections->_zPlaneMinPos = 0.0f;
	_sections->_zPlaneMaxPos = 0.0f;
}

// ==> addScene(typeScene, data)
//	Add an OpenGL Object to the scene
//		Type can be CYLINDER or BOX
//		Data will be parsed based on the type of the object and the object will be added to the scene
//--------------------------------------------------------------------
void SceneDrawer::addScene(QString typeScene, QStringList data)
{
	if (typeScene == "CYLINDER")
	{
		OpenGLCylinder* cyl = new OpenGLCylinder();
		if (!cyl->createCylinder(data))
		{
			delete cyl;
			return;
		}
		this->_objects.push_back(cyl);

		this->setDistance(sqrt(cyl->getRadius()*cyl->getRadius() + cyl->getHeight()*cyl->getHeight())*2);

		if (_objects.size() > 1)
		{
			if (cyl->getXMin()-1 < _xPlaneMin)
				_xPlaneMin = cyl->getXMin()-1;
			if (cyl->getXMax()+1 > _xPlaneMax)
				_xPlaneMax = cyl->getXMax()+1;

			if (cyl->getYMin()-1 < _yPlaneMin)
				_yPlaneMin = cyl->getYMin()-1;
			if (cyl->getYMax()+1 > _yPlaneMax)
				_yPlaneMax = cyl->getYMax()+1;

			if (cyl->getZMin()-1 < _zPlaneMin)
				_zPlaneMin = cyl->getZMin()-1;
			if (cyl->getZMax()+1 > _zPlaneMax)
				_zPlaneMax = cyl->getZMax()+1;

			_sections->_xPlaneMinPos = _xPlaneMin;
			_sections->_xPlaneMaxPos = _xPlaneMax;
			_sections->_yPlaneMinPos = _yPlaneMin;
			_sections->_yPlaneMaxPos = _yPlaneMax;
			_sections->_zPlaneMinPos = _zPlaneMin;
			_sections->_zPlaneMaxPos = _zPlaneMax;
		}
		else
		{

			_xPlaneMin = cyl->getXMin()-1;
			_xPlaneMax = cyl->getXMax()+1;
			_yPlaneMin = cyl->getYMin()-1;
			_yPlaneMax = cyl->getYMax()+1;
			_zPlaneMin = cyl->getZMin()-1;
			_zPlaneMax = cyl->getZMax()+1;

			_sections->_xPlaneMinPos = _xPlaneMin;
			_sections->_xPlaneMaxPos = _xPlaneMax;
			_sections->_yPlaneMinPos = _yPlaneMin;
			_sections->_yPlaneMaxPos = _yPlaneMax;
			_sections->_zPlaneMinPos = _zPlaneMin;
			_sections->_zPlaneMaxPos = _zPlaneMax;
		}



		this->setDistance(2* sqrt((_xPlaneMax-_xPlaneMin)*(_xPlaneMax-_xPlaneMin) + (_yPlaneMax-_yPlaneMin)*(_yPlaneMax-_yPlaneMin) + (_zPlaneMax-_zPlaneMin)*(_zPlaneMax-_zPlaneMin)));

	}
	if (typeScene == "BOX")
	{
		OpenGLBox* box = new OpenGLBox();
		box->createBox(data);
		this->_objects.push_back(box);

		if (_objects.size() > 1)
		{
			if (box->getXMin()-1 < _xPlaneMin)
				_xPlaneMin = box->getXMin()-1;
			if (box->getXMax()+1 > _xPlaneMax)
				_xPlaneMax = box->getXMax()+1;

			if (box->getYMin()-1 < _yPlaneMin)
				_yPlaneMin = box->getYMin()-1;
			if (box->getYMax()+1 > _yPlaneMax)
				_yPlaneMax = box->getYMax()+1;

			if (box->getZMin()-1 < _zPlaneMin)
				_zPlaneMin = box->getZMin()-1;
			if (box->getZMax()+1 > _zPlaneMax)
				_zPlaneMax = box->getZMax()+1;

			_sections->_xPlaneMinPos = _xPlaneMin;
			_sections->_xPlaneMaxPos = _xPlaneMax;
			_sections->_yPlaneMinPos = _yPlaneMin;
			_sections->_yPlaneMaxPos = _yPlaneMax;
			_sections->_zPlaneMinPos = _zPlaneMin;
			_sections->_zPlaneMaxPos = _zPlaneMax;
		}
		else
		{

			_xPlaneMin = box->getXMin()-1;
			_xPlaneMax = box->getXMax()+1;
			_yPlaneMin = box->getYMin()-1;
			_yPlaneMax = box->getYMax()+1;
			_zPlaneMin = box->getZMin()-1;
			_zPlaneMax = box->getZMax()+1;

			_sections->_xPlaneMinPos = _xPlaneMin;
			_sections->_xPlaneMaxPos = _xPlaneMax;
			_sections->_yPlaneMinPos = _yPlaneMin;
			_sections->_yPlaneMaxPos = _yPlaneMax;
			_sections->_zPlaneMinPos = _zPlaneMin;
			_sections->_zPlaneMaxPos = _zPlaneMax;
		}

		this->setDistance(2* sqrt((_xPlaneMax-_xPlaneMin)*(_xPlaneMax-_xPlaneMin) + (_yPlaneMax-_yPlaneMin)*(_yPlaneMax-_yPlaneMin) + (_zPlaneMax-_zPlaneMin)*(_zPlaneMax-_zPlaneMin)));
	}

	if (typeScene == "SPHERE")
	{
		OpenGLSphere* sphere = new OpenGLSphere();
		sphere->createSphere(data);
		this->_objects.push_back(sphere);

		if (_objects.size() > 1)
		{
			if (sphere->getXMin()-1 < _xPlaneMin)
				_xPlaneMin = sphere->getXMin()-1;
			if (sphere->getXMax()+1 > _xPlaneMax)
				_xPlaneMax = sphere->getXMax()+1;

			if (sphere->getYMin()-1 < _yPlaneMin)
				_yPlaneMin = sphere->getYMin()-1;
			if (sphere->getYMax()+1 > _yPlaneMax)
				_yPlaneMax = sphere->getYMax()+1;

			if (sphere->getZMin()-1 < _zPlaneMin)
				_zPlaneMin = sphere->getZMin()-1;
			if (sphere->getZMax()+1 > _zPlaneMax)
				_zPlaneMax = sphere->getZMax()+1;

			_sections->_xPlaneMinPos = _xPlaneMin;
			_sections->_xPlaneMaxPos = _xPlaneMax;
			_sections->_yPlaneMinPos = _yPlaneMin;
			_sections->_yPlaneMaxPos = _yPlaneMax;
			_sections->_zPlaneMinPos = _zPlaneMin;
			_sections->_zPlaneMaxPos = _zPlaneMax;
		}
		else
		{

			_xPlaneMin = sphere->getXMin()-1;
			_xPlaneMax = sphere->getXMax()+1;
			_yPlaneMin = sphere->getYMin()-1;
			_yPlaneMax = sphere->getYMax()+1;
			_zPlaneMin = sphere->getZMin()-1;
			_zPlaneMax = sphere->getZMax()+1;

			_sections->_xPlaneMinPos = _xPlaneMin;
			_sections->_xPlaneMaxPos = _xPlaneMax;
			_sections->_yPlaneMinPos = _yPlaneMin;
			_sections->_yPlaneMaxPos = _yPlaneMax;
			_sections->_zPlaneMinPos = _zPlaneMin;
			_sections->_zPlaneMaxPos = _zPlaneMax;
		}

		this->setDistance(2* sqrt((_xPlaneMax-_xPlaneMin)*(_xPlaneMax-_xPlaneMin) + (_yPlaneMax-_yPlaneMin)*(_yPlaneMax-_yPlaneMin) + (_zPlaneMax-_zPlaneMin)*(_zPlaneMax-_zPlaneMin)));
	}

	emit informationChanged();
}

// ==> clearScene()
//	Remove all OpenGL Object out of the scene
//--------------------------------------------------------------------
void SceneDrawer::clearScene()
{
	for (int i=0; i<_objects.size(); i++)
		delete _objects[i];
	_objects.clear();
}

// ==> initializeGL()
//	Initialize the OpenGL Widget
//--------------------------------------------------------------------
void SceneDrawer::initializeGL()
 {
	// Initialize QGLWidget (parent)
	QGLWidget::initializeGL();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.4196f,0.5451f,0.6667f,1.0f);

	// Place light
	GLfloat pos[] = {0., 0., 0., 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);

	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
		
			glBlendFunc (GL_ONE, GL_ONE);
 }

// ==> slotOnFrameUpdate()
//	Every new frame, the OpenGL widget needs to be updated
//--------------------------------------------------------------------
void SceneDrawer::slotOnFrameUpdate()
{
	this->updateGL();
}

// ==> setAzimuth(azimuth)
//	Set the azimuth and update the camera position based on this azimuth
//--------------------------------------------------------------------
void SceneDrawer::setAzimuth(float azimuth)
{
	_azimuth = azimuth;

	if (_azimuth > 360.0)
		_azimuth = _azimuth - 360.0;
	if (_azimuth < 0.0)
		_azimuth = _azimuth + 360.0;

	updateCamera();
}

// ==> setElevation(elevation)
//	Set the elevation and update the camera position based on this elevation
//--------------------------------------------------------------------
void SceneDrawer::setElevation(float elevation)
{
	_elevation = elevation;
	if (_elevation > 180.0)
		_elevation = 180.0;
	if (_elevation < 0.0)
		_elevation = 0.0;

	updateCamera();
}

// ==> setDistance(distance)
//	Set the distance and update the camera position based on this distance
//--------------------------------------------------------------------
void SceneDrawer::setDistance(float distance)
{
	_distance = distance;
	if (_distance < 0.0)
		_distance = 0.0;

	updateCamera();
}

// ==> setZPlaneMin(pos)
//	Set the 2D Z section plane min to the position 'pos'
//--------------------------------------------------------------------
void SceneDrawer::setZPlaneMin(float pos)
{
	_sections->_zPlaneMinPos = pos;
	if (_limitedPlanes)
	{
		if (_sections->_zPlaneMinPos < _zPlaneMin)
			_sections->_zPlaneMinPos = _zPlaneMin;
		if (_sections->_zPlaneMinPos > _sections->_zPlaneMaxPos)
			_sections->_zPlaneMinPos = _sections->_zPlaneMaxPos;
	}
	emit informationChanged();
}

// ==> setZPlaneMax(pos)
//	Set the 2D Z section plane max to the position 'pos'
//--------------------------------------------------------------------
void SceneDrawer::setZPlaneMax(float pos)
{
	_sections->_zPlaneMaxPos = pos;
	if (_limitedPlanes)
	{
		if (_sections->_zPlaneMaxPos > _zPlaneMax)
			_sections->_zPlaneMaxPos = _zPlaneMax;
		if (_sections->_zPlaneMaxPos < _sections->_zPlaneMinPos)
			_sections->_zPlaneMaxPos = _sections->_zPlaneMinPos;
	}
	emit informationChanged();
}

// ==> setXPlaneMin(pos)
//	Set the 2D X section plane min to the position 'pos'
//--------------------------------------------------------------------
void SceneDrawer::setXPlaneMin(float pos)
{
	_sections->_xPlaneMinPos = pos;
	if (_limitedPlanes)
	{
		if (_sections->_xPlaneMinPos < _xPlaneMin)
			_sections->_xPlaneMinPos = _xPlaneMin;
		if (_sections->_xPlaneMinPos > _sections->_xPlaneMaxPos)
			_sections->_xPlaneMinPos = _sections->_xPlaneMaxPos;
	}
	emit informationChanged();
}

// ==> setXPlaneMax(pos)
//	Set the 2D X section plane max to the position 'pos'
//--------------------------------------------------------------------
void SceneDrawer::setXPlaneMax(float pos)
{
	_sections->_xPlaneMaxPos = pos;
	if (_limitedPlanes)
	{
		if (_sections->_xPlaneMaxPos > _xPlaneMax)
			_sections->_xPlaneMaxPos = _xPlaneMax;
		if (_sections->_xPlaneMaxPos < _sections->_xPlaneMinPos)
			_sections->_xPlaneMaxPos = _sections->_xPlaneMinPos;
	}
	emit informationChanged();
}

// ==> setYPlaneMin(pos)
//	Set the 2D Y section plane min to the position 'pos'
//--------------------------------------------------------------------
void SceneDrawer::setYPlaneMin(float pos)
{
	_sections->_yPlaneMinPos = pos;
	if (_limitedPlanes)
	{
		if (_sections->_yPlaneMinPos < _yPlaneMin)
			_sections->_yPlaneMinPos = _yPlaneMin;
		if (_sections->_yPlaneMinPos > _sections->_yPlaneMaxPos)
			_sections->_yPlaneMinPos = _sections->_yPlaneMaxPos;
	}
	emit informationChanged();
}

// ==> setYPlaneMax(pos)
//	Set the 2D Y section plane max to the position 'pos'
//--------------------------------------------------------------------
void SceneDrawer::setYPlaneMax(float pos)
{
	_sections->_yPlaneMaxPos = pos;
	if (_limitedPlanes)
	{
		if (_sections->_yPlaneMaxPos > _yPlaneMax)
			_sections->_yPlaneMaxPos = _yPlaneMax;
		if (_sections->_yPlaneMaxPos < _sections->_yPlaneMinPos)
			_sections->_yPlaneMaxPos = _sections->_yPlaneMinPos;
	}
	emit informationChanged();
}

// ==> updateCamera()
//	Update the camera based on the distance, elevation and azimuth
//  Emit a signal that the camera position is changed
//--------------------------------------------------------------------
void SceneDrawer::updateCamera()
{
	//std::cout << "Camera updated" << std::endl;
	_camPosZ = _distance * cos(PI/2.0 - _elevation*PI/180.0) * cos(_azimuth*PI/180.0);
	_camPosX = _distance * cos(PI/2.0 - _elevation*PI/180.0) * sin(_azimuth*PI/180.0);
	_camPosY = _distance * sin(PI/2.0 - _elevation*PI/180.0);

	emit cameraPositionChanged(_camPosX, _camPosY, _camPosZ);
	emit cameraStrafeChanged(_camStrafeX, _camStrafeY, _camStrafeZ);
	emit informationChanged();
}


// ==> resizeGL(width, height)
//	Update the OpenGL widget based on the new resolution
//--------------------------------------------------------------------
void SceneDrawer::resizeGL(int width, int height)
{
	if ((width<=0) || (height<=0))
		return;

	//set viewport
	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//set persepctive
	GLdouble aspect_ratio=(GLdouble)width/(GLdouble)height;
	gluPerspective(45.0f, 4/3, 0.1, 5000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// ==> paintGL()
//	Repaint the OpenGL widget
//--------------------------------------------------------------------
void SceneDrawer::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_LIGHTING);
	GLfloat pos[] = {_camPosX, _camPosY, _camPosZ};
	glLightfv(GL_LIGHT1, GL_POSITION,pos);

	glPushMatrix();

	gluLookAt(_camPosX+_camStrafeX, _camPosY+_camStrafeY, _camPosZ+_camStrafeZ,
				_camLookX+_camStrafeX, _camLookY+_camStrafeY, _camLookZ+_camStrafeZ,
				0, 1, 0);

	glDisable (GL_BLEND);

	GLfloat m_ambient[] = {0.1, 0.1, 0.1};
	GLfloat m_diffuse[] = {220.0/255.0, 136.0/255.0, 55.0/255.0};
	GLfloat m_specular[] = {0.0, 0.0, 0.0};

	glMaterialfv( GL_FRONT, GL_AMBIENT, m_ambient );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, m_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, m_specular );
	
	glEnable(GL_LIGHTING);

	// Draw all the OpenGL Objects of the scene
	for (int i=0; i<this->_objects.size(); i++)
	{
		glPushMatrix();
		_objects[i]->draw();
		glPopMatrix();
	}



	glDisable(GL_LIGHTING);

	// Draws the basic axis
	glLineWidth (2.0);
	glPointSize(0.1);

	GLUquadricObj *quadratic =gluNewQuadric();			// Create A Pointer To The Quadric Object ( NEW )

	// Draw X axis red
	glBegin (GL_LINES);
	glColor3f (1,0,0); 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(2000.0, 0.0, 0.0 );

	// Draw stripes
	for (int i=5; i<=2000; i=i+5)
	{
		glVertex3f(i, 0.0, 0.0);
		glVertex3f(i, 0.0, 5.0 );
	}
	
	 // Draw Y axis green.
	glColor3f (0,1,0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 2000.0, 0.0 ); 

	// Draw stripes
	for (int i=5; i<=2000; i=i+5)
	{
		glVertex3f(0, i, 0.0 );
		glVertex3f(sqrt((5.0*5.0)/2.0), i, sqrt((5.0*5.0)/2.0));	
	}
	
	// Draw Z axis blue.
	glColor3f (0,0,1);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 2000.0 ); 

	// Draw stripes
	for (int i=5; i<=2000; i=i+5)
	{
		glVertex3f(0.0, 0.0, i);
		glVertex3f(5.0, 0.0, i );
	}
	glEnd();

	glColor3f (0,0.3,0); // z axis is blue.
	GLfloat m_diffuse2[] = {0.0, 0.6, 0.0};
	glMaterialfv( GL_FRONT, GL_DIFFUSE, m_diffuse2 );


	// Draw the section planes
	if (_sectionsEnabled)
	{
		glEnable(GL_LIGHTING);
	
		// TYPE 1: Rectangular cut out
		// Draw 6 section planes
		if (_sections->_typeSections == Sections3D::SECTIONS_RECTANGULAR)
		{
			glEnable (GL_BLEND);
			glBegin(GL_QUADS);

				glNormal3f(0.0, -1.0, 0.0);
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMinPos, this->_sections->_zPlaneMinPos);		
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMinPos, this->_sections->_zPlaneMinPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMinPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMinPos,  this->_sections->_zPlaneMaxPos);	

				glNormal3f(0.0, 1.0, 0.0);
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMaxPos, this->_sections->_zPlaneMinPos);
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMaxPos, this->_sections->_zPlaneMinPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMaxPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMaxPos,  this->_sections->_zPlaneMaxPos);	

				glNormal3f(0.0, 0.0, 1.0);
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMinPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMinPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos,  this->_sections->_yPlaneMaxPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f(this->_sections->_xPlaneMinPos,  this->_sections->_yPlaneMaxPos,  this->_sections->_zPlaneMaxPos);	

				glNormal3f(0.0, 0.0, -1.0);
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMinPos, this->_sections->_zPlaneMinPos);	
				glVertex3f(this->_sections->_xPlaneMinPos,  this->_sections->_yPlaneMaxPos, this->_sections->_zPlaneMinPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos,  this->_sections->_yPlaneMaxPos, this->_sections->_zPlaneMinPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMinPos, this->_sections->_zPlaneMinPos);	
		
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMinPos, this->_sections->_zPlaneMinPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos,  this->_sections->_yPlaneMaxPos, this->_sections->_zPlaneMinPos);	
				glVertex3f( this->_sections->_xPlaneMaxPos,  this->_sections->_yPlaneMaxPos,  this->_sections->_zPlaneMaxPos);
				glVertex3f( this->_sections->_xPlaneMaxPos, this->_sections->_yPlaneMinPos,  this->_sections->_zPlaneMaxPos);	
			
				glNormal3f(-1.0, 0.0, 0.0);
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMinPos, this->_sections->_zPlaneMinPos);	
				glVertex3f(this->_sections->_xPlaneMinPos, this->_sections->_yPlaneMinPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f(this->_sections->_xPlaneMinPos,  this->_sections->_yPlaneMaxPos,  this->_sections->_zPlaneMaxPos);	
				glVertex3f(this->_sections->_xPlaneMinPos,  this->_sections->_yPlaneMaxPos, this->_sections->_zPlaneMinPos);	
			glEnd();

			glDisable (GL_BLEND);
		}
		// TYPE 2: PIE PIECE cut out
		else if (_sections->_typeSections == Sections3D::SECTIONS_XY)
		{	
			glEnable (GL_BLEND);
			glEnable(GL_LIGHTING);

			// Draw the two rectangular planes of the pie piece on the max and min angle
			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glRotatef(this->_sections->_angleMax, 0, 0, 1);
				glBegin(GL_QUADS);
					glNormal3f(0.0, 1.0, 0.0);
					glVertex3f(_sections->_strafeX,  0, - this->_sections->_height/2.0);
					glVertex3f(  this->_sections->_radius,  0, - this->_sections->_height/2.0);
					glVertex3f(  this->_sections->_radius,  0,   this->_sections->_height/2.0);	
					glVertex3f( 0,  0,   this->_sections->_height/2.0);	
				glEnd();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glRotatef(this->_sections->_angleMin, 0, 0, 1);
				glBegin(GL_QUADS);	
					glNormal3f(0.0, -1.0, 0.0);
					glVertex3f( 0,  0, - this->_sections->_height/2.0);	
					glVertex3f(  this->_sections->_radius,  0,  - this->_sections->_height/2.0);	
					glVertex3f(  this->_sections->_radius,  0,   this->_sections->_height/2.0);	
					glVertex3f( 0,  0,   this->_sections->_height/2.0);	
				glEnd();
			glPopMatrix();	
				
			// Draw the upper and downer plane of the pie piece
			// This planes are circular segments
			// Discretizing the plane in smaller planes with small angle steps
			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glBegin(GL_TRIANGLES);
				float previousI = this->_sections->_angleMin;
				for (float i=this->_sections->_angleMin+0.5; i < this->_sections->_angleMax; i=i+0.5)
				{
					glNormal3f(0.0, 0.0, -1.0);
					glVertex3f( 0,  0, this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(previousI*PI/180.0),  this->_sections->_radius*sin(previousI*PI/180.0),  - this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(i*PI/180.0),  this->_sections->_radius*sin(i*PI/180.0), - this->_sections->_height/2.0);
					
					glNormal3f(0.0, 0.0, 1.0);
					glVertex3f( 0,  0,  this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(previousI*PI/180.0),  this->_sections->_radius*sin(previousI*PI/180.0),  this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(i*PI/180.0),  this->_sections->_radius*sin(i*PI/180.0),  this->_sections->_height/2.0);
					previousI = i;
				}
				glEnd();

				// Draw the outer circular segment of the pie piece
				// Discretizing the plane in smaller planes with small angle steps
				glBegin(GL_QUADS);
				previousI = this->_sections->_angleMin;
				for (float i=this->_sections->_angleMin+0.5; i < this->_sections->_angleMax; i=i+0.5)
				{
					glNormal3f( this->_sections->_radius*cos((previousI+0.25)*PI/180.0),  this->_sections->_radius*sin((previousI+0.25)*PI/180.0),  0.0);
					glVertex3f( this->_sections->_radius*cos(previousI*PI/180.0),  this->_sections->_radius*sin(previousI*PI/180.0), - this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(i*PI/180.0),  this->_sections->_radius*sin(i*PI/180.0), - this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(i*PI/180.0),  this->_sections->_radius*sin(i*PI/180.0),  this->_sections->_height/2.0);
					glVertex3f( this->_sections->_radius*cos(previousI*PI/180.0),  this->_sections->_radius*sin(previousI*PI/180.0),  this->_sections->_height/2.0);
					previousI = i;
				}
				glEnd();
			glPopMatrix();	

			glDisable (GL_BLEND);

		}
		// TYPE 2: PIE PIECE cut out
		else if (_sections->_typeSections == Sections3D::SECTIONS_YZ)
		{
			glEnable (GL_BLEND);
			glEnable(GL_LIGHTING);
	
			// Draw the two rectangular planes of the pie piece on the max and min angle
			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glRotatef(this->_sections->_angleMax, 1, 0, 0);
				glBegin(GL_QUADS);
					glNormal3f(0.0, 0.0, 1.0);
					glVertex3f(-this->_sections->_height/2.0, 0, 0);	
					glVertex3f(-this->_sections->_height/2.0 , this->_sections->_radius, 0.0);
					glVertex3f(this->_sections->_height/2.0,  this->_sections->_radius,  0.0);	
					glVertex3f(this->_sections->_height/2.0, 0,  0);	
				glEnd();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glRotatef(this->_sections->_angleMin, 1, 0, 0);
				glBegin(GL_QUADS);	
					glNormal3f(0.0, 0.0, -1.0);
					glVertex3f(-this->_sections->_height/2.0, 0, 0);	
					glVertex3f(-this->_sections->_height/2.0 , this->_sections->_radius, 0.0);	
					glVertex3f(this->_sections->_height/2.0,  this->_sections->_radius,  0.0);
					glVertex3f(this->_sections->_height/2.0, 0,  0);	
					glEnd();
			glPopMatrix();	

			// Draw the upper and downer plane of the pie piece
			// This planes are circular segments
			// Discretizing the plane in smaller planes with small angle steps
			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glBegin(GL_TRIANGLES);
				float previousI = this->_sections->_angleMin;
				for (float i=this->_sections->_angleMin+0.5; i < this->_sections->_angleMax; i=i+0.5)
				{
					glNormal3f(-1.0, 0.0, 0.0);
					glVertex3f(-this->_sections->_height/2.0, 0, 0.0);
					glVertex3f(-this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0), this->_sections->_radius*sin(previousI*PI/180.0));
					glVertex3f(-this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0), this->_sections->_radius*sin(i*PI/180.0));
					
					glNormal3f(1.0, 0.0, 0.0);
					glVertex3f(this->_sections->_height/2.0, 0, 0);
					glVertex3f(this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0), this->_sections->_radius*sin(previousI*PI/180.0));
					glVertex3f(this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0), this->_sections->_radius*sin(i*PI/180.0));
					previousI = i;
				}
				glEnd();

				// Draw the outer circular segment of the pie piece
				// Discretizing the plane in smaller planes with small angle steps
				glBegin(GL_QUADS);
				previousI = this->_sections->_angleMin;
				for (float i=this->_sections->_angleMin+0.5; i < this->_sections->_angleMax; i=i+0.5)
				{
					glNormal3f(0.0, this->_sections->_radius*cos((previousI+0.25)*PI/180.0), this->_sections->_radius*sin((previousI+0.25)*PI/180.0));
					glVertex3f(-this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0), this->_sections->_radius*sin(previousI*PI/180.0));
					glVertex3f(-this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0), this->_sections->_radius*sin(i*PI/180.0));
					glVertex3f(this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0), this->_sections->_radius*sin(i*PI/180.0));
					glVertex3f(this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0), this->_sections->_radius*sin(previousI*PI/180.0));
					previousI = i;
				}
				glEnd();
			glPopMatrix();
			
		
			

			glDisable (GL_BLEND);

		}
		// TYPE 2: PIE PIECE cut out
		else if (_sections->_typeSections == Sections3D::SECTIONS_XZ)
		{
			glEnable (GL_BLEND);
			glEnable(GL_LIGHTING);
	
			// Draw the two rectangular planes of the pie piece on the max and min angle
			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glRotatef(this->_sections->_angleMax, 0, 1, 0);
				glBegin(GL_QUADS);
					glNormal3f(1.0, 0.0, 0.0);
					glVertex3f(0, -this->_sections->_height/2.0, 0);	
					glVertex3f( 0, -this->_sections->_height/2.0, this->_sections->_radius);	
					glVertex3f( 0, this->_sections->_height/2.0,  this->_sections->_radius);	
					glVertex3f(0, this->_sections->_height/2.0,  0);	
				glEnd();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glRotatef(this->_sections->_angleMin, 0, 1, 0);
				glBegin(GL_QUADS);	
					glNormal3f(-1.0, 0.0, 0.0);
					glVertex3f(0, -this->_sections->_height/2.0, 0);
					glVertex3f( 0, -this->_sections->_height/2.0, this->_sections->_radius);	
					glVertex3f( 0, this->_sections->_height/2.0,  this->_sections->_radius);
					glVertex3f(0, this->_sections->_height/2.0,  0);	
				glEnd();
			glPopMatrix();		

			glPushMatrix();
				// Draw the upper and downer plane of the pie piece
				// This planes are circular segments
				// Discretizing the plane in smaller planes with small angle steps
				glTranslatef(_sections->_strafeX, _sections->_strafeY, _sections->_strafeZ);
				glBegin(GL_TRIANGLES);
				float previousI = this->_sections->_angleMin;
				for (float i=this->_sections->_angleMin+0.5; i < this->_sections->_angleMax; i=i+0.5)
				{
					glNormal3f(0.0, -1.0, 0.0);
					glVertex3f(0, -this->_sections->_height/2.0, 0);
					glVertex3f(this->_sections->_radius*sin(previousI*PI/180.0), -this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0));
					glVertex3f(this->_sections->_radius*sin(i*PI/180.0), -this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0));
					glNormal3f(0.0, 1.0, 0.0);
					glVertex3f(0, this->_sections->_height/2.0, 0);
					glVertex3f(this->_sections->_radius*sin(previousI*PI/180.0), this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0));
					glVertex3f(this->_sections->_radius*sin(i*PI/180.0), this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0));
					previousI = i;
				}
				glEnd();

				// Draw the outer circular segment of the pie piece
				// Discretizing the plane in smaller planes with small angle steps
				glBegin(GL_QUADS);
				previousI = this->_sections->_angleMin;
				for (float i=this->_sections->_angleMin+0.5; i < this->_sections->_angleMax; i=i+0.5)
				{
					glNormal3f(this->_sections->_radius*sin((previousI+0.25)*PI/180.0), 0.0, this->_sections->_radius*cos((previousI+0.25)*PI/180.0));
					glVertex3f(this->_sections->_radius*sin(previousI*PI/180.0), -this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0));
					glVertex3f(this->_sections->_radius*sin(i*PI/180.0), -this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0));
					glVertex3f(this->_sections->_radius*sin(i*PI/180.0), this->_sections->_height/2.0, this->_sections->_radius*cos(i*PI/180.0));
					glVertex3f(this->_sections->_radius*sin(previousI*PI/180.0), this->_sections->_height/2.0, this->_sections->_radius*cos(previousI*PI/180.0));
					previousI = i;
				}
				glEnd();
			glPopMatrix();

			glDisable (GL_BLEND);
		}	
	}

	glPopMatrix();
	gluDeleteQuadric(quadratic);
}

// ==> mousePressEvent(e)
//	When the mouse is pressed, we store the current position in the horizontal and vertical offset
//	We also see if there are some modifiers pressed (for example alt or ctrl)
//--------------------------------------------------------------------
void SceneDrawer::mousePressEvent(QMouseEvent* e)
{
	// Start picking
	int obj = this->selection(e->pos().x(), e->pos().y());
	//if (obj != 0)
	//	std::cout << "Pick " << obj << std::endl;

	// Store the coordinates of the mouse position
	_horizontalOffset = e->pos().x();
	_verticalOffset = e->pos().y();
	
	// Check if there are modifiers pressed
	_alt = false;
	_ctrl = false;
	if (e->modifiers() & Qt::AltModifier)
		_alt = true;
	if (e->modifiers() & Qt::ControlModifier)
		_ctrl = true;

	e->accept();
}

// ==> mouseMoveEvent(e)
//	Update the azimuth, elevation or strafe based on the relative change of the mouse position from the previous one
//	Again, restore the mouse position
//--------------------------------------------------------------------
void SceneDrawer::mouseMoveEvent(QMouseEvent* e)
{
	// If the modifiers are used, the change is executed on the strafe parameters, otherwise on the azimuth and elevation
	if (_ctrl && _alt)
	{
		this->setStrafeY(_camStrafeY - (float(e->pos().x()) - _horizontalOffset)*_sensitivity);
	}
	else if (_ctrl)
	{
		this->setStrafeZ(_camStrafeZ - (float(e->pos().x()) - _horizontalOffset)*_sensitivity);
	}
	else if (_alt)
	{
		this->setStrafeX(_camStrafeX - (float(e->pos().x()) - _horizontalOffset)*_sensitivity);
	}
	else
	{
		this->setAzimuth(_azimuth - (float(e->pos().x()) - _horizontalOffset)*_sensitivity);
		this->setElevation(_elevation - (float(e->pos().y()) - _verticalOffset)*_sensitivity);
	}

	// Update statusbar information
	QString status = QString("Azimuth: %1, Elevation: %2, Distance: %3, StrafeX: %4, StrafeY: %5, StrafeZ: %6").arg(_azimuth, 8, 'f', 3).arg(_elevation, 8, 'f', 3).arg(_distance, 8, 'f', 3).arg(_camStrafeX, 8, 'f', 3).arg(_camStrafeY, 8, 'f', 3).arg(_camStrafeZ, 8, 'f', 3);
	emit statusChanged(status, 0);	

	// Again, store the position of the mouse in the horizontal and vertical offset
	_horizontalOffset = e->pos().x();
	_verticalOffset = e->pos().y();

	e->accept();
}

// ==> mouseReleaseEvent(e)
//	Release all calculations on the strafe, azimuth and elevation
//--------------------------------------------------------------------
void SceneDrawer::mouseReleaseEvent(QMouseEvent* e)
{
	_horizontalOffset = 0.0;
	_verticalOffset = 0.0;
	_alt = false;
	_ctrl = false;

	e->accept();
}

// ==> wheelEvent(e)
//	Use the wheel event to change de distance of the camera
//--------------------------------------------------------------------
void SceneDrawer::wheelEvent(QWheelEvent *e)
{
	int numDegrees = e->delta() / 8;
	float numSteps = numDegrees / 15.0;

	if (e->orientation() == Qt::Vertical) 
		this->setDistance(_distance + numSteps*(0.1)*_distance);

	// Update the statusbar information
	QString status = QString("Azimuth: %1, Elevation: %2, Distance: %3, StrafeX: %4, StrafeY: %5, StrafeZ: %6").arg(_azimuth, 8, 'f', 3).arg(_elevation, 8, 'f', 3).arg(_distance, 8, 'f', 3).arg(_camStrafeX, 8, 'f', 3).arg(_camStrafeY, 8, 'f', 3).arg(_camStrafeZ, 8, 'f', 3);
	emit statusChanged(status, 0);	

	e->accept();
}




// ==> selection(mouseX, mouseY)
//	Picking of the scene based on the mouse position (doesn't work!)
//--------------------------------------------------------------------
int SceneDrawer::selection(int mouseX, int mouseY)											
{
	GLuint	buffer[512];										// Set Up A Selection Buffer
	GLint	hits;												// The Number Of Objects That We Selected

	this->_frameUpdateTimer->stop();

	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);								// Tell OpenGL To Use Our Array For Selection

	// Puts OpenGL In Selection Mode. Nothing Will Be Drawn.  Object ID's and Extents Are Stored In The Buffer.
	(void) glRenderMode(GL_SELECT);

	glInitNames();												// Initializes The Name Stack
	glPushName(0);												// Push 0 (At Least One Entry) Onto The Stack
	glPopName();
	glMatrixMode(GL_PROJECTION);								// Selects The Projection Matrix
	glPushMatrix();												// Push The Projection Matrix
	glLoadIdentity();											// Resets The Matrix

	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
	gluPickMatrix((GLdouble) mouseX, (GLdouble) (viewport[3]-mouseY), 1.0f, 1.0f, viewport);

	// Apply The Perspective Matrix
	gluPerspective(45.0f, (GLfloat) 4.0/3.0, 0.1f, 5000.0f);
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	this->paintGL();											// Render The Targets To The Selection Buffer
	glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glPopMatrix();		
	glFlush ();
	// Pop The Projection Matrix
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	hits=glRenderMode(GL_RENDER);

	this->_frameUpdateTimer->start(100);
	if (hits > 0)												// If There Were More Than 0 Hits
	{
		int	choose = buffer[3];									// Make Our Selection The First Object
		int depth = buffer[1];									// Store How Far Away It Is 
		return choose;
    }
	return 0;
}