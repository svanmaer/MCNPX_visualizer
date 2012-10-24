//#######################################################################################################################
//## MCNPXVisualizer.h
//#######################################################################################################################
//##
//## Main GUI Class
//## Provides all the GUI functionality to read/parse/build MNCPX input files and to render 
//## it with povray.
//## The GUI is build out of QDockWidgets and a central graphics widget for the rendered scene
//## There are widgets defined for MCNPX information and render options:
//##		=> Materials:		material cards out of mcnpx file
//##		=> Cells:			cell cards out of mcnpx file
//##		=> Universes:		all defined universes (parsed out of mcnpx file)
//##		=> Surfaces:		surface cards out of mcnpx file
//##		=> Render options:	basic render options (resolution, quality, ...)
//##		=> MCNPX Scene:		OpenGL Rendered boundig box of the scene (usefull for defining cross sections)
//## In the central widget there are multiple tabs. The main tab is the rendered image
//## Other tabs:
//##		=> Scene Editor:	camera placement, sections, projection types, ...
//##		=> MCNPX Editor:	textual mcnpx file
//##		=> POV Ray Editor:	builded POV ray result of the parser
//##		=> Output:			console for the python output parser
//##		=> Povray Output:	console for the povray output
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
//
//#######################################################################################################################
//## USEFULL LINKS
//#######################################################################################################################
//##
//#######################################################################################################################

#ifndef MCNPXVisualizer_H
#define MCNPXVisualizer_H

#include <QMainWindow>
#include <QPrinter>
#include <QTextEdit>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

#include "RenderManager.h"
#include "PovRayRenderer.h"
#include "PythonBinder.h"
#include "qtabwidget.h"

#include <QDockWidget>

#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QByteArray>

#include "Ui_RenderOptions.h"
#include "Ui_SurfaceCards.h"
#include "Ui_Universes.h"
#include "Ui_CellCards.h"
#include "Ui_MaterialCards.h"
#include "Ui_MCNPXScene.h"
#include "Ui_MCNPXSceneEditor.h"

#include "CameraManager.h"
#include <map>

class MCNPXVisualizer : public QMainWindow
 {
    Q_OBJECT

	public:
		MCNPXVisualizer(QApplication* app);
		virtual ~MCNPXVisualizer()
		{
			QFile file(QString::fromStdString(Config::getSingleton().TEMP) + "state");
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QByteArray line = QMainWindow::saveState();
				file.write(line.constData(), line.size());
				file.close();
			}
		}

	private:
		// INITIALIZATION
		void initializeGUI();
		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWidgets();
		void createStatusBar();

		void updateActions();

		// BASIC FILE/GUI OPERATORS
		void scaleImage(double factor);
		bool saveFile(const QString &fileName);
		void adjustScrollBar(QScrollBar *scrollBar, double factor);
		void setCurrentFile(const QString &fileName);
		void openFile(QString fileName);
		void writeText(QTextEdit* textEdit, QString text, QString color, bool isPlain = false);

		

		// PARSER
		void preparse();
		void testPython();

		// MATERIALS
		void loadStandardColors();
		void createMaterials();
		void writeColorMap();
		void loadStandardMaterials();
		void loadSavedMaterials();

		// GUI
		QApplication *app;
		QThread* cthread;

		QTabWidget *tabWidget;
		QTextEdit *textEditPOV;
		QTextEdit *textEditMCNPX;
		QTextEdit *textEditOutput;
		QTextEdit *textEditPovRayOutput;

		QLabel *commandLineLabel;
		QLineEdit *commandLine;
		
		QLabel *imageLabel;
		QScrollArea *scrollArea;
		double scaleFactor;

		QString curFile;
		QString curFilePath;
		QString curFileExt;
		QString curFileName;

		QPrinter printer;

		// ACTIONS
		QAction *openAct;
		QAction *saveAct;
		QAction *saveAsAct;
		QAction *reloadAct;
		QAction *deleteTempAct;
		QAction *printAct;
		QAction *exitAct;
		QAction *zoomInAct;
		QAction *zoomOutAct;
		QAction *normalSizeAct;
		QAction *fitToWindowAct;
		QAction *aboutAct;
		QAction *defaultViewAct;
		QAction *aboutQtAct;
		//QAction *loadg3Act;
		QAction *renderAct;
		QAction *renderOptionsAct;
		QAction *renderSaveAct;
		QAction* parserAct;

		// MENU
		QMenu *fileMenu;
		QMenu *viewMenu;
		QMenu *renderMenu;
		QMenu *parserMenu;
		QMenu *helpMenu;

		// TOOLBAR
		QToolBar *fileToolBar;
		QToolBar *renderToolBar;
		QToolBar *parserToolBar;
		QToolBar *commandLineToolBar;

		// DOCK WIDGETS
		QDockWidget *renderOptionsWidget;
		Ui::RenderOptions UiRenderOptions;

		QDockWidget *surfaceCardsWidget;
		Ui::SurfaceCards UiSurfaceCards;

		QDockWidget *universesWidget;
		Ui::Universes UiUniverses;

		QDockWidget *cellCardsWidget;
		Ui::CellCards UiCellCards;

		QDockWidget *materialCardsWidget;
		Ui::MaterialCards UiMaterialCards;

		QDockWidget *mcnpxSceneWidget;
		Ui::MCNPXScene UiMCNPXScene;

		Ui::MCNPXSceneEditor UiMCNPXSceneEditor;

		// COMMAND LINE BINDERS
		RenderManager* _renderManager;
		PythonBinder* _pythonBinder;

		// MATERIALS
		int _currentColorIndex;
		std::vector<QColor> _colorMap;
		std::map<QString, Material*> _standardMaterials;
		std::map<int, Material*> _savedMaterials;

		// COMMAND LINE
		float _extentH;
		float _extentV;
		float _originX;
		float _originY;
		float _originZ;


	private slots:
		// FILE FUNCTIONS
		void open();
		bool save();
		bool saveAs();
		void reload();
		void print();
		void deleteTempFiles();

		// IMAGE FUNCTIONS
		void zoomIn();
		void zoomOut();
		void normalSize();
		void fitToWindow();

		// COMMAND LINE
		void onCommand();
		//void renderPX(float x);
		void renderP(float x, float y, float z, float distX, float distY, float distZ);

		// RENDERER
		void render();
		bool renderSave();
		void onSnapShot();
		void finishedRendering(bool isSnapShot);
		void onPovrayOutput(QString output, PovRayRendererInformation param, bool isError = true);

		// PARSER
		void onParse();
		void parseSelectedCells();
		void parseSelectedUniverseCells();
		void finishedParsing(QString method);
		void onPythonOutput(QString output, QString method, bool isError);


		void onCheckBox_usePieceChanged(int state);

		void onItemDoubleClicked ( QTreeWidgetItem * item, int column );
		void onCellItemDoubleClicked ( QTreeWidgetItem * item, int column );

		// OTHER
		void about();
		void restoreDefaultView();
		void loadg3();
		void statusBarChanged(QString status, int timeout);

		// COMMAND LINE
		void onExtentChanged(double val);
		void onOriginXChanged(double val);
		void onOriginYChanged(double val);
		void onOriginZChanged(double val);

		// CROSS SECTIONS
		void onPX();
		void onPY();
		void onPZ();
		void onXPlaneMin();
		void onXPlaneMax();
		void onYPlaneMin();
		void onYPlaneMax();
		void onZPlaneMin();
		void onZPlaneMax();

		void onSectionsEnabledChanged(int enabled)
		{
			if (this->UiMCNPXScene.sceneDrawer)
				this->UiMCNPXScene.sceneDrawer->setSectionsEnabled(enabled);
			CameraManager::getSingletonPtr()->setSectionsEnabled(enabled);
		}

		void onLimitedPlanesChanged(int enabled)
		{
			if (this->UiMCNPXScene.sceneDrawer)
			{
				this->UiMCNPXScene.sceneDrawer->setLimitedPlanes(enabled);
			}
		}
		
		void onSceneInformationChanged();
};

#endif // MCNPXVisualizer_H
