//#######################################################################################################################
//## MCNPXVisualizer.cpp
//#######################################################################################################################
//##
//## Main GUI Class
//## Provides all the GUI functionality to read/parse/build MNCPX input files and to render 
//## it with povray.
//##
//## Usage:
//##	When opening a mcnpx file. The GUI will automatically calls the python preparser code.
//##	The preparser will parse all the basic information out of the file (cells, surfaces, materials, universes, ...)
//##	Based on this information, the user can set the rendering options (quality, camera, sections)
//##	Click on the render button to start rendering the scene. The rendered image will be
//##	visible in the central widget when it is ready.
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#######################################################################################################################
#include "MCNPXVisualizer.h"

#include <iostream>

#include <QtGui>
#include <QPixmap>
#include <QThread>
#include <QDialog>
#include <QString>
#include <QRegExp>
#include <QTextStream>
#include <QThread>

#include "Config.h"
#include "RenderManager.h"
#include "IniManager.h"

#define PI 3.14159265

//####################################################################
//#  INITIALIZATION
//####################################################################


// ==> MCNPXVisualizer()
// Constructor
//--------------------------------------------------------------------
MCNPXVisualizer::MCNPXVisualizer(QApplication* app)
{
	cthread = thread();		// store the main gui thread (used for moveToThread(cthread) out of a non-gui thread)

	Config* config = new Config();

	// Managers
	IniManager* iniManager = new IniManager();
	CameraManager* cameraManager = new CameraManager();
	CameraManager::getSingletonPtr()->createPovRayFile(QString::fromStdString(Config::getSingleton().TEMP));

	_renderManager = new RenderManager(QString::fromStdString(Config::getSingleton().TEMP) + "combined.pov", QString::fromStdString(Config::getSingleton().TEMP) + "output.png", 800, 600, 2);
	connect(_renderManager, SIGNAL(finishedRendering(bool)), this, SLOT(finishedRendering(bool)));
	connect(_renderManager, SIGNAL(onRendererCallOutput(QString, PovRayRendererInformation, bool)), this, SLOT(onPovrayOutput(QString, PovRayRendererInformation, bool)));

	_pythonBinder = new PythonBinder();
	connect(_pythonBinder, SIGNAL(pythonCallFinished(QString)), this, SLOT(finishedParsing(QString)));
	connect(_pythonBinder, SIGNAL(pythonCallOutput(QString, QString, bool)), this, SLOT(onPythonOutput(QString, QString, bool)));

	initializeGUI();

	setWindowTitle(tr("MCNPX Visualizer"));
	resize(1124, 768);

	_extentH = 100.0;
	_extentV = 100.0;
	_originX = 0.0;
	_originY = 0.0;
	_originZ = 0.0;

	_currentColorIndex = 0;
	loadStandardColors();

	QFile file(QString::fromStdString(Config::getSingleton().TEMP) + "state");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QByteArray line = file.readLine();
		QMainWindow::restoreState(line);
		file.close();
	}
	else
	{
		QFile fileDefault(QString::fromStdString(Config::getSingleton().DATA) + "defaultState");
		if (fileDefault.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QByteArray lineDefault = fileDefault.readLine();
			QMainWindow::restoreState(lineDefault);
			fileDefault.close();
		}
	}
}

//####################################################################
//#  STARTUP GUI
//####################################################################

// ==> initializeGUI()
// Does the basic GUI initialization
// Setup the main GUI view + does all the connections with the appropriate slots
//--------------------------------------------------------------------
void MCNPXVisualizer::initializeGUI()
{
	// CENTRAL TABS
	//--------------------------------------------------------------------

	// Rendering => central viewing widget for the renderen image
	imageLabel = new QLabel;
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);	
	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);

	// Scene Editor => gets all the functionality to alter the view of the scene (camera placement / cross sections / orthographic projection)
	UiMCNPXSceneEditor.setupUi(this);
	connect(UiMCNPXSceneEditor.doubleSpinBox_azimuth, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onAzimuthChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_elevation, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onElevationChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_distance, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onDistanceChanged(double)));
	
	connect(UiMCNPXSceneEditor.dial_azimuth, SIGNAL(valueChanged(int)), &UiMCNPXScene, SLOT(onAzimuthChanged(int)));
	connect(UiMCNPXSceneEditor.dial_elevation, SIGNAL(valueChanged(int)), &UiMCNPXScene, SLOT(onElevationChanged(int)));
	

	

	connect(UiMCNPXSceneEditor.checkBox_useSections, SIGNAL(stateChanged(int)), this, SLOT(onSectionsEnabledChanged(int)));
	connect(UiMCNPXSceneEditor.checkBox_limitedBB, SIGNAL(stateChanged(int)), this, SLOT(onLimitedPlanesChanged(int)));

	connect(UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onZPlaneMinChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onZPlaneMaxChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onXPlaneMinChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onXPlaneMaxChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onYPlaneMinChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onYPlaneMaxChanged(double)));

	connect(UiMCNPXSceneEditor.doubleSpinBox_angleMin, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onAngleMinChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_angleMax, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onAngleMaxChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_height, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onHeightChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_radius, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onRadiusChanged(double)));

	connect(UiMCNPXSceneEditor.checkBox_usePiece, SIGNAL(stateChanged(int)), this, SLOT(onCheckBox_usePieceChanged(int)));
	connect(&UiMCNPXSceneEditor, SIGNAL(usePieceChanged(int)), this, SLOT(onCheckBox_usePieceChanged(int)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_strafeXSections, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onStrafeXChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_strafeYSections, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onStrafeYChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_strafeZSections, SIGNAL(valueChanged(double)), &UiMCNPXScene, SLOT(onStrafeZChanged(double)));


	// Orthographic cross sections for x y z
	connect(UiMCNPXSceneEditor.pushButton_xPlane0, SIGNAL(pressed()), this, SLOT(onPX()));
	connect(UiMCNPXSceneEditor.pushButton_yPlane0, SIGNAL(pressed()), this, SLOT(onPY()));
	connect(UiMCNPXSceneEditor.pushButton_zPlane0, SIGNAL(pressed()), this, SLOT(onPZ()));

	connect(UiMCNPXSceneEditor.sectionDistance, SIGNAL(valueChanged(double)), this, SLOT(onExtentChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_originX, SIGNAL(valueChanged(double)), this, SLOT(onOriginXChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_originY, SIGNAL(valueChanged(double)), this, SLOT(onOriginYChanged(double)));
	connect(UiMCNPXSceneEditor.doubleSpinBox_originZ, SIGNAL(valueChanged(double)), this, SLOT(onOriginZChanged(double)));
	
	// Orthographic cross sections for bounding box
	connect(UiMCNPXSceneEditor.pushButton_xPlaneMin, SIGNAL(pressed()), this, SLOT(onXPlaneMin()));
	connect(UiMCNPXSceneEditor.pushButton_xPlaneMax, SIGNAL(pressed()), this, SLOT(onXPlaneMax()));
	connect(UiMCNPXSceneEditor.pushButton_yPlaneMin, SIGNAL(pressed()), this, SLOT(onYPlaneMin()));
	connect(UiMCNPXSceneEditor.pushButton_yPlaneMax, SIGNAL(pressed()), this, SLOT(onYPlaneMax()));
	connect(UiMCNPXSceneEditor.pushButton_zPlaneMin, SIGNAL(pressed()), this, SLOT(onZPlaneMin()));
	connect(UiMCNPXSceneEditor.pushButton_zPlaneMax, SIGNAL(pressed()), this, SLOT(onZPlaneMax()));
	

	// MXNPX Editor => widget for the mcnpx input file
	textEditMCNPX = new QTextEdit;

	// POV Ray Editor => widget for the builded output pov ray file
	textEditPOV = new QTextEdit;

	// Output => python console window for the parsing
	textEditOutput = new QTextEdit;
	this->textEditOutput->setReadOnly(true);

	// Povray output => povray console window for the povray rendering
	textEditPovRayOutput = new QTextEdit;
	textEditPovRayOutput->setReadOnly(true);

	// Build the tabwidget and add all the widgets
	tabWidget = new QTabWidget(this);
	tabWidget->addTab(scrollArea, QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "render.png"), QString("Rendering"));
	tabWidget->addTab(UiMCNPXSceneEditor.mcnpxSceneEditorLayOutWidget, QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "MCNPX.png"), QString("Scene Editor"));
	tabWidget->addTab(textEditMCNPX, QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "MCNPX.png"), QString("MCNPX Editor"));
	tabWidget->addTab(textEditPOV, QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "POV-Ray.Scene.png"), QString("POV Ray Editor"));
	tabWidget->addTab(textEditOutput, QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "Parser.png"), QString("Output"));
	tabWidget->addTab(textEditPovRayOutput, QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "POV-Ray.Include.png"), QString("Povray Output"));
	setCentralWidget(tabWidget);

	// DOCK WIDGETS
	//--------------------------------------------------------------------
	createDockWidgets();

	// MENU
	//--------------------------------------------------------------------
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
}

// ==> createDockWidgets()
// Setup the main QDockWidgets for the different properties (materials, surfaces, cells, universes, scene, ...)
//--------------------------------------------------------------------
void MCNPXVisualizer::createDockWidgets()
{
	// RENDER OPTIONS
	//		=> contains: resolution, quality, antialiasing, trace depth, snapshots, ...
	//----------------------------------------------------------------
	renderOptionsWidget = new QDockWidget("Render Options", this);
	renderOptionsWidget->setObjectName("Render Options");
	UiRenderOptions.setupUi(renderOptionsWidget);
	renderOptionsWidget->setWidget(UiRenderOptions.rendererFormLayOutWidget);
	this->addDockWidget(Qt::RightDockWidgetArea, renderOptionsWidget);
	connect(UiRenderOptions.snapPushButton, SIGNAL(pressed()), this, SLOT(onSnapShot()));

	// SURFACE CARDS
	//		=> contains basic information of the preparsed surface cards (nr, mnemonic, entries) 
	//----------------------------------------------------------------
	surfaceCardsWidget = new QDockWidget("Surface Cards", this);
	surfaceCardsWidget->setObjectName("Surface Cards");
	UiSurfaceCards.setupUi(surfaceCardsWidget);
	surfaceCardsWidget->setWidget(UiSurfaceCards.parserFormLayOutWidget);
	this->addDockWidget(Qt::LeftDockWidgetArea, surfaceCardsWidget);

	// CELL CARDS
	//		=> contains basic information of the preparsed cell cards (nr, material, density, geometry, params) 
	//----------------------------------------------------------------
	cellCardsWidget = new QDockWidget("Cell Cards", this);
	cellCardsWidget->setObjectName("Cell Cards");
	UiCellCards.setupUi(cellCardsWidget);
	cellCardsWidget->setWidget(UiCellCards.cellCardsFormLayOutWidget);
	this->addDockWidget(Qt::LeftDockWidgetArea, cellCardsWidget);
	connect(UiCellCards.renderSelected, SIGNAL(pressed()), this, SLOT(parseSelectedCells()));
	connect(UiCellCards.cellCardsTree, SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int)), this, SLOT(onCellItemDoubleClicked ( QTreeWidgetItem *, int)));

	// UNIVERSES
	//		=> contains a logical overview of the defined universes
	//----------------------------------------------------------------
	universesWidget = new QDockWidget("Universes", this);
	universesWidget->setObjectName("Universes");
	UiUniverses.setupUi(universesWidget);
	universesWidget->setWidget(UiUniverses.universesFormLayOutWidget);
	this->addDockWidget(Qt::LeftDockWidgetArea, universesWidget);
	connect(UiUniverses.renderSelected, SIGNAL(pressed()), this, SLOT(parseSelectedUniverseCells()));

	// MATERIALS CARDS
	//		=> contains the defined materials + the possibility to edit the color and transparancy of it
	//----------------------------------------------------------------
	materialCardsWidget = new QDockWidget("Material Cards", this);
	materialCardsWidget->setObjectName("Material Cards");
	UiMaterialCards.setupUi(materialCardsWidget);
	materialCardsWidget->setWidget(UiMaterialCards.materialCardsFormLayOutWidget);
	this->addDockWidget(Qt::BottomDockWidgetArea, materialCardsWidget);
	connect(UiMaterialCards.materialCardsTree, SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int)), this, SLOT(onItemDoubleClicked ( QTreeWidgetItem *, int)));
	
	// MCNPX Scene
	//		=> OpenGL rendering of the scene
	//		=> Rendering of the inside world (based on the imp=0 cell card)
	//		=> Renders also the planes for cross section and is usefull for camera placement
	//----------------------------------------------------------------
	mcnpxSceneWidget = new QDockWidget("MCNPX Scene", this);
	mcnpxSceneWidget->setObjectName("MCNPX Scene");
	mcnpxSceneWidget->setMinimumSize(300, 200);
	UiMCNPXScene.setupUi(mcnpxSceneWidget);
	mcnpxSceneWidget->setWidget(UiMCNPXScene.sceneDrawer);
	this->addDockWidget(Qt::BottomDockWidgetArea, mcnpxSceneWidget);
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(cameraPositionChanged(float,float,float)), CameraManager::getSingletonPtr()->getCamera(), SLOT(onCameraPositionChanged(float,float,float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(cameraLookAtChanged(float,float,float)), CameraManager::getSingletonPtr()->getCamera(), SLOT(onCameraLookAtChanged(float,float,float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(cameraStrafeChanged(float,float,float)), CameraManager::getSingletonPtr()->getCamera(), SLOT(onCameraStrafeChanged(float,float,float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(zPlaneMinChanged(float)), CameraManager::getSingletonPtr()->getSections(), SLOT(onZPlaneMinChanged(float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(zPlaneMaxChanged(float)), CameraManager::getSingletonPtr()->getSections(), SLOT(onZPlaneMaxChanged(float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(xPlaneMinChanged(float)), CameraManager::getSingletonPtr()->getSections(), SLOT(onXPlaneMinChanged(float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(xPlaneMaxChanged(float)), CameraManager::getSingletonPtr()->getSections(), SLOT(onXPlaneMaxChanged(float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(yPlaneMinChanged(float)), CameraManager::getSingletonPtr()->getSections(), SLOT(onYPlaneMinChanged(float)));
	connect(UiMCNPXScene.sceneDrawer, SIGNAL(yPlaneMaxChanged(float)), CameraManager::getSingletonPtr()->getSections(), SLOT(onYPlaneMaxChanged(float)));

	connect(this->UiMCNPXScene.sceneDrawer, SIGNAL(statusChanged(QString, int)), this, SLOT(statusBarChanged(QString, int)));
	
	connect(this->UiMCNPXScene.sceneDrawer, SIGNAL(informationChanged()), this, SLOT(onSceneInformationChanged()));
}


// ==> createActions()
// Setup the main actions for the menu and toolbars
//--------------------------------------------------------------------
void MCNPXVisualizer::createActions()
{
	openAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "Open.png"),tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "Save.png"), tr("&Save"), this);
	//saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	reloadAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "reload.png"),tr("&Reload..."), this);
	reloadAct->setShortcut(tr("Ctrl+R"));
	connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));

	deleteTempAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "cleanup.png"),tr("&Delete temp files"), this);
	connect(deleteTempAct, SIGNAL(triggered()), this, SLOT(deleteTempFiles()));

	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	printAct = new QAction(tr("&Print..."), this);
	printAct->setShortcut(tr("Ctrl+P"));
	printAct->setEnabled(false);
	connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

	zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

	normalSizeAct = new QAction(tr("&Normal Size"), this);
	// normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

	fitToWindowAct = new QAction(tr("&Fit to Window"), this);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
	connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	defaultViewAct = new QAction(tr("&Restore default view"), this);
	connect(defaultViewAct, SIGNAL(triggered()), this, SLOT(restoreDefaultView()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// Render actions
	renderAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "render.png"),tr("&Render..."), this);
	renderAct->setShortcut(tr("Ctrl+R"));
	renderAct->setStatusTip(tr("Render current MCNPX scene"));
	connect(renderAct, SIGNAL(triggered()), this, SLOT(render()));

	renderSaveAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "renderSave.png"),tr("&Save Rendered Scene..."), this);
	renderSaveAct->setShortcut(tr("Ctrl+S"));
	renderSaveAct->setStatusTip(tr("Save rendered MCNPX scene"));
	connect(renderSaveAct, SIGNAL(triggered()), this, SLOT(renderSave()));

	//renderOptionsAct = renderOptionsWidget->toggleViewAction();
	//renderOptionsAct->setIcon(QIcon("../images/renderSetup.png"));

	parserAct = new QAction(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "Parser.png"),tr("&Parse MCNPX file..."), this);
	parserAct->setShortcut(tr("Ctrl+P"));
	parserAct->setStatusTip(tr("Parse MCNPX file to a POV Ray scene"));
	connect(parserAct, SIGNAL(triggered()), this, SLOT(onParse()));

	// TODO: remove this action
	//loadg3Act = new QAction(tr("&Load g3"), this);
	//loadg3Act->setEnabled(true);
	//connect(loadg3Act, SIGNAL(triggered()), this, SLOT(loadg3()));
}


// ==> createMenus()
// Setup the main menus
//--------------------------------------------------------------------
void MCNPXVisualizer::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	fileMenu->addAction(reloadAct);
	fileMenu->addAction(deleteTempAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(fitToWindowAct);

	renderMenu = new QMenu(tr("&Render"), this);
	renderMenu->addAction(renderAct);
	renderMenu->addAction(renderSaveAct);

	parserMenu = new QMenu(tr("&Parser"), this);
	parserMenu->addAction(parserAct);

	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(defaultViewAct);
	helpMenu->addSeparator();
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);


	// add the menus to the global menubar
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(renderMenu);
	menuBar()->addMenu(parserMenu);
	menuBar()->addMenu(helpMenu);
}


// ==> createToolBars()
// Setup the toolbars
//--------------------------------------------------------------------
void MCNPXVisualizer::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->setObjectName("File");
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);
	fileToolBar->addAction(reloadAct);
	fileToolBar->addAction(deleteTempAct);
	fileToolBar->setGeometry(0, 0, 100, 20);
	//fileToolBar->addAction(loadg3Act);

	renderToolBar = addToolBar(tr("Render"));
	renderToolBar->setObjectName("Render");
	renderToolBar->addAction(renderAct);
	renderToolBar->addAction(renderSaveAct);
	//renderToolBar->addAction(renderOptionsAct);

	parserToolBar = addToolBar(tr("Parser"));
	parserToolBar->setObjectName("Parser");
	parserToolBar->addAction(parserAct);

	commandLineToolBar = addToolBar(tr("Command Line"));
	commandLineToolBar->setObjectName("Command Line");
	commandLine = new QLineEdit(this);
	commandLine->setMaximumWidth(300);
	commandLineLabel = new QLabel("input command: ", this);
	commandLineToolBar->addWidget(commandLineLabel);
	commandLineToolBar->addWidget(commandLine);

	connect(commandLine, SIGNAL(returnPressed()), this, SLOT(onCommand()));

}

// ==> createStatusBar()
// Setup the statusbar
//--------------------------------------------------------------------
void MCNPXVisualizer::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}




//####################################################################
//#  SLOTS: MENU/TOOLBAR => FILE
//####################################################################

// ==> open()
// Ask for a mcnpx file to open
//--------------------------------------------------------------------
 void MCNPXVisualizer::open()
{
	QString fileName = QFileDialog::getOpenFileName(this,
								 tr("Open File"), QString::fromStdString(Config::getSingleton().MCNPX), tr("MCNPX (*.txt *.pov *.*)"));
	if (fileName != "")
	{
		std::cout << "Opening " << fileName.toStdString().c_str() << "..." << std::endl;
		openFile(fileName); // actually open the mcnpx file
	}
		
}

// ==> openFile(fileName)
// Opens the mcnpx file, puts it into the mcnpx editor and starts the preparsing, if it is a pov ray file than just open it in the pov ray editor
//--------------------------------------------------------------------
void MCNPXVisualizer::openFile(QString fileName)
{
	if (!fileName.isEmpty()) {
		QFile file(fileName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) {
			QMessageBox::warning(this, tr("Application"),
										tr("Cannot read file %1:\n%2.")
										.arg(fileName)
										.arg(file.errorString()));
			return;
		}

		QTextStream in(&file);
		QApplication::setOverrideCursor(Qt::WaitCursor);

		// see if it is a povray or mcnpx file and put it in the right widget
		QStringList list = fileName.split(".");
		if (list.last() == "pov" || list.last() == "POV")
		{
			textEditPOV->setPlainText(in.readAll());
			textEditMCNPX->clear();
		}
		else
		{
			textEditMCNPX->setPlainText(in.readAll());
			textEditPOV->clear();
		}
		QApplication::restoreOverrideCursor();

		setCurrentFile(fileName);
		statusBar()->showMessage(tr("File loaded"), 2000);
	}

	preparse();
}

// ==> reload()
//--------------------------------------------------------------------
void MCNPXVisualizer::reload()
{
	openFile(curFile);
}

// ==> deleteTempFiles()
//    Remove all the temporary files created for the current file
//--------------------------------------------------------------------
void MCNPXVisualizer::deleteTempFiles()
{	
	QFile file;
	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cells");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cellTree");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_materials");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_surfaces");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_surfaces_opengl");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_universes");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	file.setFileName(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_importance");
	if (file.open(QFile::WriteOnly | QFile::Text))
		file.remove();

	this->reload();
}


// ==> save()
//--------------------------------------------------------------------
bool MCNPXVisualizer::save()
{
	if (curFile.isEmpty()) 
		return saveAs();
	else 
		return saveFile(curFile);
}

// ==> saveAs()
//--------------------------------------------------------------------
bool MCNPXVisualizer::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty())
		return false;
	return saveFile(fileName);
}

// ==> print()
//--------------------------------------------------------------------
void MCNPXVisualizer::print()
{
	Q_ASSERT(imageLabel->pixmap());
	QPrintDialog dialog(&printer, this);
	if (dialog.exec())
	{
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		QSize size = imageLabel->pixmap()->size();
		size.scale(rect.size(), Qt::KeepAspectRatio);
		painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
		painter.setWindow(imageLabel->pixmap()->rect());
		painter.drawPixmap(0, 0, *imageLabel->pixmap());
	}
}


// ==> saveFile(fileName)
// TODO: write changed mcnpx file
//--------------------------------------------------------------------
bool MCNPXVisualizer::saveFile(const QString &fileName)
{
	/*imageLabel->getIm
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) 
	{
		QMessageBox::warning(this, tr("MCNPX Visualiser"),
								  tr("Cannot write file %1:\n%2.")
								  .arg(fileName)
								  .arg(file.errorString()));
		return false;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << textEdit->toPlainText();
	QApplication::restoreOverrideCursor();

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;*/
	return true;
}

// ==> setCurrentFile(fileName)
// Split the filename into a path, name and extension
//--------------------------------------------------------------------
void MCNPXVisualizer::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	curFilePath = "";
	curFileExt = "";
	curFileName = "";

	QStringList list = curFile.split("/");
	QStringList list2 = list.last().split(".");
	if (list2.count() == 1)
		curFileName = list2.last();
	else
	{
		curFileExt = list2.last();
		list2.pop_back();
		curFileName = list2.join(".");
	}
	list.pop_back();
	curFilePath = list.join("/") + "/";
	
	setWindowTitle(tr("%1[*] - %2").arg(curFile).arg(tr("MCNPX Visualizer")));
}


void MCNPXVisualizer::statusBarChanged(QString status, int timeout)
{
	statusBar()->showMessage(status, timeout);
}


//####################################################################
//#  SLOTS: MENU/TOOLBAR => VIEW
//####################################################################

// ==> zoomIn()
//--------------------------------------------------------------------
void MCNPXVisualizer::zoomIn()
{
	scaleImage(1.25);
}

// ==> zoomOut()
//--------------------------------------------------------------------
void MCNPXVisualizer::zoomOut()
{
	scaleImage(0.8);
}

// ==> normalSizes()
//--------------------------------------------------------------------
void MCNPXVisualizer::normalSize()
{
	imageLabel->adjustSize();
	scaleFactor = 1.0;
}

// ==> normalSizes()
//	Stretch the image over the entire widget
//--------------------------------------------------------------------
void MCNPXVisualizer::fitToWindow()
{
	bool fitToWindow = fitToWindowAct->isChecked();
	scrollArea->setWidgetResizable(fitToWindow);
	if (!fitToWindow) 
	{
		normalSize();
	}
	updateActions();
}


// ==> updateActions()
// Update the menu actions based on the situation
//--------------------------------------------------------------------
void MCNPXVisualizer::updateActions()
{
	zoomInAct->setEnabled(!fitToWindowAct->isChecked());
	zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
	normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

// ==> scaleImage(factor)
// Scales the central rendering with a scaling factor
//--------------------------------------------------------------------
void MCNPXVisualizer::scaleImage(double factor)
{
	Q_ASSERT(imageLabel->pixmap());
	scaleFactor *= factor;
	imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

	adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(scrollArea->verticalScrollBar(), factor);

	zoomInAct->setEnabled(scaleFactor < 3.0);
	zoomOutAct->setEnabled(scaleFactor > 0.333);
}

// ==> adjustScrollBar(scrollBar, factor)
// Update the scrollbar based on the scale factor
//--------------------------------------------------------------------
void MCNPXVisualizer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}


//####################################################################
//#  SLOTS: COMMAND LINE
//####################################################################

// ==> onCommand()
// User entered a command
//--------------------------------------------------------------------
void MCNPXVisualizer::onCommand()
{

	QString command = commandLine->text();

	QRegExp reClear("CLEAR", Qt::CaseInsensitive);
	QRegExp reReset("RESET", Qt::CaseInsensitive);
	if (reClear.indexIn(command) != -1 || reReset.indexIn(command) != -1) 
	{
		_extentH = 100.0;
		_extentV = 100.0;
		_originX = 0.0;
		_originY = 0.0;
		_originZ = 0.0;

		UiMCNPXSceneEditor.sectionDistance->setValue(max(_extentH, _extentV));
		UiMCNPXSceneEditor.doubleSpinBox_originX->setValue(_originX);
		UiMCNPXSceneEditor.doubleSpinBox_originY->setValue(_originY);
		UiMCNPXSceneEditor.doubleSpinBox_originZ->setValue(_originZ);
		statusBar()->showMessage(QString("Set extent to eh=%1, ev=%2. Set origin to (%3,%4,%5).")
													.arg(_extentH, 4)
													.arg(_extentV, 4)
													.arg(_originX, 6, 'f', 2)
													.arg(_originY, 6, 'f', 2)
													.arg(_originZ, 6, 'f', 2)
													);
		commandLine->clear();
		return;
	}

	QRegExp reExtent("EXTENT\\s+([\\d.]+)\\s+([\\d,.]+)", Qt::CaseInsensitive);
	QRegExp reExtentH("EXTENT\\s+([\\d.]+)", Qt::CaseInsensitive);
	QRegExp reExtentNone("EXTENT", Qt::CaseInsensitive);

	bool extentChanged = false;
	if (reExtent.indexIn(command) != -1) 
	{
		_extentH = reExtent.cap(1).toFloat();
		_extentV = reExtent.cap(2).toFloat();
		extentChanged = true;
		
	}
	else if (reExtentH.indexIn(command) != -1) 
	{
		_extentH = reExtentH.cap(1).toFloat();
		_extentV = reExtentH.cap(1).toFloat();
		extentChanged = true;
	}
	else if (reExtentNone.indexIn(command) != -1) 
	{
		_extentH = 100.0;
		_extentV = 100.0;
		extentChanged = true;
	}
	if (extentChanged)
	{
		UiMCNPXSceneEditor.sectionDistance->setValue(max(_extentH, _extentV));
		statusBar()->showMessage(QString("Set extent to eh=%1, ev=%2.")
													.arg(_extentH, 4)
													.arg(_extentV, 4)
													);
	}


	QRegExp reOrigin1("ORIGIN\\s+([-\\d.]+)\\s+([-\\d.]+)\\s+([-\\d.]+)", Qt::CaseInsensitive);
	QRegExp reOrigin2("ORIGIN\\s+([-\\d.]+)\\s+([-\\d.]+)", Qt::CaseInsensitive);
	QRegExp reOrigin3("ORIGIN\\s+([-\\d.]+)", Qt::CaseInsensitive);
	QRegExp reOrigin4("ORIGIN", Qt::CaseInsensitive);

	bool originChanged = false;
	if (reOrigin1.indexIn(command) != -1) 
	{
		_originX = reOrigin1.cap(1).toFloat();
		_originY = reOrigin1.cap(2).toFloat();
		_originZ = reOrigin1.cap(3).toFloat();
		originChanged= true;
	}
	else if (reOrigin2.indexIn(command) != -1) 
	{
		_originX = reOrigin2.cap(1).toFloat();
		_originY = reOrigin2.cap(2).toFloat();
		_originZ = 0.0;
		originChanged= true;
	}
	else if (reOrigin3.indexIn(command) != -1) 
	{
		_originX = reOrigin3.cap(1).toFloat();
		_originY = 0.0;
		_originZ = 0.0;
		originChanged= true;
	}
	else if (reOrigin4.indexIn(command) != -1) 
	{
		_originX = 0.0;
		_originY = 0.0;
		_originZ = 0.0;
		originChanged= true;
	}
	if (originChanged)
	{
		UiMCNPXSceneEditor.doubleSpinBox_originX->setValue(_originX);
		UiMCNPXSceneEditor.doubleSpinBox_originY->setValue(_originY);
		UiMCNPXSceneEditor.doubleSpinBox_originZ->setValue(_originZ);
		statusBar()->showMessage(QString("Set origin to (%3,%4,%5).")
													.arg(_originX, 6, 'f', 2)
													.arg(_originY, 6, 'f', 2)
													.arg(_originZ, 6, 'f', 2)
													);
	}

	QRegExp rePX("PX\\s*([-\\d.]+)\\s*", Qt::CaseInsensitive);
	QRegExp rePY("PY\\s*([-\\d.]+)\\s*", Qt::CaseInsensitive);
	QRegExp rePZ("PZ\\s*([-\\d.]+)\\s*", Qt::CaseInsensitive);
	commandLine->clear();

	float extent = max(_extentH, _extentV);
	if (rePX.indexIn(command) != -1) 
	{
		float base = rePX.cap(1).toFloat();
		std::cout << "PX (" << QString::number(base).toStdString() << ")" << std::endl;
		float distance = extent / tan(22.5*PI/180.0);

		CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
		CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_X);
		CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originX;
		renderP(base+_originX, 0+_originY, 0+_originZ, base+distance+_originX, 0+_originY, 0+_originZ);
		CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
	}
	
	if (rePY.indexIn(command) != -1) 
	{
		float base = rePY.cap(1).toFloat();
		std::cout << "PY (" << QString::number(base).toStdString() << ")" << std::endl;
		float distance = extent / tan(22.5*PI/180.0);

		CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
		CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Y);
		CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originY;
		renderP(0+_originX, base+_originY, 0+_originZ, 0+_originX, base+distance+_originY, 0+_originZ);
		CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
	}

	if (rePZ.indexIn(command) != -1) 
	{
		float base = rePZ.cap(1).toFloat();
		std::cout << "PZ (" << QString::number(base).toStdString() << ")" << std::endl;
		float distance = extent / tan(22.5*PI/180.0);

		CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
		CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Z);
		CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originZ;
		renderP(0+_originX, 0+_originY, base+_originZ, 0+_originX, 0+_originY, base+distance+_originZ);
		CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
	}
	



}

// ==> onExtentChanged(val)
// Distance value for extent is changed
//--------------------------------------------------------------------
void MCNPXVisualizer::onExtentChanged(double val)
{
	_extentH = val;
	_extentV = val;
	statusBar()->showMessage(QString("Set extent to eh=%1, ev=%2.")
													.arg(_extentH, 4)
													.arg(_extentV, 4)
													);
}

// ==> onOriginXChanged(val)
// Origin change of 2D section
//--------------------------------------------------------------------
void MCNPXVisualizer::onOriginXChanged(double val)
{
	_originX = val;
	statusBar()->showMessage(QString("Set origin to (%3,%4,%5).")
													.arg(_originX, 6, 'f', 2)
													.arg(_originY, 6, 'f', 2)
													.arg(_originZ, 6, 'f', 2)
													);
}

// ==> onOriginYChanged(val)
// Origin change of 2D section
//--------------------------------------------------------------------
void MCNPXVisualizer::onOriginYChanged(double val)
{
	_originY = val;
	statusBar()->showMessage(QString("Set origin to (%3,%4,%5).")
													.arg(_originX, 6, 'f', 2)
													.arg(_originY, 6, 'f', 2)
													.arg(_originZ, 6, 'f', 2)
													);
}

// ==> onOriginZChanged(val)
// Origin change of 2D section
//--------------------------------------------------------------------
void MCNPXVisualizer::onOriginZChanged(double val)
{
	_originZ = val;
	statusBar()->showMessage(QString("Set origin to (%3,%4,%5).")
													.arg(_originX, 6, 'f', 2)
													.arg(_originY, 6, 'f', 2)
													.arg(_originZ, 6, 'f', 2)
													);
}



// ==> renderP(x, y, z, distX, distY, distZ)
// Start rendering a 2D section wit camera position at x, y, z and lookat distX, distY, distZ
// It overrules the basic camera parameters and is only used for 2D section shortcuts
//--------------------------------------------------------------------
void MCNPXVisualizer::renderP(float x, float y, float z, float distX, float distY, float distZ)
{
	QString cameraString = "camera\n";
	cameraString += "{   \n";           
	cameraString += "\tlook_at <" + QString::number(x) + "," + QString::number(y) + "," + QString::number(z) + ">\n";
	cameraString += "\tlocation <" + QString::number(distX) + "," + QString::number(distY) + "," + QString::number(distZ) + "> \n";   
	cameraString += "\tright  <-4/3,0,0>  \n";  //image_width/image_height,0,0>  \n";  
	cameraString += "\tangle 45.0\n";
	cameraString += "}\n";
	
	QString lightString =  "light_source\n";
	lightString += "{\n";
	lightString += "\t<" + QString::number(distX) + "," + QString::number(distY) + "," + QString::number(distZ) + "> \n";   
	lightString += "\tcolor White\n";
	lightString += "}\n";



	// give basic scene information and file info the the cameramanager (he creates the combine.pov that glues everything together)
	CameraManager::getSingletonPtr()->setClippedByImp0(true);
	CameraManager::getSingletonPtr()->setOrthographicProjection(true);
	CameraManager::getSingletonPtr()->setCameraString(cameraString);
	CameraManager::getSingletonPtr()->setLightString(lightString);
	CameraManager::getSingletonPtr()->setMaxTraceLevel(UiRenderOptions.maxTraceSpinbox->value());
	CameraManager::getSingletonPtr()->setInputFileName(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov");
	CameraManager::getSingletonPtr()->createPovRayFile(QString::fromStdString(Config::getSingleton().TEMP));

	CameraManager::getSingletonPtr()->setCameraString("");
	CameraManager::getSingletonPtr()->setLightString("");
	
	// give the quality information to rendermanager
	_renderManager->setParams(UiRenderOptions.widthSpinbox->value(), UiRenderOptions.heightSpinbox->value(), UiRenderOptions.qualityComboBox->currentIndex(), UiRenderOptions.antialiasCheckBox->isChecked(), UiRenderOptions.processes->value());

	// debug info for the output widget
	QString output = "<br />Start Rendering " + CameraManager::getSingletonPtr()->getInputFileName() + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Width               : " + QString::number(UiRenderOptions.widthSpinbox->value()) + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height              : " + QString::number(UiRenderOptions.heightSpinbox->value()) + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Quality             : " + QString::number(UiRenderOptions.qualityComboBox->currentIndex()) + "<br />";
	if (UiRenderOptions.antialiasCheckBox->isChecked())
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Antialias           : on <br />";
	else
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Antialias           : off <br />";
	output += "<br />";
	writeText(this->textEditPovRayOutput, output, "0000ff");

	statusBar()->showMessage(tr("Initialize Rendering... It can take 30 seconds before actual rendering starts..."), 0);

	// Start the rendering
	_renderManager->render();
}





//####################################################################
//#  SLOTS: CROSS SECTION
//####################################################################

// ==> onPX()
// Start renderin a PX 2D orthographic section
//--------------------------------------------------------------------
void MCNPXVisualizer::onPX()
{
	float base = UiMCNPXSceneEditor.sectionBase->value();
	std::cout << "PX (" << QString::number(base).toStdString() << ")" << std::endl;
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_X);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originX;
	renderP(base+_originX, 0+_originY, 0+_originZ, base+distance+_originX, 0+_originY, 0+_originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onPY()
// Start renderin a PY 2D orthographic section
//--------------------------------------------------------------------
void MCNPXVisualizer::onPY()
{
	float base = UiMCNPXSceneEditor.sectionBase->value();
	std::cout << "PY (" << QString::number(base).toStdString() << ")" << std::endl;
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Y);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originY;
	renderP(0+_originX, base+_originY, 0+_originZ, 0+_originX, base+distance+_originY, 0+_originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onPZ()
// Start renderin a PZ 2D orthographic section
//--------------------------------------------------------------------
void MCNPXVisualizer::onPZ()
{
	float base = UiMCNPXSceneEditor.sectionBase->value();
	std::cout << "PZ (" << QString::number(base).toStdString() << ")" << std::endl;
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Z);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originZ;
	renderP(0+_originX, 0+_originY, base+_originZ, 0+_originX, 0+_originY, base+distance+_originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}


// ==> onXPlaneMin()
// Start rendering a 2D orthographic section of the x min plane of the bounding box
// The plane is centered aroud the min and max of the bounding box coordinates
//--------------------------------------------------------------------
void MCNPXVisualizer::onXPlaneMin()
{
	float base = UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value();
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	// calculate the origin of the plane based on the min and max values for y and z bounding box
	float originX = 0.0;
	float originY = UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() ) / 2.0;
	float originZ = UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() ) / 2.0;

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_X_INVERSE);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originX;
	renderP(base+originX, 0+originY, 0+originZ, base-distance+originX, 0+originY, 0+originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onXPlaneMax()
// Start rendering a 2D orthographic section of the x max plane of the bounding box
// The plane is centered aroud the min and max of the bounding box coordinates
//--------------------------------------------------------------------
void MCNPXVisualizer::onXPlaneMax()
{
	float base = UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->value();
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	// calculate the origin of the plane based on the min and max values for y and z bounding box
	float originX = 0.0;
	float originY = UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() ) / 2.0;
	float originZ = UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() ) / 2.0;

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_X);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originX;
	renderP(base+originX, 0+originY, 0+originZ, base+distance+originX, 0+originY, 0+originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onYPlaneMin()
// Start rendering a 2D orthographic section of the y min plane of the bounding box
// The plane is centered aroud the min and max of the bounding box coordinates
//--------------------------------------------------------------------
void MCNPXVisualizer::onYPlaneMin()
{
	float base = UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value();
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	// calculate the origin of the plane based on the min and max values for x and z bounding box
	float originX = UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() ) / 2.0;
	float originY = 0.0;
	float originZ = UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() ) / 2.0;

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Y_INVERSE);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originY;
	renderP(0+originX, base+originY, 0+originZ, 0+originX, base-distance+originY, 0+originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onYPlaneMax()
// Start rendering a 2D orthographic section of the y max plane of the bounding box
// The plane is centered aroud the min and max of the bounding box coordinates
//--------------------------------------------------------------------
void MCNPXVisualizer::onYPlaneMax()
{
	float base = UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->value();
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	// calculate the origin of the plane based on the min and max values for x and z bounding box
	float originX = UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() ) / 2.0;
	float originY = 0.0;
	float originZ = UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value() ) / 2.0;

	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Y);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originY;
	renderP(0+originX, base+originY, 0+originZ, 0+originX, base+distance+originY, 0+originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onZPlaneMin()
// Start rendering a 2D orthographic section of the z min plane of the bounding box
// The plane is centered aroud the min and max of the bounding box coordinates
//--------------------------------------------------------------------
void MCNPXVisualizer::onZPlaneMin()
{
	float base = UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->value();
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	// calculate the origin of the plane based on the min and max values for x and y bounding box
	float originX = UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() ) / 2.0;
	float originY = UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() ) / 2.0;
	float originZ = 0.0;


	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Z_INVERSE);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originZ;
	renderP(0+originX, 0+originY, base+originZ, 0+originX, 0+originY, base-distance+originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

// ==> onZPlaneMax()
// Start rendering a 2D orthographic section of the z min plane of the bounding box
// The plane is centered aroud the min and max of the bounding box coordinates
//--------------------------------------------------------------------
void MCNPXVisualizer::onZPlaneMax()
{
	float base = UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->value();
	float distance = UiMCNPXSceneEditor.sectionDistance->value() / tan(22.5*PI/180.0);

	// calculate the origin of the plane based on the min and max values for x and y bounding box
	float originX = UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->value() ) / 2.0;
	float originY = UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() + (UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->value() - UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->value() ) / 2.0;
	float originZ = 0.0;


	CameraManager::getSingletonPtr()->getSections()->_useShortCut = true;
	CameraManager::getSingletonPtr()->getSections()->setShortCut(Sections3D::SHORTCUT_Z);
	CameraManager::getSingletonPtr()->getSections()->_shortCutBase = base+_originZ;
	renderP(0+originX, 0+originY, base+originZ, 0+originX, 0+originY, base+distance+originZ);
	CameraManager::getSingletonPtr()->getSections()->_useShortCut = false;
}

//####################################################################
//#  SLOTS: TAB => SCENE EDITOR
//####################################################################

// ==> onCheckBox_usePieceChanged(state)
// Switch between rectangular section or piecewice section (interprets the alignment of it and informs the cameramanager)
//--------------------------------------------------------------------
void MCNPXVisualizer::onCheckBox_usePieceChanged(int state)
{
	if (state)
	{
		if (UiMCNPXSceneEditor.radioButton_XY->isChecked())
			CameraManager::getSingletonPtr()->getSections()->setTypeSections(Sections3D::SECTIONS_XY);
		else if (UiMCNPXSceneEditor.radioButton_YZ->isChecked())
			CameraManager::getSingletonPtr()->getSections()->setTypeSections(Sections3D::SECTIONS_YZ);
		else
			CameraManager::getSingletonPtr()->getSections()->setTypeSections(Sections3D::SECTIONS_XZ);
	}
	else
	{
		CameraManager::getSingletonPtr()->getSections()->setTypeSections(Sections3D::SECTIONS_RECTANGULAR);
	}
	onSceneInformationChanged();
}

// ==> onSceneInformationChanged()
// Update the GUI of the Scene Editor based on the current state
//--------------------------------------------------------------------
void MCNPXVisualizer::onSceneInformationChanged()
{
	// update camera parameters in the GUI
	UiMCNPXSceneEditor.doubleSpinBox_azimuth->setValue(this->UiMCNPXScene.sceneDrawer->getAzimuth());
	UiMCNPXSceneEditor.doubleSpinBox_elevation->setValue(this->UiMCNPXScene.sceneDrawer->getElevation());
	UiMCNPXSceneEditor.doubleSpinBox_distance->setValue(this->UiMCNPXScene.sceneDrawer->getDistance());
	
	UiMCNPXSceneEditor.dial_azimuth->setValue(this->UiMCNPXScene.sceneDrawer->getAzimuth());
	UiMCNPXSceneEditor.dial_elevation->setValue(this->UiMCNPXScene.sceneDrawer->getElevation());
	UiMCNPXSceneEditor.horizontalSlider_distance->setValue(this->UiMCNPXScene.sceneDrawer->getDistance());
	
	UiMCNPXSceneEditor.doubleSpinBox_strafeX->setValue(this->UiMCNPXScene.sceneDrawer->getStrafeX());
	UiMCNPXSceneEditor.doubleSpinBox_strafeY->setValue(this->UiMCNPXScene.sceneDrawer->getStrafeY());
	UiMCNPXSceneEditor.doubleSpinBox_strafeZ->setValue(this->UiMCNPXScene.sceneDrawer->getStrafeZ());
	
	if (this->UiMCNPXScene.sceneDrawer->getLimitedPlanes() == true)
	{
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setMinimum(this->UiMCNPXScene.sceneDrawer->getXPlaneMin());
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setMinimum(this->UiMCNPXScene.sceneDrawer->getXPlaneMin());
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setMaximum(this->UiMCNPXScene.sceneDrawer->getXPlaneMax());
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setMaximum(this->UiMCNPXScene.sceneDrawer->getXPlaneMax());
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setTickInterval((this->UiMCNPXScene.sceneDrawer->getXPlaneMax()-this->UiMCNPXScene.sceneDrawer->getXPlaneMin())/10);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setTickInterval((this->UiMCNPXScene.sceneDrawer->getXPlaneMax()-this->UiMCNPXScene.sceneDrawer->getXPlaneMin())/10);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMinPos);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMaxPos);
		UiMCNPXSceneEditor.label_xPlaneMin_Min->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getXPlaneMin())));
		UiMCNPXSceneEditor.label_xPlaneMax_Min->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getXPlaneMin())));
		UiMCNPXSceneEditor.label_xPlaneMin_Max->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getXPlaneMax())));
		UiMCNPXSceneEditor.label_xPlaneMax_Max->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getXPlaneMax())));
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setMinimum(this->UiMCNPXScene.sceneDrawer->getXPlaneMin());
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setMinimum(this->UiMCNPXScene.sceneDrawer->getXPlaneMin());
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setMaximum(this->UiMCNPXScene.sceneDrawer->getXPlaneMax());
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setMaximum(this->UiMCNPXScene.sceneDrawer->getXPlaneMax());
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMinPos);
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMaxPos);
		
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setMinimum(this->UiMCNPXScene.sceneDrawer->getYPlaneMin());
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setMinimum(this->UiMCNPXScene.sceneDrawer->getYPlaneMin());
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setMaximum(this->UiMCNPXScene.sceneDrawer->getYPlaneMax());
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setMaximum(this->UiMCNPXScene.sceneDrawer->getYPlaneMax());
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setTickInterval((this->UiMCNPXScene.sceneDrawer->getYPlaneMax()-this->UiMCNPXScene.sceneDrawer->getYPlaneMin())/10);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setTickInterval((this->UiMCNPXScene.sceneDrawer->getYPlaneMax()-this->UiMCNPXScene.sceneDrawer->getYPlaneMin())/10);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMinPos);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMaxPos);
		UiMCNPXSceneEditor.label_yPlaneMin_Min->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getYPlaneMin())));
		UiMCNPXSceneEditor.label_yPlaneMax_Min->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getYPlaneMin())));
		UiMCNPXSceneEditor.label_yPlaneMin_Max->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getYPlaneMax())));
		UiMCNPXSceneEditor.label_yPlaneMax_Max->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getYPlaneMax())));
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setMinimum(this->UiMCNPXScene.sceneDrawer->getYPlaneMin());
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setMinimum(this->UiMCNPXScene.sceneDrawer->getYPlaneMin());
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setMaximum(this->UiMCNPXScene.sceneDrawer->getYPlaneMax());
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setMaximum(this->UiMCNPXScene.sceneDrawer->getYPlaneMax());
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMinPos);
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMaxPos);
		
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setMinimum(this->UiMCNPXScene.sceneDrawer->getZPlaneMin());
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setMinimum(this->UiMCNPXScene.sceneDrawer->getZPlaneMin());
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setMaximum(this->UiMCNPXScene.sceneDrawer->getZPlaneMax());
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setMaximum(this->UiMCNPXScene.sceneDrawer->getZPlaneMax());
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setTickInterval((this->UiMCNPXScene.sceneDrawer->getZPlaneMax()-this->UiMCNPXScene.sceneDrawer->getZPlaneMin())/10);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setTickInterval((this->UiMCNPXScene.sceneDrawer->getZPlaneMax()-this->UiMCNPXScene.sceneDrawer->getZPlaneMin())/10);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMinPos);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMaxPos);
		UiMCNPXSceneEditor.label_zPlaneMin_Min->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getZPlaneMin())));
		UiMCNPXSceneEditor.label_zPlaneMax_Min->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getZPlaneMin())));
		UiMCNPXSceneEditor.label_zPlaneMin_Max->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getZPlaneMax())));
		UiMCNPXSceneEditor.label_zPlaneMax_Max->setText(QString::number(int(this->UiMCNPXScene.sceneDrawer->getZPlaneMax())));
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setMinimum(this->UiMCNPXScene.sceneDrawer->getZPlaneMin());
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setMinimum(this->UiMCNPXScene.sceneDrawer->getZPlaneMin());
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setMaximum(this->UiMCNPXScene.sceneDrawer->getZPlaneMax());
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setMaximum(this->UiMCNPXScene.sceneDrawer->getZPlaneMax());
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMinPos);
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMaxPos);
	}	
	else
	{
		int min = -5000;
		int max = 5000;
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setMinimum(min);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setMinimum(min);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setMaximum(max);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setMaximum(max);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setTickInterval((max-min)/10);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setTickInterval((max-min)/10);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMinPos);
		UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMaxPos);
		UiMCNPXSceneEditor.label_xPlaneMin_Min->setText(QString::number(int(min)));
		UiMCNPXSceneEditor.label_xPlaneMax_Min->setText(QString::number(int(min)));
		UiMCNPXSceneEditor.label_xPlaneMin_Max->setText(QString::number(int(max)));
		UiMCNPXSceneEditor.label_xPlaneMax_Max->setText(QString::number(int(max)));
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setMinimum(min);
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setMinimum(min);
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setMaximum(max);
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setMaximum(max);
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMinPos);
		UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_xPlaneMaxPos);
		
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setMinimum(min);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setMinimum(min);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setMaximum(max);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setMaximum(max);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setTickInterval((max-min)/10);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setTickInterval((max-min)/10);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMinPos);
		UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMaxPos);
		UiMCNPXSceneEditor.label_yPlaneMin_Min->setText(QString::number(int(min)));
		UiMCNPXSceneEditor.label_yPlaneMax_Min->setText(QString::number(int(min)));
		UiMCNPXSceneEditor.label_yPlaneMin_Max->setText(QString::number(int(max)));
		UiMCNPXSceneEditor.label_yPlaneMax_Max->setText(QString::number(int(max)));
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setMinimum(min);
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setMinimum(min);
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setMaximum(max);
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setMaximum(max);
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMinPos);
		UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_yPlaneMaxPos);
		
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setMinimum(min);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setMinimum(min);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setMaximum(max);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setMaximum(max);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setTickInterval((max-min)/10);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setTickInterval((max-min)/10);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMinPos);
		UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMaxPos);
		UiMCNPXSceneEditor.label_zPlaneMin_Min->setText(QString::number(int(min)));
		UiMCNPXSceneEditor.label_zPlaneMax_Min->setText(QString::number(int(min)));
		UiMCNPXSceneEditor.label_zPlaneMin_Max->setText(QString::number(int(max)));
		UiMCNPXSceneEditor.label_zPlaneMax_Max->setText(QString::number(int(max)));
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setMinimum(min);
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setMinimum(min);
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setMaximum(max);
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setMaximum(max);
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMinPos);
		UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setValue(this->UiMCNPXScene.sceneDrawer->getSections()->_zPlaneMaxPos);
	}
	
	bool sectionsEnabled = this->UiMCNPXScene.sceneDrawer->getSectionsEnabled();
	bool rectSections = false;
	bool pieSections = false;
	if (sectionsEnabled && this->UiMCNPXScene.sceneDrawer->getSections()->_typeSections == Sections3D::SECTIONS_RECTANGULAR)
	 	rectSections = true;
	else if (sectionsEnabled)
	 	pieSections = true;
	 	
	 	

	UiMCNPXSceneEditor.horizontalSlider_xPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.horizontalSlider_xPlaneMax->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_xPlaneMin_Min->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_xPlaneMin_Max->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_xPlaneMax_Min->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_xPlaneMax_Max->setEnabled(rectSections);
	UiMCNPXSceneEditor.doubleSpinBox_xPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.doubleSpinBox_xPlaneMax->setEnabled(rectSections);
	UiMCNPXSceneEditor.pushButton_xPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.pushButton_xPlaneMax->setEnabled(rectSections);
	
	UiMCNPXSceneEditor.horizontalSlider_yPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.horizontalSlider_yPlaneMax->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_yPlaneMin_Min->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_yPlaneMin_Max->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_yPlaneMax_Min->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_yPlaneMax_Max->setEnabled(rectSections);
	UiMCNPXSceneEditor.doubleSpinBox_yPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.doubleSpinBox_yPlaneMax->setEnabled(rectSections);
	UiMCNPXSceneEditor.pushButton_yPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.pushButton_yPlaneMax->setEnabled(rectSections);
	
	UiMCNPXSceneEditor.horizontalSlider_zPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.horizontalSlider_zPlaneMax->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_zPlaneMin_Min->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_zPlaneMin_Max->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_zPlaneMax_Min->setEnabled(rectSections);
	UiMCNPXSceneEditor.label_zPlaneMax_Max->setEnabled(rectSections);
	UiMCNPXSceneEditor.doubleSpinBox_zPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.doubleSpinBox_zPlaneMax->setEnabled(rectSections);
	UiMCNPXSceneEditor.pushButton_zPlaneMin->setEnabled(rectSections);
	UiMCNPXSceneEditor.pushButton_zPlaneMax->setEnabled(rectSections);
	
	// pie sections elements
	UiMCNPXSceneEditor.doubleSpinBox_angleMax->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_AngleMax_Min->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_angleMax_Max->setEnabled(pieSections);
	UiMCNPXSceneEditor.horizontalSlider_angleMax->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_angleMax->setEnabled(pieSections);
	
	UiMCNPXSceneEditor.label_angleMin_Min->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_angleMin_Max->setEnabled(pieSections);
	UiMCNPXSceneEditor.horizontalSlider_angleMin->setEnabled(pieSections);
	UiMCNPXSceneEditor.doubleSpinBox_angleMin->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_angleMin->setEnabled(pieSections);
	
	UiMCNPXSceneEditor.label_radius->setEnabled(pieSections);
	UiMCNPXSceneEditor.doubleSpinBox_radius->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_radius_Min->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_radius_Max->setEnabled(pieSections);
	UiMCNPXSceneEditor.horizontalSlider_radius->setEnabled(pieSections);
	
	UiMCNPXSceneEditor.doubleSpinBox_height->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_height_Min->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_height_Max->setEnabled(pieSections);
	UiMCNPXSceneEditor.horizontalSlider_height->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_height->setEnabled(pieSections);
	
	UiMCNPXSceneEditor.label_strafeSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_strafeXSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_strafeYSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.label_strafeZSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.doubleSpinBox_strafeXSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.doubleSpinBox_strafeYSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.doubleSpinBox_strafeZSections->setEnabled(pieSections);
	UiMCNPXSceneEditor.radioButton_XY->setEnabled(pieSections);
	UiMCNPXSceneEditor.radioButton_YZ->setEnabled(pieSections);
	UiMCNPXSceneEditor.radioButton_XZ->setEnabled(pieSections);
}	





//####################################################################
//#  SLOTS: TAB => OUTPUT/POVRAY OUTPUT
//####################################################################

// ==> writeText(textEdit, text, color, isPlain)
// Write text to a textEdit widget
//		textEdit: widget to which the text must be written
//		text: the text that needs to be written
//		color: the color of the written text (of the form 000000 for black and FFFFFF for white)
//		isPlain=false: if it is written with rich html text in color or just plain black
//--------------------------------------------------------------------
void MCNPXVisualizer::writeText(QTextEdit* textEdit, QString text, QString color, bool isPlain)
{
	QTextCursor c = textEdit->textCursor();
	c.movePosition(QTextCursor::End);
	textEdit->setTextCursor(c);
	if (isPlain)
	{
		textEdit->insertHtml("<span style=\" color:#000000\"></span>");
		textEdit->insertPlainText(text);
	}
	else
		textEdit->insertHtml(QString("<span style=\" color:#" + color + "\">" + text + "</span>"));

	c = textEdit->textCursor();
	c.movePosition(QTextCursor::End);
	textEdit->setTextCursor(c);
}



//####################################################################
//#  SLOTS: MENU/TOOLBAR => RENDER
//####################################################################

// ==> render()
// Callback from the povray binder that there is output information
//		output: return text of the povray instance
//		param: parameters of the povray builded scene (it contains the begin and end row/column of the rendered scene), used for merging multiple povray instances together or as debug info
//		isError: if the output information is an error (so the GUI can inform the user there was an error occured
//--------------------------------------------------------------------
void MCNPXVisualizer::render()
{
	// give basic scene information and file info the the cameramanager (he creates the combine.pov that glues everything together)
	CameraManager::getSingletonPtr()->setClippedByImp0(true);
	CameraManager::getSingletonPtr()->setMaxTraceLevel(UiRenderOptions.maxTraceSpinbox->value());
	CameraManager::getSingletonPtr()->setInputFileName(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov");
	CameraManager::getSingletonPtr()->createPovRayFile(QString::fromStdString(Config::getSingleton().TEMP));
	
	// give the quality information to rendermanager
	_renderManager->setParams(UiRenderOptions.widthSpinbox->value(), UiRenderOptions.heightSpinbox->value(), UiRenderOptions.qualityComboBox->currentIndex(), UiRenderOptions.antialiasCheckBox->isChecked(), UiRenderOptions.processes->value());


	// debug info for the output widget
	QString output = "<br />Start Rendering " + CameraManager::getSingletonPtr()->getInputFileName() + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Width               : " + QString::number(UiRenderOptions.widthSpinbox->value()) + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height              : " + QString::number(UiRenderOptions.heightSpinbox->value()) + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Quality             : " + QString::number(UiRenderOptions.qualityComboBox->currentIndex()) + "<br />";
	if (UiRenderOptions.antialiasCheckBox->isChecked())
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Antialias           : on <br />";
	else
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Antialias           : off <br />";
	output += "<br />";
	writeText(this->textEditPovRayOutput, output, "0000ff");

	statusBar()->showMessage(tr("Initialize Rendering... It can take 30 seconds before actual rendering starts..."), 0);

	// Start the rendering
	_renderManager->render();
	UiRenderOptions.progressBar->setValue(_renderManager->getProgress());
	
}


// ==> onPovrayOutput(output, param, isError)
// Callback from the povray binder that there is output information
//		output: return text of the povray instance
//		param: parameters of the povray builded scene (it contains the begin and end row/column of the rendered scene), used for merging multiple povray instances together or as debug info
//		isError: if the output information is an error (so the GUI can inform the user there was an error occured
//--------------------------------------------------------------------
void MCNPXVisualizer::onPovrayOutput(QString output, PovRayRendererInformation param, bool isError)
{
	moveToThread(cthread); // necessary because we want to use the GUI out of a non gui thread
	
	UiRenderOptions.progressBar->setValue(_renderManager->getProgress());
	if (_renderManager->getProgress() != 0)
			statusBar()->showMessage(QString("Rendering Scene: %1% ").arg(_renderManager->getProgress(), 3)
												+ _renderManager->getProgressTime());
	else
	{
		statusBar()->showMessage(tr("Initialize Rendering... It can take 30 seconds before actual rendering starts... ")
											+ _renderManager->getParsingTime(), 0);	
	}
	if (isError)
	{	
		//QString paraminfo("<br /><br /> ERROR: " + param.outputFile + "<br />");
		//paraminfo += QString("(" + QString::number(param.startRow) + "," + QString::number(param.startColumn) + ") to (" + QString::number(param.endRow) + "," + QString::number(param.endColumn) + ") <br />");
		//writeText(this->textEditPovRayOutput, paraminfo, "ff0000");
		writeText(this->textEditPovRayOutput, output.replace("\n", "<br />"), "000000");
	}
	else
	{
		//QString paraminfo = "<br /><br /> " + param.outputFile + "<br />";
		//paraminfo += QString("(" + QString::number(param.startRow) + "," + QString::number(param.startColumn) + ") to (" + QString::number(param.endRow) + "," + QString::number(param.endColumn) + ") <br />");
		//writeText(this->textEditPovRayOutput, paraminfo, "0000ff");
		writeText(this->textEditPovRayOutput, output.replace("\n", "<br />"), "000000", true);
	}
}




// ==> finishedRendering(isSnapShot)
//	Called when the rendering of a process is finished (if there are more processes, this function is called multiple times)
//  It asks the output image from the rendermanager and prints it in the main rendering tab
//	It draws always the whole image (so if one process is still busy, his part is still black in the global output)
//		isSnapShot: boolean that is used for multiplexing to the right output widget
//--------------------------------------------------------------------
void MCNPXVisualizer::finishedRendering(bool isSnapShot)
{
	moveToThread(cthread); // necessary because we want to use the GUI out of a non gui thread
	
	UiRenderOptions.progressBar->setValue(_renderManager->getProgress());

	if (isSnapShot)
	{
		this->UiRenderOptions.snapPushButton->setIcon(QPixmap::fromImage(*this->_renderManager->getOutputImage()));
		statusBar()->showMessage(QString("Snapshot rendered for ") + QString::number(_renderManager->getProgress()) + QString("%."));
	}
	else
	{
		imageLabel->setPixmap(QPixmap::fromImage(*this->_renderManager->getOutputImage()));
		scaleFactor = 1.0;
		printAct->setEnabled(true);
		fitToWindowAct->setEnabled(true);
		updateActions();

		if (!fitToWindowAct->isChecked())
			imageLabel->adjustSize();


		// debug info for the output window
		QString output = "<br />Finisehd Rendering " + CameraManager::getSingletonPtr()->getInputFileName() + "<br />";
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Opening Image...<br />";
		writeText(this->textEditPovRayOutput, output, "00ff00");

		statusBar()->showMessage(QString("Rendering Scene: %1% ").arg(_renderManager->getProgress(), 3)
												+ _renderManager->getProgressTime());
	}    
}



// ==> renderSave()
//	Saves the rendered scene to an image file
//--------------------------------------------------------------------
bool MCNPXVisualizer::renderSave()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Rendered Image"),
														 "/home",
														 tr("Images (*.png *.xpm *.jpg)"));
	if (fileName.isEmpty())
		return false;

	imageLabel->pixmap()->save(fileName);
	return true;
}


//####################################################################
//#  SLOTS: MENU/TOOLBAR => PARSER
//####################################################################


// ==> preparse()
//	Preparse the current mcnpx file
//	Calls the python MCNPXPreParser.py and defines the appropriate intermediate information files
//		=> The MCNPXPreParser will save information about surfaces, cells, univeres, importance and materials
//--------------------------------------------------------------------
void MCNPXVisualizer::preparse()
{
	// Clear the current information out of the GUI
	this->UiSurfaceCards.clearSurfaces();
	this->UiCellCards.clearCells();
	this->UiUniverses.clearUniverses();
	this->UiMaterialCards.clearMaterials(); 
	this->_currentColorIndex = 0;
	this->UiMCNPXScene.sceneDrawer->clearScene();

	// Prepare the command line commando for the preparser
	QStringList args;
	args.push_back(curFile);
	std::cout << QString("\"" + curFile + "\"").toStdString().c_str() << std::endl; 
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_surfaces");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cells");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_universes");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_importance" );
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_materials" );
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cellTree" );

	// Debug info for the output window
	QString output = "<br /><br />Python MCNPXPreParser.py \"" + curFile + "\"";
	output += " \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_surfaces" + "\"";
	output += " \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cells" + "\"";
	output += " \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_universes" + "\"";
	output += " \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_importance" + "\"";
	output += " \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_materials" + "\"";
	output += " \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cellTree" + "\"";
	output += "<br/>";
	writeText(textEditOutput, output, "0000ff");

	// Start running the python method
	_pythonBinder->call("MCNPXPreParser", args);
}


// ==> preparse()
//	Preparse the current mcnpx file
//	Calls the python MCNPXPreParser.py and defines the appropriate intermediate information files
//		=> The MCNPXPreParser will save information about surfaces, cells, univeres, importance and materials
//--------------------------------------------------------------------
void MCNPXVisualizer::onParse()
{
	moveToThread(cthread); // necessary because we want to use the GUI out of a non gui thread

	std::cout << "start parsing " << curFile.toStdString().c_str() << "..." << std::endl;

	// First write the material information colormap to file
	writeColorMap();

	// Prepare the command line command for the parser
	QStringList args;
	args.push_back(curFile);
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap");

	// Debug info for the output window
	QString output = "<br /><br/>Python MCNPXtoPOV.py \"" + curFile + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov" + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap" + "\"<br/>";
	writeText(textEditOutput, output, "0000ff");

	// Start running the python method
	_pythonBinder->call("MCNPXtoPOV", args);
}



// ==> finishedParsing(method)
//	Called when the pythonbinder has finished some external python method
//		method: python method that is finished
//	=> multiplex the functionality based on the finishing method
//--------------------------------------------------------------------
void MCNPXVisualizer::finishedParsing(QString method)
{
	if (method == "MCNPXPreParser")
	{
		QString line;

		// MATERIALS
		//--------------------------------------------------------------------
		// Load the materials in the gui based on the materials parsed out of the mcnpx file
		this->createMaterials();
		

		// SURFACES
		//--------------------------------------------------------------------
		// Load the surfaces in the gui
		QFile file(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_surfaces");
		if (!file.open(QFile::ReadOnly | QFile::Text))
		{
			std::cout << "ERROR (finishedParsing) => couldn't open surfaces file" << std::endl;
			return ;
		}
		QTextStream in(&file);
		while (!((line = in.readLine()).isNull()))
		{

			QStringList list = line.split("&");
			if (list.size() == 0)
				break;
			else if (list.size() != 3)
				continue;
			else
			{
				QString number = list.at(0);
				QString mnemonic = list.at(1);
				QString data = list.at(2);
				this->UiSurfaceCards.addSurface(number, mnemonic, data);
			}
		} 
		file.close();


		// CELLS
		//--------------------------------------------------------------------
		// Load the cells into the gui
		QFile fileCell(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cells");
		if (!fileCell.open(QFile::ReadOnly | QFile::Text))
		{
			std::cout << "ERROR (finishedParsing) => couldn't open cells file" << std::endl;
			return ;
		}
		QTextStream inCell(&fileCell);
		while (!((line = inCell.readLine()).isNull()))
		{

			QStringList list = line.split("&");
			if (list.size() == 0)
				break;
			else if (list.size() != 5)
				continue;
			else
			{
				QString number = list.at(0);
				QString material = list.at(1);
				QString density = list.at(2);
				QString geometry = list.at(3);
				QString params = list.at(4);
				QColor color = Qt::white;
				if (material.toInt() >= 0)
				{
					color = this->UiMaterialCards.getMaterialColor(material.toInt());
					this->UiCellCards.addCell(number, material, color, density, geometry, params);
				}
			}
		} 
		fileCell.close();


		// UNIVERSES
		//--------------------------------------------------------------------
		// Load the universes into the gui
		QFile fileUniverse(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_universes");
		if (!fileUniverse.open(QFile::ReadOnly | QFile::Text))
		{
			std::cout << "ERROR (finishedParsing) => couldn't open universes file" << std::endl;
			return ;
		}
		QTextStream inUniverse(&fileUniverse);
		while (!((line = inUniverse.readLine()).isNull()))
		{
			QStringList list = line.split("&");
			if (list.size() == 0)
				break;
			else
			{
				QString universe = list.at(0);
				list.pop_front();
				//this->UiUniverses.addUniverse(universe, list);
			}
		} 
		fileUniverse.close();

		// CELL TREE
		//--------------------------------------------------------------------
		// Load the universes into the gui
		QFile fileCellTree(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_cellTree");
		if (!fileCellTree.open(QFile::ReadOnly | QFile::Text))
		{
			std::cout << "ERROR (finishedParsing) => couldn't open cell tree file" << std::endl;
			return ;
		}
		QTextStream inCellTree(&fileCellTree);
		QString data("");
		while (!((line = inCellTree.readLine()).isNull()))
		{
			data += line + "\n";
		} 
		this->UiUniverses.createTree(data, UiCellCards._cells, UiMaterialCards._materials);
		fileCellTree.close();


		// UNIVERSES
		//--------------------------------------------------------------------
		// Load importance and add bounding box in the OpenGL MCNPX Scene
		QFile fileImportance(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_importance");
		if (!fileImportance.open(QFile::ReadOnly | QFile::Text))
		{
			std::cout << "ERROR (finishedParsing) => couldn't open universes file" << std::endl;
			return ;
		}
		QTextStream inImportance(&fileImportance);
		while (!((line = inImportance.readLine()).isNull()))
		{
			QStringList list = line.split("&");
			if (list.size() == 0)
				break;
			else
			{
				QString typeScene =  list.at(0);
				list.pop_front();
				this->UiMCNPXScene.sceneDrawer->addScene(typeScene, list);
			}
		} 
		fileImportance.close();
	}

	// Only a subset of the cells has been parsed
	// Prepare the renderer for rendering the subset of the cells
	// Start rendering the subset
	else if (method == "MCNPXCellParser")
	{
		std::cout << "cells has been parsed" << std::endl;
		CameraManager::getSingletonPtr()->setClippedByImp0(false);
		CameraManager::getSingletonPtr()->setMaxTraceLevel(UiRenderOptions.maxTraceSpinbox->value());
		CameraManager::getSingletonPtr()->setInputFileName(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx_cells.pov");
		CameraManager::getSingletonPtr()->createPovRayFile(QString::fromStdString(Config::getSingleton().TEMP));

		_renderManager->setParams(UiRenderOptions.widthSpinbox->value(), UiRenderOptions.heightSpinbox->value(), UiRenderOptions.qualityComboBox->currentIndex(), UiRenderOptions.antialiasCheckBox->isChecked(), UiRenderOptions.processes->value());
		_renderManager->render();
	}
}



 // ==> onPythonOutput(output, method, isError)
//	Called when the pythonbinder has some standard output for a certain method
//		output: standard output information of python
//		method: method which has been finished
//		isError: bool if there was an error
//--------------------------------------------------------------------
void MCNPXVisualizer::onPythonOutput(QString output, QString method, bool isError)
{
	if (isError)
	{	
		writeText(textEditOutput, output, "ff0000");
		if (method == "MCNPXPreParser")
		{
			QMessageBox msgBox;
			msgBox.setText("There was a preparsing error in \"" + curFileName + "." + curFileExt + "\".");
			msgBox.exec();
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("There was a parsing error in \"" + curFileName + "." + curFileExt + "\".");
			msgBox.exec();
		}
		
	}
	else
	{
		writeText(textEditOutput, output, "000000", true);
		if (method != "MCNPXPreParser" && method != "MCNPXCellParser")
		{
			QMessageBox msgBox;
			msgBox.setText("Parsing of \"" + curFileName + "." + curFileExt + "\" completed.");
			msgBox.exec();
		}
	}
}

// ==> loadg3()
//	TODO: temporary function => needs to be removed
//--------------------------------------------------------------------
void MCNPXVisualizer::loadg3()
{
	this->openFile("D:/UHasselt/2e Master/Stage/SVN/python/MCNPXToPOV/src/mcnpx/g3.txt");
}

// ==> testPython()
//	TODO: temporary function => needs to be removed
//--------------------------------------------------------------------
void MCNPXVisualizer::testPython()
{
	QStringList args;
	args.push_back(QString::fromStdString(Config::getSingleton().MCNPX) + "g3.txt");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov");
	_pythonBinder->call("MCNPXtoPOV", args);	
}




//####################################################################
//#  SLOTS: MENU/TOOLBAR => HELP
//####################################################################

// ==> about()
//--------------------------------------------------------------------
void MCNPXVisualizer::about()
{
	QMessageBox::about(this, tr("MCNPX Visualizer"),
		 tr("<p>The <b>MCNPX Visualizer</b> is an application for parsing mcnpx files into povray files. "
		 "These povray files can be rendered and edited</p>"
		 "<p>(c) Nick Michiels for SCK-CEN Mol (2011)</p>"));
}

// ==> restoreDefaultView()
//	=> set all the dock widgets to the original positions
//--------------------------------------------------------------------
void MCNPXVisualizer::restoreDefaultView()
{
	QFile file(QString::fromStdString(Config::getSingleton().DATA) + "defaultState");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QByteArray line = file.readLine();
		QMainWindow::restoreState(line);
		file.close();
	}

	QFile fileStore(QString::fromStdString(Config::getSingleton().TEMP) + "state");
	if (fileStore.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QByteArray lineStore = QMainWindow::saveState();
		fileStore.write(lineStore.constData(), lineStore.size());
		fileStore.close();
	}
}

//####################################################################
//#  SLOTS: QDOCKWIDGETS => RENDEROPTIONS
//####################################################################

// ==> onSnapShot()
// Start rendering a snapshot
//		Snapshot is a lowres rendered example of the scene with the current settings
//--------------------------------------------------------------------
void MCNPXVisualizer::onSnapShot()
{
	// Setup the cameramanager
	CameraManager::getSingletonPtr()->setClippedByImp0(true);
	CameraManager::getSingletonPtr()->setMaxTraceLevel(UiRenderOptions.maxTraceSpinbox->value());
	CameraManager::getSingletonPtr()->setInputFileName(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx.pov");
	CameraManager::getSingletonPtr()->createPovRayFile(QString::fromStdString(Config::getSingleton().TEMP));

	// Setup the rendermanager with a low resolution
	_renderManager->setParams(75, 50, 10, true, UiRenderOptions.processes->value());

	// Debug info for the output window
	QString output = "<br />Start Rendering " + CameraManager::getSingletonPtr()->getInputFileName() + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Width               : " + QString::number(150) + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height              : " + QString::number(100) + "<br />";
	output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Quality             : " + QString::number(10) + "<br />";
	if (false)
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Antialias           : on <br />";
	else
		output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Antialias           : off <br />";
	output += "<br />";
	writeText(this->textEditPovRayOutput, output, "0000ff");

	statusBar()->showMessage(tr("Initialize Rendering Snapshot... It can take 30 seconds before actual rendering starts..."), 0);
	
	// Start rendering the snapshot
	_renderManager->render(true);
	UiRenderOptions.progressBar->setValue(_renderManager->getProgress());
}


//####################################################################
//#  SLOTS: QDOCKWIDGETS => SURFACES
//####################################################################


//####################################################################
//#  SLOTS: QDOCKWIDGETS => CELL CARDS
//####################################################################

// ==> parseSelectedCells()
// Start parsing the selected cell cards
//--------------------------------------------------------------------
void MCNPXVisualizer::parseSelectedCells()
{
	moveToThread(cthread); // necessary because we want to use the GUI out of a non gui thread

	// First write the material information colormap to file
	writeColorMap();
	
	// Write the subset of the cellcards that need to be parsed to file (which can be used by the python parser)
	QFile file(QString::fromStdString(Config::getSingleton().TEMP) + "cellsToParse.txt");
	if (file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream out(&file);

		QList<QTreeWidgetItem*> items = this->UiCellCards.cellCardsTree->selectedItems();
		for (int i=0; i<items.size(); i++)
		{
			out << items[i]->text(0).toStdString().c_str() << "\n";
		}
	}
	file.close();

	// Setup the command line command and arguments
	QStringList args;
	args.push_back(curFile);
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx_cells.pov");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + "cellsToParse.txt");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap");

	QString output = "<br /><br/>Python MCNPXCellParser.py \"" + curFile + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx_cells.pov" + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + "cellsToParse.txt" + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap" + "\"<br/>";
	writeText(textEditOutput, output, "0000ff");
	
	// Start the python method
	_pythonBinder->call("MCNPXCellParser", args);
}


// ==> onCellItemDoubleClicked(item, column)
//	Called when there is clicked on a cellcard
//--------------------------------------------------------------------
void  MCNPXVisualizer::onCellItemDoubleClicked ( QTreeWidgetItem * item, int column )
{
	// If clicked on the material column => ask the color the change
	if (column == 1)
	{
		QColor currentColor = this->UiMaterialCards.getMaterialColor(item->text(1).toInt());
		QColor color = QColorDialog::getColor(currentColor, this);
		if (color.isValid()) {
			QPixmap pix(20, 20);
			pix.fill(color);

			this->UiMaterialCards.getMaterial(item->text(1).toInt())->setColor(color);
			this->UiMaterialCards.updateMaterial(item->text(1).toInt());

			QList<QTreeWidgetItem*> list = this->UiCellCards.findAllCellsWithMaterial(item->text(1));
			for (int i=0; i<list.size(); i++)
			{
				list[i]->setIcon(1, pix);
			}
		}
	}
}


//####################################################################
//#  SLOTS: QDOCKWIDGETS => UNIVERSES
//####################################################################

// ==> parseSelectedUniverseCells()
// Start parsing the selected cell cards in the universes panel
//--------------------------------------------------------------------
void MCNPXVisualizer::parseSelectedUniverseCells()
{
	moveToThread(cthread); // necessary because we want to use the GUI out of a non gui thread

	// First write the material information colormap to file
	writeColorMap();
	
	// Write the subset of the cellcards that need to be parsed to file (which can be used by the python parser)
	QFile file(QString::fromStdString(Config::getSingleton().TEMP) + "cellsToParse.txt");
	if (file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream out(&file);

		QList<QTreeWidgetItem*> items = this->UiUniverses.universesTree->selectedItems();
		for (int i=0; i<items.size(); i++)
		{
			out << items[i]->text(0).toStdString().c_str() << "\n";
		}
	}
	file.close();

	// Setup the command line command and arguments
	QStringList args;
	args.push_back(curFile);
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx_cells.pov");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + "cellsToParse.txt");
	args.push_back(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap");

	QString output = "<br /><br/>Python MCNPXCellParser.py \"" + curFile + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx_cells.pov" + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + "cellsToParse.txt" + "\" \"" + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap" + "\"<br/>";
	writeText(textEditOutput, output, "0000ff");
	
	// Start the python method
	_pythonBinder->call("MCNPXCellParser", args);
}


//####################################################################
//#  SLOTS: QDOCKWIDGETS => MATERIAL CARDS
//####################################################################

// ==> onItemDoubleClicked(item, column)
//	Called when there is clicked on a materialcard
//--------------------------------------------------------------------
void  MCNPXVisualizer::onItemDoubleClicked( QTreeWidgetItem * item, int column )
{
	// If clicked on the color column => ask the color the change
	if (column == 2)
	{
		QColor currentColor = this->UiMaterialCards.getMaterialColor(item->text(0).toInt());
		std::cout << "Current color: " << currentColor.red() << "," << currentColor.green() << "," << currentColor.blue() << std::endl;
		QColor color = QColorDialog::getColor(currentColor, this);
		if (color.isValid()) {
			QPixmap pix(20, 20);
			pix.fill(color);
			//item->setIcon(2, pix);
			this->UiMaterialCards.getMaterial(item->text(0).toInt())->setColor(color);
			this->UiMaterialCards.updateMaterial(item->text(0).toInt());

			QList<QTreeWidgetItem*> list = this->UiCellCards.findAllCellsWithMaterial(item->text(0));
			for (int i=0; i<list.size(); i++)
			{
				list[i]->setIcon(1, pix);
			}
		}
	}
	// If clicked on the transparancy column => ask the transparancy the change
	else if (column == 3)
	{
		bool ok;
		int d = QInputDialog::getInteger(this, tr("Set transparancy percentage"),
				tr("Transparancy:"), item->text(3).toDouble(), 0, 100, 1, &ok);

		if (ok)
		{
			Material* mat = this->UiMaterialCards.getMaterial(item->text(0).toInt());
			if (mat)
			{
				mat->setAlpha(float(d/100.0));
				this->UiMaterialCards.updateMaterial(item->text(0).toInt());
			}
			;//this->UiMaterialCards.setMaterialTransparancy(item->text(0), d);
		}

	}
}

// ==> loadStandardColors()
//	Load the standard pseudo random colors out of DATA/randomColors.txt
//--------------------------------------------------------------------
void MCNPXVisualizer::loadStandardColors()
{
	_colorMap.clear();
	QFile file(QString::fromStdString(Config::getSingleton().DATA) + "randomColors.txt");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		std::cout << "ERROR (finishedParsing) => couldn't open universes file" << std::endl;
		return ;
	}

	QTextStream infile(&file);
	QString line;
	while (!((line = infile.readLine()).isNull()))
	{
		QStringList list = line.split(" ");
		if (list.size() == 0)
		break;
		else
		{
		QColor color(list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt());
		_colorMap.push_back(color);
		}
	} 
	file.close();
}

// ==> writeColorMap()
//	Write the materials color map to file so the parser can use the colors
//--------------------------------------------------------------------
void MCNPXVisualizer::writeColorMap()
{
	// Open the specific color map file for the current file out of the temp directory
	QFile fileSurfaces(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap");
	if (!fileSurfaces.open(QFile::WriteOnly | QFile::Text))
	{
		std::cout << "ERROR (finishedParsing) => couldn't open surfaces file" << std::endl;
		this->writeText(this->textEditOutput, QString("ERROR saving colormap: couldn't open ") + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap", "ff0000");
		return;
	}

	QTextStream outSurfaces(&fileSurfaces);
	outSurfaces << this->_currentColorIndex << "\n";

	// Write a map from material number to color to the file
	std::map<int, Material*>::iterator iter;
	for(iter = this->UiMaterialCards._materials.begin(); iter != UiMaterialCards._materials.end(); ++iter){
		outSurfaces << iter->first << ' ' << iter->second->getRed() << ' ' << iter->second->getGreen() << ' ' << iter->second->getBlue() << ' ' << iter->second->getAlpha() << '\n';
	}
	fileSurfaces.close();

	this->writeText(this->textEditOutput, QString("Colormap saved: ") + QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap", "00ff00");
}


// ==> loadStandardMaterials()
//	Load the standard name to color map out of the DATA/materials.txt file
//  The users specifies the mapping of the material name to a color
//--------------------------------------------------------------------
void MCNPXVisualizer::loadStandardMaterials()
{
	_standardMaterials.clear(); // remove any previous loaded material

	QString line;
	QFile fileMaterials(QString::fromStdString(Config::getSingleton().DATA) + "materials.txt");
	if (!fileMaterials.open(QFile::ReadOnly | QFile::Text))
	{
		std::cout << "ERROR (loadStandardMaterials) => couldn't open materials file" << std::endl;
		return ;
	}
	QTextStream inMaterials(&fileMaterials);

	int number;
	int r, g, b;
	float a;
	QString name;
	// The structure needs to be of the form: "name => red green blue alpha"
	QRegExp re("^[\\s\\t]*(\\S+)[\\s]*=>[\\s\\t]*(\\d+)[\\s\\t]+(\\d+)[\\s\\t]+(\\d+)[\\s\\t]+([\\d.]+)[\\s\\t]*[\\$]*[\\w\\W]*$", Qt::CaseInsensitive);
	
	while (!((line = inMaterials.readLine()).isNull()))
	{
		if (re.exactMatch(line))
		{
			if (re.indexIn(line) != -1) 
			{
				name = re.cap(1);
				r = re.cap(2).toInt();
				g = re.cap(3).toInt();
				b = re.cap(4).toInt();
				a = re.cap(5).toFloat();
			}
			Material* mat = new Material(-1, name);
			mat->setColor(QColor(r, g, b));
			mat->setAlpha(a);
			_standardMaterials[name] = mat;
		}
	}
	fileMaterials.close();
}

// ==> loadSavedMaterials()
//	Loads the previous saved materials color map for the current file out of the temp directory
//--------------------------------------------------------------------
void MCNPXVisualizer::loadSavedMaterials()
{
	_savedMaterials.clear(); // remove any earlier loaded color

	QString line;
	QFile fileMaterials(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_colorMap");
	if (!fileMaterials.open(QFile::ReadOnly | QFile::Text))
	{
		std::cout << "ERROR (loadSavedMaterials) => couldn't open materials file" << std::endl;
		return ;
	}
	QTextStream inSavedMaterialsFile(&fileMaterials);

	// first line is the current color index
	line = inSavedMaterialsFile.readLine();
	QTextStream str(&QString(line));
	str >> _currentColorIndex;

	if (_currentColorIndex >= _colorMap.size())
		_currentColorIndex = 0;
	int number, r, g, b;
	float a;
	while (!((line = inSavedMaterialsFile.readLine()).isNull()))
	{
		QTextStream str(&line);
		str >> number >> r >> g >>  b >> a;
		Material* mat = new Material(number, "");
		mat->setColor(QColor(r, g, b));
		mat->setAlpha(a);
		_savedMaterials[number] = mat;
	}	
	fileMaterials.close();
}

// ==> createMaterials()
//	Create the material colors based on the saved color map, the standard color map and the pseudo random color map
//--------------------------------------------------------------------
void MCNPXVisualizer::createMaterials()
{
	// First load in the predefined color map for materialnames
	// This map is used for seeking an default appropriate color
	// It only is used if the users hasn't specified the color manually previously
	// and if the name is correctly specified in the mcnpx file

	loadStandardMaterials();
	loadSavedMaterials();

	QString line;

	// material 0 is standard
	if (!this->UiMaterialCards.hasMaterial(0))
	{
		Material* emptyMaterial = new Material(0, "");
		emptyMaterial->setColor(QColor(255.0, 255.0, 255.0));
		emptyMaterial->setAlpha(0.0);
		this->UiMaterialCards.addMaterial(emptyMaterial);
	}

	// First create all the found materials in the mxnpx file
	QFile fileMaterials(QString::fromStdString(Config::getSingleton().TEMP) + curFileName + "_materials");
	if (!fileMaterials.open(QFile::ReadOnly | QFile::Text))
	{
		std::cout << "ERROR (finishedParsing) => couldn't open materials file" << std::endl;
		return ;
	}
	QTextStream inMaterials(&fileMaterials);

	int number;
	QString name;
	while (!((line = inMaterials.readLine()).isNull()))
	{
		QTextStream str(&QString(line));
		str >> number >> name;
		//std::cout << number << ": " << name.toStdString() << std::endl;
		if (name == "None")
			name == "";
		Material* mat = new Material(number, name);
		mat->setColor(QColor(255.0, 255.0, 255.0));
		mat->setAlpha(1.0);

		// see if there is a color saved by the user
		bool foundSavedMaterial = false;
		std::map<int, Material*>::iterator iter;
		for(iter = _savedMaterials.begin(); iter != _savedMaterials.end(); ++iter)
		{
			if ((*iter).first == number)
			{
				mat->setColor((*iter).second->getColor());
				mat->setAlpha((*iter).second->getAlpha());
				foundSavedMaterial = true;
			}
		}
		
		// if not found saved material, seek in the standard materials map
		bool foundStandardMaterial = false;
		if (!foundSavedMaterial)
		{
			std::map<QString, Material*>::iterator iter;
			for(iter = _standardMaterials.begin(); iter != _standardMaterials.end(); ++iter)
			{
				if ((*iter).first == name)
				{
					mat->setColor((*iter).second->getColor());
					mat->setAlpha((*iter).second->getAlpha());
					foundStandardMaterial = true;
				}
			}
		}

		// if no saved or standard material, use the pseuderandom color
		if (!foundSavedMaterial && !foundStandardMaterial)
		{
			mat->setColor(_colorMap[_currentColorIndex]);

			_currentColorIndex++;
			if (_currentColorIndex >= _colorMap.size())
				_currentColorIndex = 0;
		}

		this->UiMaterialCards.addMaterial(mat);
	}
}

//####################################################################
//#  SLOTS: QDOCKWIDGETS => MCNPX SCENE
//####################################################################






 

















