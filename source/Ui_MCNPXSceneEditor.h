//#########################################################################################################
//## Ui_MCNPXSceneEditor.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the editor for the MCNPX Scene
//##	=> The user can edit basic functionality for the camera
//##	=> The user can define section planes on the scene
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef UI_MCNPX_SCENE_EDITOR_H
#define UI_MCNPX_SCENE_EDITOR_H

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
#include <QSlider>
#include <QDial>
#include <QCheckBox>

class Ui_MCNPXSceneEditor : public QObject
{
	Q_OBJECT
	public:
		
		// basic layout
		QFormLayout *mcnpxSceneEditorLayOut;
		QWidget *mcnpxSceneEditorLayOutWidget;
/*
		QDial *azimuth;
		QDial *elevation;
		QSlider *distance;

		QSlider *zPlaneMin;
		QSlider *zPlaneMax;

		QSlider *xPlaneMin;
		QSlider *xPlaneMax;

		QSlider *yPlaneMin;
		QSlider *yPlaneMax;

		QCheckBox *enableSections;
		QCheckBox *limitedPlanes;

		FloatSpinBox *floatspinbox;
*/


		// New UI
		QWidget *verticalLayoutWidget;
		QVBoxLayout *verticalLayout;
		QGridLayout *gridLayout_settings;
		QGroupBox *groupBox_camera;
		QWidget *gridLayoutWidget;
		QGridLayout *gridLayout_camera;
		QDoubleSpinBox *doubleSpinBox_azimuth;
		QLabel *label_elevation;
		QDoubleSpinBox *doubleSpinBox_elevation;
		QDial *dial_elevation;
		QLabel *label_azimuth;
		QDial *dial_azimuth;
		QLabel *label_strafe;
		QHBoxLayout *horizontalLayout_strafe;
		QLabel *label_strafeX;
		QDoubleSpinBox *doubleSpinBox_strafeX;
		QLabel *label_strafeY;
		QDoubleSpinBox *doubleSpinBox_strafeY;
		QLabel *label_strafeZ;
		QDoubleSpinBox *doubleSpinBox_strafeZ;
		QDoubleSpinBox *doubleSpinBox_distance;
		QHBoxLayout *horizontalLayout_distanceSlider;
		QLabel *label_distanceMin;
		QSlider *horizontalSlider_distance;
		QLabel *label_dinstanceMax;
		QLabel *label_distance;
		QCheckBox *checkBox_orthographic;
		QGroupBox *groupBox_sections;
		//QWidget *verticalLayoutWidget_2;
		QVBoxLayout *verticalLayout_sections;
		QHBoxLayout *horizontalLayout_fastSection;
		QPushButton *pushButton_xPlane0;
		QPushButton *pushButton_yPlane0;
		QPushButton *pushButton_zPlane0;
		QHBoxLayout *horizontalLayout_sectionsDistance;
		QLabel *label_sectionsBase;
		QDoubleSpinBox *sectionBase;
		QLabel *label_sectionsDistance;
		QDoubleSpinBox *sectionDistance;
		QSpacerItem *horizontalSpacer_sectionsDistance;
		QHBoxLayout *horizontalLayoutOrigin;
		QLabel *label_originY;
		QDoubleSpinBox *doubleSpinBox_originX;
		QLabel *label_originX;
		QDoubleSpinBox *doubleSpinBox_originY;
		QLabel *label_originZ;
		QDoubleSpinBox *doubleSpinBox_originZ;
		QFrame *line_sections;
		QGridLayout *gridLayout_sectionPlanes;
		QDoubleSpinBox *doubleSpinBox_xPlaneMin;
		QDoubleSpinBox *doubleSpinBox_xPlaneMax;
		QHBoxLayout *horizontalLayout_xPlaneMin;
		QLabel *label_xPlaneMin_Min;
		QSlider *horizontalSlider_xPlaneMin;
		QLabel *label_xPlaneMin_Max;
		QDoubleSpinBox *doubleSpinBox_yPlaneMin;
		QHBoxLayout *horizontalLayout_xPlaneMax;
		QLabel *label_xPlaneMax_Min;
		QSlider *horizontalSlider_xPlaneMax;
		QLabel *label_xPlaneMax_Max;
		QHBoxLayout *horizontalLayout_yPlaneMin;
		QLabel *label_yPlaneMin_Min;
		QSlider *horizontalSlider_yPlaneMin;
		QLabel *label_yPlaneMin_Max;
		QHBoxLayout *horizontalLayout_yPlaneMax;
		QLabel *label_yPlaneMax_Min;
		QSlider *horizontalSlider_yPlaneMax;
		QLabel *label_yPlaneMax_Max;
		QHBoxLayout *horizontalLayout_zPlaneMin;
		QLabel *label_zPlaneMin_Min;
		QSlider *horizontalSlider_zPlaneMin;
		QLabel *label_zPlaneMin_Max;
		QHBoxLayout *horizontalLayout_zPlaneMax;
		QLabel *label_zPlaneMax_Min;
		QSlider *horizontalSlider_zPlaneMax;
		QLabel *label_zPlaneMax_Max;
		QDoubleSpinBox *doubleSpinBox_yPlaneMax;
		QDoubleSpinBox *doubleSpinBox_zPlaneMin;
		QDoubleSpinBox *doubleSpinBox_zPlaneMax;
		QPushButton *pushButton_xPlaneMin;
		QPushButton *pushButton_xPlaneMax;
		QPushButton *pushButton_yPlaneMin;
		QPushButton *pushButton_yPlaneMax;
		QPushButton *pushButton_zPlaneMin;
		QPushButton *pushButton_zPlaneMax;
		QLabel *label_angleMax;
		QFrame *line_sections2;
		QDoubleSpinBox *doubleSpinBox_angleMax;
		QHBoxLayout *horizontalLayout_angleMax;
		QLabel *label_angleMin_Min;
		QSlider *horizontalSlider_angleMin;
		QLabel *label_angleMin_Max;
		QPushButton *pushButton_angleMin;
		QDoubleSpinBox *doubleSpinBox_angleMin;
		QHBoxLayout *horizontalLayout_angleMin;
		QLabel *label_AngleMax_Min;
		QLabel *label_angleMin;
		QSlider *horizontalSlider_angleMax;
		QLabel *label_angleMax_Max;
		QLabel *label_radius;
		QDoubleSpinBox *doubleSpinBox_radius;
		QHBoxLayout *horizontalLayout_radius;
		QLabel *label_radius_Min;
		QSlider *horizontalSlider_radius;
		QLabel *label_radius_Max;
		QLabel *label_strafeSections;
		QHBoxLayout *horizontalLayout_strafeSections;
		QLabel *label_strafeXSections;
		QDoubleSpinBox *doubleSpinBox_strafeXSections;
		QLabel *label_strafeYSections;
		QDoubleSpinBox *doubleSpinBox_strafeYSections;
		QLabel *label_strafeZSections;
		QDoubleSpinBox *doubleSpinBox_strafeZSections;
		QFrame *line_sections3;
		QHBoxLayout *horizontalLayout_sectionOptions;
		QCheckBox *checkBox_useSections;
		QCheckBox *checkBox_limitedBB;
		QCheckBox *checkBox_usePiece;
		QHBoxLayout *horizontalLayout_piece;
		QRadioButton *radioButton_XY;
		QRadioButton *radioButton_YZ;
		QRadioButton *radioButton_XZ;
		QLabel *label_height;
		QDoubleSpinBox *doubleSpinBox_height;
		QHBoxLayout *horizontalLayout_height;
		QLabel *label_height_Min;
		QSlider *horizontalSlider_height;
		QLabel *label_height_Max;

		// ==> setupUi(mcnpxSceneEditor)
		//   Setup the connections for the UI of the widget (with parent mcnpxSceneEditor)
		//--------------------------------------------------------------------
		void setupUi(QWidget *mcnpxSceneEditor)
		{
			mcnpxSceneEditorLayOutWidget = new QWidget(mcnpxSceneEditor);
			this->setupNewUi(mcnpxSceneEditorLayOutWidget);
			
			connect(doubleSpinBox_distance, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_distance(double)));
			connect(horizontalSlider_distance, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_distance(int)));
			connect(horizontalSlider_distance, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_distance(int)));

			connect(doubleSpinBox_xPlaneMin, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_xPlaneMin(double)));
			connect(doubleSpinBox_xPlaneMax, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_xPlaneMax(double)));
			connect(doubleSpinBox_yPlaneMin, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_yPlaneMin(double)));
			connect(doubleSpinBox_yPlaneMax, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_yPlaneMax(double)));
			connect(doubleSpinBox_zPlaneMin, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_zPlaneMin(double)));
			connect(doubleSpinBox_zPlaneMax, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_zPlaneMax(double)));

			connect(horizontalSlider_xPlaneMin, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_xPlaneMin(int)));
			connect(horizontalSlider_xPlaneMax, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_xPlaneMax(int)));
			connect(horizontalSlider_yPlaneMin, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_yPlaneMin(int)));
			connect(horizontalSlider_yPlaneMax, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_yPlaneMax(int)));
			connect(horizontalSlider_zPlaneMin, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_zPlaneMin(int)));
			connect(horizontalSlider_zPlaneMax, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_zPlaneMax(int)));
			connect(horizontalSlider_xPlaneMin, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_xPlaneMin(int)));
			connect(horizontalSlider_xPlaneMax, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_xPlaneMax(int)));
			connect(horizontalSlider_yPlaneMin, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_yPlaneMin(int)));
			connect(horizontalSlider_yPlaneMax, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_yPlaneMax(int)));
			connect(horizontalSlider_zPlaneMin, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_zPlaneMin(int)));
			connect(horizontalSlider_zPlaneMax, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSlider_zPlaneMax(int)));

			connect(doubleSpinBox_angleMax, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_angleMax(double)));
			connect(doubleSpinBox_angleMin, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_angleMin(double)));
			connect(doubleSpinBox_radius, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_radius(double)));
			connect(doubleSpinBox_height, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBox_height(double)));
			connect(horizontalSlider_angleMax, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_angleMax(int)));
			connect(horizontalSlider_angleMin, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_angleMin(int)));
			connect(horizontalSlider_radius, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_radius(int)));
			connect(horizontalSlider_height, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSlider_height(int)));

			connect(radioButton_XY, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonXYChanged(bool)));
			connect(radioButton_YZ, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonYZChanged(bool)));
			connect(radioButton_XZ, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonXZChanged(bool)));
			
			QMetaObject::connectSlotsByName(mcnpxSceneEditor);
		} // setupUi

		
		// ==> setupNewUi(parent)
		//   Setup the whole UI for the widget (with parent parent)
		//--------------------------------------------------------------------
		void setupNewUi(QWidget *parent)
		{
			gridLayout_settings = new QGridLayout(parent);
			gridLayout_settings->setObjectName(QString::fromUtf8("gridLayout_settings"));
			groupBox_camera = new QGroupBox(parent);
			groupBox_camera->setObjectName(QString::fromUtf8("groupBox_camera"));
			gridLayout_camera = new QGridLayout(groupBox_camera);
			gridLayout_camera->setObjectName(QString::fromUtf8("gridLayout_camera"));
			doubleSpinBox_azimuth = new QDoubleSpinBox(groupBox_camera);
			doubleSpinBox_azimuth->setObjectName(QString::fromUtf8("doubleSpinBox_azimuth"));
			doubleSpinBox_azimuth->setMinimumSize(QSize(70, 0));
			doubleSpinBox_azimuth->setMouseTracking(true);
			doubleSpinBox_azimuth->setMinimum(-360);
   		 	doubleSpinBox_azimuth->setMaximum(360);

			gridLayout_camera->addWidget(doubleSpinBox_azimuth, 0, 1, 1, 1);

			label_elevation = new QLabel(groupBox_camera);
			label_elevation->setObjectName(QString::fromUtf8("label_elevation"));

			gridLayout_camera->addWidget(label_elevation, 1, 0, 1, 1);

			doubleSpinBox_elevation = new QDoubleSpinBox(groupBox_camera);
			doubleSpinBox_elevation->setObjectName(QString::fromUtf8("doubleSpinBox_elevation"));
			doubleSpinBox_elevation->setMinimumSize(QSize(70, 0));
			doubleSpinBox_elevation->setMouseTracking(true);
			doubleSpinBox_elevation->setValue(90);
			doubleSpinBox_elevation->setMinimum(0);
			doubleSpinBox_elevation->setMaximum(180);

			gridLayout_camera->addWidget(doubleSpinBox_elevation, 1, 1, 1, 1);

			dial_elevation = new QDial(groupBox_camera);
			dial_elevation->setObjectName(QString::fromUtf8("dial_elevation"));
			dial_elevation->setMinimum(0);
			dial_elevation->setMaximum(180);
			dial_elevation->setValue(90);
			dial_elevation->setWrapping(false);
			dial_elevation->setNotchTarget(2);
			dial_elevation->setNotchesVisible(true);

			gridLayout_camera->addWidget(dial_elevation, 1, 2, 1, 1);

			label_azimuth = new QLabel(groupBox_camera);
			label_azimuth->setObjectName(QString::fromUtf8("label_azimuth"));

			gridLayout_camera->addWidget(label_azimuth, 0, 0, 1, 1);

			dial_azimuth = new QDial(groupBox_camera);
			dial_azimuth->setObjectName(QString::fromUtf8("dial_azimuth"));
			dial_azimuth->setMinimum(0);
			dial_azimuth->setMaximum(360);
			dial_azimuth->setWrapping(true);
			dial_azimuth->setNotchTarget(5);
			dial_azimuth->setNotchesVisible(true);

			gridLayout_camera->addWidget(dial_azimuth, 0, 2, 1, 1);

			label_strafe = new QLabel(groupBox_camera);
			label_strafe->setObjectName(QString::fromUtf8("label_strafe"));
			label_strafe->setMaximumSize(QSize(16777215, 30));

			gridLayout_camera->addWidget(label_strafe, 3, 0, 1, 1);

			horizontalLayout_strafe = new QHBoxLayout();
			horizontalLayout_strafe->setObjectName(QString::fromUtf8("horizontalLayout_strafe"));
			horizontalLayout_strafe->setSizeConstraint(QLayout::SetDefaultConstraint);
			label_strafeX = new QLabel(groupBox_camera);
			label_strafeX->setObjectName(QString::fromUtf8("label_strafeX"));

			horizontalLayout_strafe->addWidget(label_strafeX);

			doubleSpinBox_strafeX = new QDoubleSpinBox(groupBox_camera);
			doubleSpinBox_strafeX->setObjectName(QString::fromUtf8("doubleSpinBox_strafeX"));
			doubleSpinBox_strafeX->setMinimumSize(QSize(70, 0));
			doubleSpinBox_strafeX->setMouseTracking(true);
			doubleSpinBox_strafeX->setMaximum(10000);
			doubleSpinBox_strafeX->setMinimum(-10000);

			horizontalLayout_strafe->addWidget(doubleSpinBox_strafeX);

			label_strafeY = new QLabel(groupBox_camera);
			label_strafeY->setObjectName(QString::fromUtf8("label_strafeY"));

			horizontalLayout_strafe->addWidget(label_strafeY);

			doubleSpinBox_strafeY = new QDoubleSpinBox(groupBox_camera);
			doubleSpinBox_strafeY->setObjectName(QString::fromUtf8("doubleSpinBox_strafeY"));
			doubleSpinBox_strafeY->setMinimumSize(QSize(70, 0));
			doubleSpinBox_strafeY->setMouseTracking(true);
			doubleSpinBox_strafeY->setMaximum(10000);
			doubleSpinBox_strafeY->setMinimum(-10000);

			horizontalLayout_strafe->addWidget(doubleSpinBox_strafeY);

			label_strafeZ = new QLabel(groupBox_camera);
			label_strafeZ->setObjectName(QString::fromUtf8("label_strafeZ"));

			horizontalLayout_strafe->addWidget(label_strafeZ);

			doubleSpinBox_strafeZ = new QDoubleSpinBox(groupBox_camera);
			doubleSpinBox_strafeZ->setObjectName(QString::fromUtf8("doubleSpinBox_strafeZ"));
			doubleSpinBox_strafeZ->setMinimumSize(QSize(70, 0));
			doubleSpinBox_strafeZ->setMouseTracking(true);
			doubleSpinBox_strafeZ->setMaximum(10000);
			doubleSpinBox_strafeZ->setMinimum(-10000);

			horizontalLayout_strafe->addWidget(doubleSpinBox_strafeZ);


			gridLayout_camera->addLayout(horizontalLayout_strafe, 3, 1, 1, 2);

			doubleSpinBox_distance = new QDoubleSpinBox(groupBox_camera);
			doubleSpinBox_distance->setObjectName(QString::fromUtf8("doubleSpinBox_distance"));
			doubleSpinBox_distance->setMinimumSize(QSize(70, 0));
			doubleSpinBox_distance->setMouseTracking(true);
			doubleSpinBox_distance->setMinimum(-500);
			doubleSpinBox_distance->setMaximum(500);
			doubleSpinBox_distance->setValue(100);

			gridLayout_camera->addWidget(doubleSpinBox_distance, 2, 1, 1, 1);

			horizontalLayout_distanceSlider = new QHBoxLayout();
			horizontalLayout_distanceSlider->setObjectName(QString::fromUtf8("horizontalLayout_distanceSlider"));
			label_distanceMin = new QLabel(groupBox_camera);
			label_distanceMin->setObjectName(QString::fromUtf8("label_distanceMin"));
			label_distanceMin->setMinimumSize(QSize(30, 0));

			horizontalLayout_distanceSlider->addWidget(label_distanceMin);

			horizontalSlider_distance = new QSlider(groupBox_camera);
			horizontalSlider_distance->setObjectName(QString::fromUtf8("horizontalSlider_distance"));
			horizontalSlider_distance->setMinimum(-500);
			horizontalSlider_distance->setMaximum(500);
			horizontalSlider_distance->setValue(100);
			horizontalSlider_distance->setTracking(false);
			horizontalSlider_distance->setOrientation(Qt::Horizontal);
			horizontalSlider_distance->setInvertedAppearance(false);
			horizontalSlider_distance->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_distance->setTickInterval(25);

			horizontalLayout_distanceSlider->addWidget(horizontalSlider_distance);

			label_dinstanceMax = new QLabel(groupBox_camera);
			label_dinstanceMax->setObjectName(QString::fromUtf8("label_dinstanceMax"));
			label_dinstanceMax->setMinimumSize(QSize(30, 0));
			label_dinstanceMax->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_distanceSlider->addWidget(label_dinstanceMax);


			gridLayout_camera->addLayout(horizontalLayout_distanceSlider, 2, 2, 1, 1);

			label_distance = new QLabel(groupBox_camera);
			label_distance->setObjectName(QString::fromUtf8("label_distance"));

			gridLayout_camera->addWidget(label_distance, 2, 0, 1, 1);

			checkBox_orthographic = new QCheckBox(groupBox_camera);
			checkBox_orthographic->setObjectName(QString::fromUtf8("checkBox_orthographic"));

			gridLayout_camera->addWidget(checkBox_orthographic, 4, 0, 1, 3);



			gridLayout_settings->addWidget(groupBox_camera, 0, 0, 2, 1);

			groupBox_sections = new QGroupBox(parent);
			groupBox_sections->setObjectName(QString::fromUtf8("groupBox_sections"));
			verticalLayout_sections = new QVBoxLayout(groupBox_sections);
			verticalLayout_sections->setObjectName(QString::fromUtf8("verticalLayout_sections"));
			verticalLayout_sections->setContentsMargins(0, 0, 0, 0);
			horizontalLayout_fastSection = new QHBoxLayout();
			horizontalLayout_fastSection->setObjectName(QString::fromUtf8("horizontalLayout_fastSection"));
			pushButton_xPlane0 = new QPushButton(groupBox_sections);
			pushButton_xPlane0->setObjectName(QString::fromUtf8("pushButton_xPlane0"));
			pushButton_xPlane0->setMinimumSize(QSize(0, 50));

			horizontalLayout_fastSection->addWidget(pushButton_xPlane0);

			pushButton_yPlane0 = new QPushButton(groupBox_sections);
			pushButton_yPlane0->setObjectName(QString::fromUtf8("pushButton_yPlane0"));
			pushButton_yPlane0->setMinimumSize(QSize(0, 50));

			horizontalLayout_fastSection->addWidget(pushButton_yPlane0);

			pushButton_zPlane0 = new QPushButton(groupBox_sections);
			pushButton_zPlane0->setObjectName(QString::fromUtf8("pushButton_zPlane0"));
			pushButton_zPlane0->setMinimumSize(QSize(0, 50));

			horizontalLayout_fastSection->addWidget(pushButton_zPlane0);


			verticalLayout_sections->addLayout(horizontalLayout_fastSection);

			horizontalLayout_sectionsDistance = new QHBoxLayout();
			horizontalLayout_sectionsDistance->setObjectName(QString::fromUtf8("horizontalLayout_sectionsDistance"));
			label_sectionsBase = new QLabel(groupBox_sections);
			label_sectionsBase->setObjectName(QString::fromUtf8("label_sectionsBase"));
			label_sectionsBase->setMaximumSize(QSize(60, 16777215));

			horizontalLayout_sectionsDistance->addWidget(label_sectionsBase);

			sectionBase = new QDoubleSpinBox(groupBox_sections);
			sectionBase->setObjectName(QString::fromUtf8("sectionBase"));
			sectionBase->setMinimumSize(QSize(70, 0));
			sectionBase->setMaximumSize(QSize(100, 16777215));
			sectionBase->setMaximum(10000);
			sectionBase->setMinimum(-10000);

			horizontalLayout_sectionsDistance->addWidget(sectionBase);

			label_sectionsDistance = new QLabel(groupBox_sections);
			label_sectionsDistance->setObjectName(QString::fromUtf8("label_sectionsDistance"));
			label_sectionsDistance->setMaximumSize(QSize(60, 16777215));

			horizontalLayout_sectionsDistance->addWidget(label_sectionsDistance);

			sectionDistance = new QDoubleSpinBox(groupBox_sections);
			sectionDistance->setObjectName(QString::fromUtf8("sectionDistance"));
			sectionDistance->setMinimumSize(QSize(70, 0));
			sectionDistance->setMaximumSize(QSize(100, 16777215));
			sectionDistance->setMaximum(10000);
			sectionDistance->setMinimum(0);
			sectionDistance->setValue(100);

			horizontalLayout_sectionsDistance->addWidget(sectionDistance);

			horizontalSpacer_sectionsDistance = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

			horizontalLayout_sectionsDistance->addItem(horizontalSpacer_sectionsDistance);

			verticalLayout_sections->addLayout(horizontalLayout_sectionsDistance);

			horizontalLayoutOrigin = new QHBoxLayout();
			horizontalLayoutOrigin->setObjectName(QString::fromUtf8("horizontalLayoutOrigin"));
			label_originY = new QLabel(groupBox_sections);
			label_originY->setObjectName(QString::fromUtf8("label_originY"));
			label_originY->setMaximumSize(QSize(40, 16777215));

			horizontalLayoutOrigin->addWidget(label_originY);

			doubleSpinBox_originX = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_originX->setObjectName(QString::fromUtf8("doubleSpinBox_originX"));
			doubleSpinBox_originX->setMaximum(10000);
			doubleSpinBox_originX->setMinimum(-10000);

			horizontalLayoutOrigin->addWidget(doubleSpinBox_originX);

			label_originX = new QLabel(groupBox_sections);
			label_originX->setObjectName(QString::fromUtf8("label_originX"));
			label_originX->setMaximumSize(QSize(40, 16777215));

			horizontalLayoutOrigin->addWidget(label_originX);

			doubleSpinBox_originY = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_originY->setObjectName(QString::fromUtf8("doubleSpinBox_originY"));
			doubleSpinBox_originY->setMaximum(10000);
			doubleSpinBox_originY->setMinimum(-10000);

			horizontalLayoutOrigin->addWidget(doubleSpinBox_originY);

			label_originZ = new QLabel(groupBox_sections);
			label_originZ->setObjectName(QString::fromUtf8("label_originZ"));
			label_originZ->setMaximumSize(QSize(40, 16777215));

			horizontalLayoutOrigin->addWidget(label_originZ);

			doubleSpinBox_originZ = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_originZ->setObjectName(QString::fromUtf8("doubleSpinBox_originZ"));
			doubleSpinBox_originZ->setMaximum(10000);
			doubleSpinBox_originZ->setMinimum(-10000);

			horizontalLayoutOrigin->addWidget(doubleSpinBox_originZ);


			verticalLayout_sections->addLayout(horizontalLayoutOrigin);

			line_sections = new QFrame(groupBox_sections);
			line_sections->setObjectName(QString::fromUtf8("line_sections"));
			line_sections->setFrameShape(QFrame::HLine);
			line_sections->setFrameShadow(QFrame::Sunken);

			verticalLayout_sections->addWidget(line_sections);

			gridLayout_sectionPlanes = new QGridLayout();
			gridLayout_sectionPlanes->setObjectName(QString::fromUtf8("gridLayout_sectionPlanes"));
			doubleSpinBox_xPlaneMin = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_xPlaneMin->setObjectName(QString::fromUtf8("doubleSpinBox_xPlaneMin"));
			doubleSpinBox_xPlaneMin->setMinimumSize(QSize(70, 0));
			doubleSpinBox_xPlaneMin->setMouseTracking(true);
			doubleSpinBox_xPlaneMin->setMinimum(-5000);
			doubleSpinBox_xPlaneMin->setMaximum(5000);
			doubleSpinBox_xPlaneMin->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_xPlaneMin, 8, 1, 1, 1);

			doubleSpinBox_xPlaneMax = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_xPlaneMax->setObjectName(QString::fromUtf8("doubleSpinBox_xPlaneMax"));
			doubleSpinBox_xPlaneMax->setMinimumSize(QSize(70, 0));
			doubleSpinBox_xPlaneMax->setMouseTracking(true);
			doubleSpinBox_xPlaneMax->setMinimum(-5000);
			doubleSpinBox_xPlaneMax->setMaximum(5000);
			doubleSpinBox_xPlaneMax->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_xPlaneMax, 9, 1, 1, 1);

			horizontalLayout_xPlaneMin = new QHBoxLayout();
			horizontalLayout_xPlaneMin->setObjectName(QString::fromUtf8("horizontalLayout_xPlaneMin"));
			label_xPlaneMin_Min = new QLabel(groupBox_sections);
			label_xPlaneMin_Min->setObjectName(QString::fromUtf8("label_xPlaneMin_Min"));
			label_xPlaneMin_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_xPlaneMin->addWidget(label_xPlaneMin_Min);

			horizontalSlider_xPlaneMin = new QSlider(groupBox_sections);
			horizontalSlider_xPlaneMin->setObjectName(QString::fromUtf8("horizontalSlider_xPlaneMin"));
			horizontalSlider_xPlaneMin->setMinimum(-5000);
			horizontalSlider_xPlaneMin->setMaximum(5000);
			horizontalSlider_xPlaneMin->setValue(100);
			horizontalSlider_xPlaneMin->setTracking(false);
			horizontalSlider_xPlaneMin->setOrientation(Qt::Horizontal);
			horizontalSlider_xPlaneMin->setInvertedAppearance(false);
			horizontalSlider_xPlaneMin->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_xPlaneMin->setTickInterval(25);

			horizontalLayout_xPlaneMin->addWidget(horizontalSlider_xPlaneMin);

			label_xPlaneMin_Max = new QLabel(groupBox_sections);
			label_xPlaneMin_Max->setObjectName(QString::fromUtf8("label_xPlaneMin_Max"));
			label_xPlaneMin_Max->setMinimumSize(QSize(30, 0));
			label_xPlaneMin_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_xPlaneMin->addWidget(label_xPlaneMin_Max);

			gridLayout_sectionPlanes->addLayout(horizontalLayout_xPlaneMin, 8, 2, 1, 1);

			doubleSpinBox_yPlaneMin = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_yPlaneMin->setObjectName(QString::fromUtf8("doubleSpinBox_yPlaneMin"));
			doubleSpinBox_yPlaneMin->setMinimumSize(QSize(70, 0));
			doubleSpinBox_yPlaneMin->setMouseTracking(true);
			doubleSpinBox_yPlaneMin->setMinimum(-5000);
			doubleSpinBox_yPlaneMin->setMaximum(5000);
			doubleSpinBox_yPlaneMin->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_yPlaneMin, 10, 1, 1, 1);

			horizontalLayout_xPlaneMax = new QHBoxLayout();
			horizontalLayout_xPlaneMax->setObjectName(QString::fromUtf8("horizontalLayout_xPlaneMax"));
			label_xPlaneMax_Min = new QLabel(groupBox_sections);
			label_xPlaneMax_Min->setObjectName(QString::fromUtf8("label_xPlaneMax_Min"));
			label_xPlaneMax_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_xPlaneMax->addWidget(label_xPlaneMax_Min);

			horizontalSlider_xPlaneMax = new QSlider(groupBox_sections);
			horizontalSlider_xPlaneMax->setObjectName(QString::fromUtf8("horizontalSlider_xPlaneMax"));
			horizontalSlider_xPlaneMax->setMinimum(-5000);
			horizontalSlider_xPlaneMax->setMaximum(5000);
			horizontalSlider_xPlaneMax->setValue(100);
			horizontalSlider_xPlaneMax->setTracking(false);
			horizontalSlider_xPlaneMax->setOrientation(Qt::Horizontal);
			horizontalSlider_xPlaneMax->setInvertedAppearance(false);
			horizontalSlider_xPlaneMax->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_xPlaneMax->setTickInterval(25);

			horizontalLayout_xPlaneMax->addWidget(horizontalSlider_xPlaneMax);

			label_xPlaneMax_Max = new QLabel(groupBox_sections);
			label_xPlaneMax_Max->setObjectName(QString::fromUtf8("label_xPlaneMax_Max"));
			label_xPlaneMax_Max->setMinimumSize(QSize(30, 0));
			label_xPlaneMax_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_xPlaneMax->addWidget(label_xPlaneMax_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_xPlaneMax, 9, 2, 1, 1);

			horizontalLayout_yPlaneMin = new QHBoxLayout();
			horizontalLayout_yPlaneMin->setObjectName(QString::fromUtf8("horizontalLayout_yPlaneMin"));
			label_yPlaneMin_Min = new QLabel(groupBox_sections);
			label_yPlaneMin_Min->setObjectName(QString::fromUtf8("label_yPlaneMin_Min"));
			label_yPlaneMin_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_yPlaneMin->addWidget(label_yPlaneMin_Min);

			horizontalSlider_yPlaneMin = new QSlider(groupBox_sections);
			horizontalSlider_yPlaneMin->setObjectName(QString::fromUtf8("horizontalSlider_yPlaneMin"));
			horizontalSlider_yPlaneMin->setMinimum(-5000);
			horizontalSlider_yPlaneMin->setMaximum(5000);
			horizontalSlider_yPlaneMin->setValue(100);
			horizontalSlider_yPlaneMin->setTracking(false);
			horizontalSlider_yPlaneMin->setOrientation(Qt::Horizontal);
			horizontalSlider_yPlaneMin->setInvertedAppearance(false);
			horizontalSlider_yPlaneMin->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_yPlaneMin->setTickInterval(25);

			horizontalLayout_yPlaneMin->addWidget(horizontalSlider_yPlaneMin);

			label_yPlaneMin_Max = new QLabel(groupBox_sections);
			label_yPlaneMin_Max->setObjectName(QString::fromUtf8("label_yPlaneMin_Max"));
			label_yPlaneMin_Max->setMinimumSize(QSize(30, 0));
			label_yPlaneMin_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_yPlaneMin->addWidget(label_yPlaneMin_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_yPlaneMin, 10, 2, 1, 1);

			horizontalLayout_yPlaneMax = new QHBoxLayout();
			horizontalLayout_yPlaneMax->setObjectName(QString::fromUtf8("horizontalLayout_yPlaneMax"));
			label_yPlaneMax_Min = new QLabel(groupBox_sections);
			label_yPlaneMax_Min->setObjectName(QString::fromUtf8("label_yPlaneMax_Min"));
			label_yPlaneMax_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_yPlaneMax->addWidget(label_yPlaneMax_Min);

			horizontalSlider_yPlaneMax = new QSlider(groupBox_sections);
			horizontalSlider_yPlaneMax->setObjectName(QString::fromUtf8("horizontalSlider_yPlaneMax"));
			horizontalSlider_yPlaneMax->setMinimum(-5000);
			horizontalSlider_yPlaneMax->setMaximum(5000);
			horizontalSlider_yPlaneMax->setValue(100);
			horizontalSlider_yPlaneMax->setTracking(false);
			horizontalSlider_yPlaneMax->setOrientation(Qt::Horizontal);
			horizontalSlider_yPlaneMax->setInvertedAppearance(false);
			horizontalSlider_yPlaneMax->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_yPlaneMax->setTickInterval(25);

			horizontalLayout_yPlaneMax->addWidget(horizontalSlider_yPlaneMax);

			label_yPlaneMax_Max = new QLabel(groupBox_sections);
			label_yPlaneMax_Max->setObjectName(QString::fromUtf8("label_yPlaneMax_Max"));
			label_yPlaneMax_Max->setMinimumSize(QSize(30, 0));
			label_yPlaneMax_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_yPlaneMax->addWidget(label_yPlaneMax_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_yPlaneMax, 11, 2, 1, 1);

			horizontalLayout_zPlaneMin = new QHBoxLayout();
			horizontalLayout_zPlaneMin->setObjectName(QString::fromUtf8("horizontalLayout_zPlaneMin"));
			label_zPlaneMin_Min = new QLabel(groupBox_sections);
			label_zPlaneMin_Min->setObjectName(QString::fromUtf8("label_zPlaneMin_Min"));
			label_zPlaneMin_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_zPlaneMin->addWidget(label_zPlaneMin_Min);

			horizontalSlider_zPlaneMin = new QSlider(groupBox_sections);
			horizontalSlider_zPlaneMin->setObjectName(QString::fromUtf8("horizontalSlider_zPlaneMin"));
			horizontalSlider_zPlaneMin->setMinimum(-5000);
			horizontalSlider_zPlaneMin->setMaximum(5000);
			horizontalSlider_zPlaneMin->setValue(100);
			horizontalSlider_zPlaneMin->setTracking(false);
			horizontalSlider_zPlaneMin->setOrientation(Qt::Horizontal);
			horizontalSlider_zPlaneMin->setInvertedAppearance(false);
			horizontalSlider_zPlaneMin->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_zPlaneMin->setTickInterval(25);

			horizontalLayout_zPlaneMin->addWidget(horizontalSlider_zPlaneMin);

			label_zPlaneMin_Max = new QLabel(groupBox_sections);
			label_zPlaneMin_Max->setObjectName(QString::fromUtf8("label_zPlaneMin_Max"));
			label_zPlaneMin_Max->setMinimumSize(QSize(30, 0));
			label_zPlaneMin_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_zPlaneMin->addWidget(label_zPlaneMin_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_zPlaneMin, 12, 2, 1, 1);

			horizontalLayout_zPlaneMax = new QHBoxLayout();
			horizontalLayout_zPlaneMax->setObjectName(QString::fromUtf8("horizontalLayout_zPlaneMax"));
			label_zPlaneMax_Min = new QLabel(groupBox_sections);
			label_zPlaneMax_Min->setObjectName(QString::fromUtf8("label_zPlaneMax_Min"));
			label_zPlaneMax_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_zPlaneMax->addWidget(label_zPlaneMax_Min);

			horizontalSlider_zPlaneMax = new QSlider(groupBox_sections);
			horizontalSlider_zPlaneMax->setObjectName(QString::fromUtf8("horizontalSlider_zPlaneMax"));
			horizontalSlider_zPlaneMax->setMinimum(-5000);
			horizontalSlider_zPlaneMax->setMaximum(5000);
			horizontalSlider_zPlaneMax->setValue(100);
			horizontalSlider_zPlaneMax->setTracking(false);
			horizontalSlider_zPlaneMax->setOrientation(Qt::Horizontal);
			horizontalSlider_zPlaneMax->setInvertedAppearance(false);
			horizontalSlider_zPlaneMax->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_zPlaneMax->setTickInterval(25);

			horizontalLayout_zPlaneMax->addWidget(horizontalSlider_zPlaneMax);

			label_zPlaneMax_Max = new QLabel(groupBox_sections);
			label_zPlaneMax_Max->setObjectName(QString::fromUtf8("label_zPlaneMax_Max"));
			label_zPlaneMax_Max->setMinimumSize(QSize(30, 0));
			label_zPlaneMax_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_zPlaneMax->addWidget(label_zPlaneMax_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_zPlaneMax, 13, 2, 1, 1);

			doubleSpinBox_yPlaneMax = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_yPlaneMax->setObjectName(QString::fromUtf8("doubleSpinBox_yPlaneMax"));
			doubleSpinBox_yPlaneMax->setMinimumSize(QSize(70, 0));
			doubleSpinBox_yPlaneMax->setMouseTracking(true);
			doubleSpinBox_yPlaneMax->setMinimum(-5000);
			doubleSpinBox_yPlaneMax->setMaximum(5000);
			doubleSpinBox_yPlaneMax->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_yPlaneMax, 11, 1, 1, 1);

			doubleSpinBox_zPlaneMin = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_zPlaneMin->setObjectName(QString::fromUtf8("doubleSpinBox_zPlaneMin"));
			doubleSpinBox_zPlaneMin->setMinimumSize(QSize(70, 0));
			doubleSpinBox_zPlaneMin->setMouseTracking(true);
			doubleSpinBox_zPlaneMin->setMinimum(-5000);
			doubleSpinBox_zPlaneMin->setMaximum(5000);
			doubleSpinBox_zPlaneMin->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_zPlaneMin, 12, 1, 1, 1);

			doubleSpinBox_zPlaneMax = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_zPlaneMax->setObjectName(QString::fromUtf8("doubleSpinBox_zPlaneMax"));
			doubleSpinBox_zPlaneMax->setMinimumSize(QSize(70, 0));
			doubleSpinBox_zPlaneMax->setMouseTracking(true);
			doubleSpinBox_zPlaneMax->setMinimum(-5000);
			doubleSpinBox_zPlaneMax->setMaximum(5000);
			doubleSpinBox_zPlaneMax->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_zPlaneMax, 13, 1, 1, 1);

			pushButton_xPlaneMin = new QPushButton(groupBox_sections);
			pushButton_xPlaneMin->setObjectName(QString::fromUtf8("pushButton_xPlaneMin"));
			pushButton_xPlaneMin->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(pushButton_xPlaneMin, 8, 0, 1, 1);

			pushButton_xPlaneMax = new QPushButton(groupBox_sections);
			pushButton_xPlaneMax->setObjectName(QString::fromUtf8("pushButton_xPlaneMax"));
			pushButton_xPlaneMax->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(pushButton_xPlaneMax, 9, 0, 1, 1);

			pushButton_yPlaneMin = new QPushButton(groupBox_sections);
			pushButton_yPlaneMin->setObjectName(QString::fromUtf8("pushButton_yPlaneMin"));
			pushButton_yPlaneMin->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(pushButton_yPlaneMin, 10, 0, 1, 1);

			pushButton_yPlaneMax = new QPushButton(groupBox_sections);
			pushButton_yPlaneMax->setObjectName(QString::fromUtf8("pushButton_yPlaneMax"));
			pushButton_yPlaneMax->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(pushButton_yPlaneMax, 11, 0, 1, 1);

			pushButton_zPlaneMin = new QPushButton(groupBox_sections);
			pushButton_zPlaneMin->setObjectName(QString::fromUtf8("pushButton_zPlaneMin"));
			pushButton_zPlaneMin->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(pushButton_zPlaneMin, 12, 0, 1, 1);

			pushButton_zPlaneMax = new QPushButton(groupBox_sections);
			pushButton_zPlaneMax->setObjectName(QString::fromUtf8("pushButton_zPlaneMax"));
			pushButton_zPlaneMax->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(pushButton_zPlaneMax, 13, 0, 1, 1);

			label_angleMax = new QLabel(groupBox_sections);
			label_angleMax->setObjectName(QString::fromUtf8("label_angleMax"));
			label_angleMax->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(label_angleMax, 16, 0, 1, 1);

			line_sections2 = new QFrame(groupBox_sections);
			line_sections2->setObjectName(QString::fromUtf8("line_sections2"));
			line_sections2->setFrameShape(QFrame::HLine);
			line_sections2->setFrameShadow(QFrame::Sunken);

			gridLayout_sectionPlanes->addWidget(line_sections2, 14, 0, 1, 3);

			doubleSpinBox_angleMax = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_angleMax->setObjectName(QString::fromUtf8("doubleSpinBox_angleMax"));
			doubleSpinBox_angleMax->setMinimumSize(QSize(70, 0));
			doubleSpinBox_angleMax->setMouseTracking(true);
			doubleSpinBox_angleMax->setMinimum(-360);
			doubleSpinBox_angleMax->setMaximum(360);
			doubleSpinBox_angleMax->setValue(45);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_angleMax, 16, 1, 1, 1);

			horizontalLayout_angleMax = new QHBoxLayout();
			horizontalLayout_angleMax->setObjectName(QString::fromUtf8("horizontalLayout_angleMax"));
			label_angleMin_Min = new QLabel(groupBox_sections);
			label_angleMin_Min->setObjectName(QString::fromUtf8("label_angleMin_Min"));
			label_angleMin_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_angleMax->addWidget(label_angleMin_Min);

			horizontalSlider_angleMax = new QSlider(groupBox_sections);
			horizontalSlider_angleMax->setObjectName(QString::fromUtf8("horizontalSlider_angleMin"));
			horizontalSlider_angleMax->setMinimum(-360);
			horizontalSlider_angleMax->setMaximum(360);
			horizontalSlider_angleMax->setValue(45);
			horizontalSlider_angleMax->setSliderPosition(45);
			horizontalSlider_angleMax->setTracking(false);
			horizontalSlider_angleMax->setOrientation(Qt::Horizontal);
			horizontalSlider_angleMax->setInvertedAppearance(false);
			horizontalSlider_angleMax->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_angleMax->setTickInterval(25);

			horizontalLayout_angleMax->addWidget(horizontalSlider_angleMax);

			label_angleMin_Max = new QLabel(groupBox_sections);
			label_angleMin_Max->setObjectName(QString::fromUtf8("label_angleMin_Max"));
			label_angleMin_Max->setMinimumSize(QSize(30, 0));
			label_angleMin_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_angleMax->addWidget(label_angleMin_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_angleMax, 16, 2, 1, 1);

			label_angleMin = new QLabel(groupBox_sections);
			label_angleMin->setObjectName(QString::fromUtf8("label_angleMin"));
			label_angleMin->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(label_angleMin, 17, 0, 1, 1);

			doubleSpinBox_angleMin = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_angleMin->setObjectName(QString::fromUtf8("doubleSpinBox_angleMin"));
			doubleSpinBox_angleMin->setMinimumSize(QSize(70, 0));
			doubleSpinBox_angleMin->setMouseTracking(true);
			doubleSpinBox_angleMin->setMinimum(-360);
			doubleSpinBox_angleMin->setMaximum(360);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_angleMin, 17, 1, 1, 1);

			horizontalLayout_angleMin = new QHBoxLayout();
			horizontalLayout_angleMin->setObjectName(QString::fromUtf8("horizontalLayout_angleMin"));
			label_AngleMax_Min = new QLabel(groupBox_sections);
			label_AngleMax_Min->setObjectName(QString::fromUtf8("label_AngleMax_Min"));
			label_AngleMax_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_angleMin->addWidget(label_AngleMax_Min);

			horizontalSlider_angleMin = new QSlider(groupBox_sections);
			horizontalSlider_angleMin->setObjectName(QString::fromUtf8("horizontalSlider_angleMax"));
			horizontalSlider_angleMin->setMinimum(-360);
			horizontalSlider_angleMin->setMaximum(360);
			horizontalSlider_angleMin->setTracking(false);
			horizontalSlider_angleMin->setOrientation(Qt::Horizontal);
			horizontalSlider_angleMin->setInvertedAppearance(false);
			horizontalSlider_angleMin->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_angleMin->setTickInterval(25);

			horizontalLayout_angleMin->addWidget(horizontalSlider_angleMin);

			label_angleMax_Max = new QLabel(groupBox_sections);
			label_angleMax_Max->setObjectName(QString::fromUtf8("label_angleMax_Max"));
			label_angleMax_Max->setMinimumSize(QSize(30, 0));
			label_angleMax_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_angleMin->addWidget(label_angleMax_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_angleMin, 17, 2, 1, 1);

			label_radius = new QLabel(groupBox_sections);
			label_radius->setObjectName(QString::fromUtf8("label_radius"));
			label_radius->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(label_radius, 18, 0, 1, 1);

			doubleSpinBox_radius = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_radius->setObjectName(QString::fromUtf8("doubleSpinBox_Radius"));
			doubleSpinBox_radius->setMinimumSize(QSize(70, 0));
			doubleSpinBox_radius->setMouseTracking(true);
			doubleSpinBox_radius->setMaximum(10000);
			doubleSpinBox_radius->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_radius, 18, 1, 1, 1);

			horizontalLayout_radius = new QHBoxLayout();
			horizontalLayout_radius->setObjectName(QString::fromUtf8("horizontalLayout_radius"));
			label_radius_Min = new QLabel(groupBox_sections);
			label_radius_Min->setObjectName(QString::fromUtf8("label_radius_Min"));
			label_radius_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_radius->addWidget(label_radius_Min);

			horizontalSlider_radius = new QSlider(groupBox_sections);
			horizontalSlider_radius->setObjectName(QString::fromUtf8("horizontalSlider_radius"));
			horizontalSlider_radius->setMaximum(1000);
			horizontalSlider_radius->setValue(100);
			horizontalSlider_radius->setSliderPosition(100);
			horizontalSlider_radius->setTracking(false);
			horizontalSlider_radius->setOrientation(Qt::Horizontal);
			horizontalSlider_radius->setInvertedAppearance(false);
			horizontalSlider_radius->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_radius->setTickInterval(25);

			horizontalLayout_radius->addWidget(horizontalSlider_radius);

			label_radius_Max = new QLabel(groupBox_sections);
			label_radius_Max->setObjectName(QString::fromUtf8("label_radius_Max"));
			label_radius_Max->setMinimumSize(QSize(30, 0));
			label_radius_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_radius->addWidget(label_radius_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_radius, 18, 2, 1, 1);

			label_strafeSections = new QLabel(groupBox_sections);
			label_strafeSections->setObjectName(QString::fromUtf8("label_strafeSections"));
			label_strafeSections->setMaximumSize(QSize(16777215, 30));

			gridLayout_sectionPlanes->addWidget(label_strafeSections, 20, 0, 1, 1);

			horizontalLayout_strafeSections = new QHBoxLayout();
			horizontalLayout_strafeSections->setObjectName(QString::fromUtf8("horizontalLayout_strafeSections"));
			horizontalLayout_strafeSections->setSizeConstraint(QLayout::SetDefaultConstraint);
			label_strafeXSections = new QLabel(groupBox_sections);
			label_strafeXSections->setObjectName(QString::fromUtf8("label_strafeXSections"));

			horizontalLayout_strafeSections->addWidget(label_strafeXSections);

			doubleSpinBox_strafeXSections = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_strafeXSections->setObjectName(QString::fromUtf8("doubleSpinBox_strafeXSections"));
			doubleSpinBox_strafeXSections->setMinimumSize(QSize(70, 0));
			doubleSpinBox_strafeXSections->setMouseTracking(true);
			doubleSpinBox_strafeXSections->setMaximum(5000);
			doubleSpinBox_strafeXSections->setMinimum(-5000);

			horizontalLayout_strafeSections->addWidget(doubleSpinBox_strafeXSections);

			label_strafeYSections = new QLabel(groupBox_sections);
			label_strafeYSections->setObjectName(QString::fromUtf8("label_strafeYSections"));

			horizontalLayout_strafeSections->addWidget(label_strafeYSections);

			doubleSpinBox_strafeYSections = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_strafeYSections->setObjectName(QString::fromUtf8("doubleSpinBox_strafeYSections"));
			doubleSpinBox_strafeYSections->setMinimumSize(QSize(70, 0));
			doubleSpinBox_strafeYSections->setMouseTracking(true);
			doubleSpinBox_strafeYSections->setMaximum(5000);
			doubleSpinBox_strafeYSections->setMinimum(-5000);

			horizontalLayout_strafeSections->addWidget(doubleSpinBox_strafeYSections);

			label_strafeZSections = new QLabel(groupBox_sections);
			label_strafeZSections->setObjectName(QString::fromUtf8("label_strafeZSections"));

			horizontalLayout_strafeSections->addWidget(label_strafeZSections);

			doubleSpinBox_strafeZSections = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_strafeZSections->setObjectName(QString::fromUtf8("doubleSpinBox_strafeZSections"));
			doubleSpinBox_strafeZSections->setMinimumSize(QSize(70, 0));
			doubleSpinBox_strafeZSections->setMouseTracking(true);
			doubleSpinBox_strafeZSections->setMaximum(5000);
			doubleSpinBox_strafeZSections->setMinimum(-5000);

			horizontalLayout_strafeSections->addWidget(doubleSpinBox_strafeZSections);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_strafeSections, 20, 1, 1, 2);

			line_sections3 = new QFrame(groupBox_sections);
			line_sections3->setObjectName(QString::fromUtf8("line_sections3"));
			line_sections3->setFrameShape(QFrame::HLine);
			line_sections3->setFrameShadow(QFrame::Sunken);

			gridLayout_sectionPlanes->addWidget(line_sections3, 7, 0, 1, 3);

			horizontalLayout_sectionOptions = new QHBoxLayout();
			horizontalLayout_sectionOptions->setObjectName(QString::fromUtf8("horizontalLayout_sectionOptions"));
			checkBox_useSections = new QCheckBox(groupBox_sections);
			checkBox_useSections->setObjectName(QString::fromUtf8("checkBox_useSections"));
			checkBox_useSections->setMaximumSize(QSize(110, 60));
			checkBox_useSections->setChecked(true);
			checkBox_useSections->setTristate(false);

			horizontalLayout_sectionOptions->addWidget(checkBox_useSections);

			checkBox_limitedBB = new QCheckBox(groupBox_sections);
			checkBox_limitedBB->setObjectName(QString::fromUtf8("checkBox_limitedBB"));
			checkBox_limitedBB->setChecked(true);

			horizontalLayout_sectionOptions->addWidget(checkBox_limitedBB);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_sectionOptions, 5, 0, 1, 3);

			checkBox_usePiece = new QCheckBox(groupBox_sections);
			checkBox_usePiece->setObjectName(QString::fromUtf8("checkBox_usePiece"));
			checkBox_usePiece->setMaximumSize(QSize(110, 60));
			checkBox_usePiece->setChecked(false);
			checkBox_usePiece->setTristate(false);



			horizontalLayout_piece = new QHBoxLayout();
			horizontalLayout_piece->setObjectName(QString::fromUtf8("horizontalLayout_piece"));
			radioButton_XY = new QRadioButton(groupBox_sections);
			radioButton_XY->setObjectName(QString::fromUtf8("radioButton_XY"));
			radioButton_XY->setMaximumSize(QSize(50, 16777215));
			radioButton_XY->setChecked(true);

			horizontalLayout_piece->addWidget(radioButton_XY);

			radioButton_YZ = new QRadioButton(groupBox_sections);
			radioButton_YZ->setObjectName(QString::fromUtf8("radioButton_YZ"));
			radioButton_YZ->setMaximumSize(QSize(50, 16777215));

			horizontalLayout_piece->addWidget(radioButton_YZ);

			radioButton_XZ = new QRadioButton(groupBox_sections);
			radioButton_XZ->setObjectName(QString::fromUtf8("radioButton_XZ"));
			radioButton_XZ->setMaximumSize(QSize(16777215, 16777215));

			horizontalLayout_piece->addWidget(radioButton_XZ);

			gridLayout_sectionPlanes->addLayout(horizontalLayout_piece, 15, 1, 1, 2);

			label_height = new QLabel(groupBox_sections);
			label_height->setObjectName(QString::fromUtf8("label_height"));
			label_height->setMinimumSize(QSize(30, 30));

			gridLayout_sectionPlanes->addWidget(label_height, 19, 0, 1, 1);

			doubleSpinBox_height = new QDoubleSpinBox(groupBox_sections);
			doubleSpinBox_height->setObjectName(QString::fromUtf8("doubleSpinBox_height"));
			doubleSpinBox_height->setMinimumSize(QSize(70, 0));
			doubleSpinBox_height->setMouseTracking(true);
			doubleSpinBox_height->setMaximum(10000);
			doubleSpinBox_height->setValue(100);

			gridLayout_sectionPlanes->addWidget(doubleSpinBox_height, 19, 1, 1, 1);

			horizontalLayout_height = new QHBoxLayout();
			horizontalLayout_height->setObjectName(QString::fromUtf8("horizontalLayout_height"));
			label_height_Min = new QLabel(groupBox_sections);
			label_height_Min->setObjectName(QString::fromUtf8("label_height_Min"));
			label_height_Min->setMinimumSize(QSize(30, 0));

			horizontalLayout_height->addWidget(label_height_Min);

			horizontalSlider_height = new QSlider(groupBox_sections);
			horizontalSlider_height->setObjectName(QString::fromUtf8("horizontalSlider_height"));
			horizontalSlider_height->setMaximum(1000);
			horizontalSlider_height->setValue(100);
			horizontalSlider_height->setSliderPosition(100);
			horizontalSlider_height->setTracking(false);
			horizontalSlider_height->setOrientation(Qt::Horizontal);
			horizontalSlider_height->setInvertedAppearance(false);
			horizontalSlider_height->setTickPosition(QSlider::TicksBothSides);
			horizontalSlider_height->setTickInterval(25);

			horizontalLayout_height->addWidget(horizontalSlider_height);

			label_height_Max = new QLabel(groupBox_sections);
			label_height_Max->setObjectName(QString::fromUtf8("label_height_Max"));
			label_height_Max->setMinimumSize(QSize(30, 0));
			label_height_Max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

			horizontalLayout_height->addWidget(label_height_Max);


			gridLayout_sectionPlanes->addLayout(horizontalLayout_height, 19, 2, 1, 1);

			gridLayout_sectionPlanes->addWidget(checkBox_usePiece, 15, 0, 1, 1);





			verticalLayout_sections->addLayout(gridLayout_sectionPlanes);


			gridLayout_settings->addWidget(groupBox_sections, 0, 1, 2, 1);

			retranslateUi(parent);
			
		}


		void retranslateUi(QWidget *parent)
		{
			groupBox_camera->setTitle(QApplication::translate("parent", "Camera", 0, QApplication::UnicodeUTF8));
			label_elevation->setText(QApplication::translate("parent", "Elevation:", 0, QApplication::UnicodeUTF8));
			label_azimuth->setText(QApplication::translate("parent", "Azimuth:", 0, QApplication::UnicodeUTF8));
			label_strafe->setText(QApplication::translate("parent", "Strafe", 0, QApplication::UnicodeUTF8));
			label_strafeX->setText(QApplication::translate("parent", "X:", 0, QApplication::UnicodeUTF8));
			label_strafeY->setText(QApplication::translate("parent", "Y:", 0, QApplication::UnicodeUTF8));
			label_strafeZ->setText(QApplication::translate("parent", "Z:", 0, QApplication::UnicodeUTF8));
			label_distanceMin->setText(QApplication::translate("parent", "0", 0, QApplication::UnicodeUTF8));
			label_dinstanceMax->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			label_distance->setText(QApplication::translate("parent", "Distance:", 0, QApplication::UnicodeUTF8));
			checkBox_orthographic->setText(QApplication::translate("parent", "Use Orthographic", 0, QApplication::UnicodeUTF8));
			groupBox_sections->setTitle(QApplication::translate("parent", "Sections", 0, QApplication::UnicodeUTF8));
			pushButton_xPlane0->setText(QApplication::translate("parent", "PX", 0, QApplication::UnicodeUTF8));
			pushButton_yPlane0->setText(QApplication::translate("parent", "PY", 0, QApplication::UnicodeUTF8));
			pushButton_zPlane0->setText(QApplication::translate("parent", "PZ", 0, QApplication::UnicodeUTF8));
			checkBox_useSections->setText(QApplication::translate("parent", "Use Sections", 0, QApplication::UnicodeUTF8));
			checkBox_limitedBB->setText(QApplication::translate("parent", "Limited by bounding box", 0, QApplication::UnicodeUTF8));
			label_sectionsBase->setText(QApplication::translate("Form", "   Base: ", 0, QApplication::UnicodeUTF8));
			label_sectionsDistance->setText(QApplication::translate("Form", "  Distance: ", 0, QApplication::UnicodeUTF8));
			label_originY->setText(QApplication::translate("Form", "   Origin X:", 0, QApplication::UnicodeUTF8));
			label_originX->setText(QApplication::translate("Form", "Origin Y:", 0, QApplication::UnicodeUTF8));
			label_originZ->setText(QApplication::translate("Form", "Origin Z:", 0, QApplication::UnicodeUTF8));
			label_xPlaneMin_Min->setText(QApplication::translate("parent", "-5000", 0, QApplication::UnicodeUTF8));
			label_xPlaneMin_Max->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			label_xPlaneMax_Min->setText(QApplication::translate("parent", "-5000", 0, QApplication::UnicodeUTF8));
			label_xPlaneMax_Max->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			label_yPlaneMin_Min->setText(QApplication::translate("parent", "-5000", 0, QApplication::UnicodeUTF8));
			label_yPlaneMin_Max->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			label_yPlaneMax_Min->setText(QApplication::translate("parent", "-5000", 0, QApplication::UnicodeUTF8));
			label_yPlaneMax_Max->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			label_zPlaneMin_Min->setText(QApplication::translate("parent", "-5000", 0, QApplication::UnicodeUTF8));
			label_zPlaneMin_Max->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			label_zPlaneMax_Min->setText(QApplication::translate("parent", "-5000", 0, QApplication::UnicodeUTF8));
			label_zPlaneMax_Max->setText(QApplication::translate("parent", "5000", 0, QApplication::UnicodeUTF8));
			pushButton_xPlaneMin->setText(QApplication::translate("parent", "X Min", 0, QApplication::UnicodeUTF8));
			pushButton_xPlaneMax->setText(QApplication::translate("parent", "X Max", 0, QApplication::UnicodeUTF8));
			pushButton_yPlaneMin->setText(QApplication::translate("parent", "Y Min", 0, QApplication::UnicodeUTF8));
			pushButton_yPlaneMax->setText(QApplication::translate("parent", "Y Max", 0, QApplication::UnicodeUTF8));
			pushButton_zPlaneMin->setText(QApplication::translate("parent", "Z Min", 0, QApplication::UnicodeUTF8));
			pushButton_zPlaneMax->setText(QApplication::translate("parent", "Z Max", 0, QApplication::UnicodeUTF8));
			label_angleMax->setText(QApplication::translate("Form", "   Angle Max", 0, QApplication::UnicodeUTF8));
			label_angleMin_Min->setText(QApplication::translate("Form", "-360", 0, QApplication::UnicodeUTF8));
			label_angleMin_Max->setText(QApplication::translate("Form", "360", 0, QApplication::UnicodeUTF8));
			label_angleMin->setText(QApplication::translate("Form", "   Angle Min", 0, QApplication::UnicodeUTF8));
			label_AngleMax_Min->setText(QApplication::translate("Form", "-360", 0, QApplication::UnicodeUTF8));
			label_angleMax_Max->setText(QApplication::translate("Form", "360", 0, QApplication::UnicodeUTF8));
			label_radius->setText(QApplication::translate("Form", "   Radius", 0, QApplication::UnicodeUTF8));
			label_radius_Min->setText(QApplication::translate("Form", "0", 0, QApplication::UnicodeUTF8));
			label_radius_Max->setText(QApplication::translate("Form", "1000", 0, QApplication::UnicodeUTF8));
			label_strafeSections->setText(QApplication::translate("Form", "   Strafe", 0, QApplication::UnicodeUTF8));
			label_strafeXSections->setText(QApplication::translate("Form", "X:", 0, QApplication::UnicodeUTF8));
			label_strafeYSections->setText(QApplication::translate("Form", "Y:", 0, QApplication::UnicodeUTF8));
			label_strafeZSections->setText(QApplication::translate("Form", "Z:", 0, QApplication::UnicodeUTF8));
			checkBox_usePiece->setText(QApplication::translate("Form", "Piece", 0, QApplication::UnicodeUTF8));
			radioButton_XY->setText(QApplication::translate("Form", "XY", 0, QApplication::UnicodeUTF8));
			radioButton_YZ->setText(QApplication::translate("Form", "YZ", 0, QApplication::UnicodeUTF8));
			radioButton_XZ->setText(QApplication::translate("Form", "XZ", 0, QApplication::UnicodeUTF8));
			label_height->setText(QApplication::translate("Form", "   Height", 0, QApplication::UnicodeUTF8));
			label_height_Min->setText(QApplication::translate("Form", "0", 0, QApplication::UnicodeUTF8));
			label_height_Max->setText(QApplication::translate("Form", "1000", 0, QApplication::UnicodeUTF8));
		} // retranslateUi


	public slots:

		void onRadioButtonXYChanged(bool state)
		{
			if (state)
			{
				radioButton_YZ->setChecked(false);
				radioButton_XZ->setChecked(false);
				emit usePieceChanged(checkBox_usePiece->isChecked());
			}
		}

		void onRadioButtonYZChanged(bool state)
		{
			if (state)
			{
				radioButton_XY->setChecked(false);
				radioButton_XZ->setChecked(false);
				emit usePieceChanged(checkBox_usePiece->isChecked());
			}
		}

		void onRadioButtonXZChanged(bool state)
		{
			if (state)
			{
				radioButton_YZ->setChecked(false);
				radioButton_XY->setChecked(false);
				emit usePieceChanged(checkBox_usePiece->isChecked());
			}
		}

		void onHorizontalSlider_distance(int val)
		{
			doubleSpinBox_distance->setValue(val);
		}
		void onDoubleSpinBox_distance(double val)
		{
			horizontalSlider_distance->setValue(val);
		}

		void onDoubleSpinBox_xPlaneMin(double val)
		{
			horizontalSlider_xPlaneMin->setValue(val);
		}
		void onDoubleSpinBox_xPlaneMax(double val)
		{
			horizontalSlider_xPlaneMax->setValue(val);
		}
		void onDoubleSpinBox_yPlaneMin(double val)
		{
			horizontalSlider_yPlaneMin->setValue(val);
		}
		void onDoubleSpinBox_yPlaneMax(double val)
		{
			horizontalSlider_yPlaneMax->setValue(val);
		}
		void onDoubleSpinBox_zPlaneMin(double val)
		{
			horizontalSlider_zPlaneMin->setValue(val);
		}
		void onDoubleSpinBox_zPlaneMax(double val)
		{
			horizontalSlider_zPlaneMax->setValue(val);
		}
		void onHorizontalSlider_xPlaneMin(int val)
		{
			doubleSpinBox_xPlaneMin->setValue(val);
		}
		void onHorizontalSlider_xPlaneMax(int val)
		{
			doubleSpinBox_xPlaneMax->setValue(val);
		}
		void onHorizontalSlider_yPlaneMin(int val)
		{
			doubleSpinBox_yPlaneMin->setValue(val);
		}
		void onHorizontalSlider_yPlaneMax(int val)
		{
			doubleSpinBox_yPlaneMax->setValue(val);
		}
		void onHorizontalSlider_zPlaneMin(int val)
		{
			doubleSpinBox_zPlaneMin->setValue(val);
		}
		void onHorizontalSlider_zPlaneMax(int val)
		{
			doubleSpinBox_zPlaneMax->setValue(val);
		}

		void onDoubleSpinBox_angleMax(double val)
		{
			horizontalSlider_angleMax->setValue(val);
		}
		void onHorizontalSlider_angleMax(int val)
		{
			doubleSpinBox_angleMax->setValue(val);
		}
		void onDoubleSpinBox_angleMin(double val)
		{
			horizontalSlider_angleMin->setValue(val);
		}
		void onHorizontalSlider_angleMin(int val)
		{
			doubleSpinBox_angleMin->setValue(val);
		}
		void onDoubleSpinBox_radius(double val)
		{
			horizontalSlider_radius->setValue(val);
		}
		void onHorizontalSlider_radius(int val)
		{
			doubleSpinBox_radius->setValue(val);
		}
		void onDoubleSpinBox_height(double val)
		{
			horizontalSlider_height->setValue(val);
		}
		void onHorizontalSlider_height(int val)
		{
			doubleSpinBox_height->setValue(val);
		}
		


	signals:
		void usePieceChanged(int state);
	

};

namespace Ui {
    class MCNPXSceneEditor: public Ui_MCNPXSceneEditor {};
} // namespace Ui


#endif //
