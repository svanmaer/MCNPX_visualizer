//#########################################################################################################
//## Ui_MCNPXScene.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the OpenGL Widget to render the inner and outer case + sections
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef UI_MCNPX_SCENE_H
#define UI_MCNPX_SCENE_H

#include <QMainWindow>
#include <QtGui>
#include <QPixmap>
#include <QPrinter>
#include <QTextEdit>
#include <QThread>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QString>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QStringList>

#include "SceneDrawer.h"

class Ui_MCNPXScene : public QObject
{
	Q_OBJECT
	public:
		
		Ui_MCNPXScene(){sceneDrawer = NULL;}

		// Basic layout
		QVBoxLayout *mcnpxSceneFormLayOut;
		QWidget *mcnpxSceneFormLayOutWidget;

		// OpenGL Widget
		SceneDrawer* sceneDrawer;

		// ==> setupUi(mcnpxScene)
		//   Setup the UI of the widget (with parent mcnpxScene)
		//--------------------------------------------------------------------
		void setupUi(QWidget *mcnpxScene)
		{
			// Creates the OpenGL widget and add it over the whole parent widget
			sceneDrawer = new SceneDrawer(mcnpxScene);
			sceneDrawer->setGeometry(0, 0, 300, 300);
			mcnpxScene->setGeometry(0, 0, 300, 300);

			QMetaObject::connectSlotsByName(mcnpxScene);
		} // setupUi

	public slots:

		// Connect events from the top level with the OpenGL SceneDrawer
		//********************************************************************

		void onAzimuthChanged(double azimuth)
		{
			if (sceneDrawer && sceneDrawer->getAzimuth() != azimuth)
				sceneDrawer->setAzimuth(azimuth);
		}
		void onElevationChanged(double elevation)
		{
			if (sceneDrawer && sceneDrawer->getElevation() != elevation)
				sceneDrawer->setElevation(elevation);
		}
		void onAzimuthChanged(int azimuth)
		{
			if (sceneDrawer && sceneDrawer->getAzimuth() != azimuth)
				sceneDrawer->setAzimuth(azimuth);
		}
		void onElevationChanged(int elevation)
		{
			if (sceneDrawer && sceneDrawer->getElevation() != elevation)
				sceneDrawer->setElevation(elevation);
		}
		void onDistanceChanged(double distance)
		{
			if (sceneDrawer && sceneDrawer->getDistance() != distance)
				sceneDrawer->setDistance(distance);
		}

		void onZPlaneMinChanged(double pos)
		{
			if (sceneDrawer && sceneDrawer->getSections()->_zPlaneMinPos != pos)
				sceneDrawer->setZPlaneMin(pos);
		}

		void onZPlaneMaxChanged(double pos)
		{
			if (sceneDrawer && sceneDrawer->getSections()->_zPlaneMaxPos != pos)
				sceneDrawer->setZPlaneMax(pos);
		}
		void onXPlaneMinChanged(double pos)
		{
			if (sceneDrawer && sceneDrawer->getSections()->_xPlaneMinPos != pos)
				sceneDrawer->setXPlaneMin(pos);
		}

		void onXPlaneMaxChanged(double pos)
		{
			if (sceneDrawer && sceneDrawer->getSections()->_xPlaneMaxPos != pos)
				sceneDrawer->setXPlaneMax(pos);
		}

		void onYPlaneMinChanged(double pos)
		{
			if (sceneDrawer && sceneDrawer->getSections()->_yPlaneMinPos != pos)
				sceneDrawer->setYPlaneMin(pos);
		}

		void onYPlaneMaxChanged(double pos)
		{
			if (sceneDrawer && sceneDrawer->getSections()->_yPlaneMaxPos != pos)
				sceneDrawer->setYPlaneMax(pos);
		}

		void onAngleMinChanged(double angle)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->angleMinChanged(angle);
		}

		void onAngleMaxChanged(double angle)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->angleMaxChanged(angle);
		}

		void onHeightChanged(double height)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->heightChanged(height);
		}

		void onRadiusChanged(double radius)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->radiusChanged(radius);
		}

		void onStrafeXChanged(double strafe)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->strafeXChanged(strafe);
		}

		void onStrafeYChanged(double strafe)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->strafeYChanged(strafe);
		}

		void onStrafeZChanged(double strafe)
		{
			if (sceneDrawer)
				sceneDrawer->getSections()->strafeZChanged(strafe);
		}		
};

namespace Ui {
    class MCNPXScene: public Ui_MCNPXScene {};
} // namespace Ui


#endif //
