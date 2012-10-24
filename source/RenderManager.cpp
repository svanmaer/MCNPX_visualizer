//#########################################################################################################
//## RenderManager.cpp
//#########################################################################################################
//##
//## Handles to complete POV-Ray Rendering process
//## It creates a vector of POV-Ray renderers, based on how many processors to be used
//## It emmits signals when parts of the rendered image is finished
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################


#include "RenderManager.h"
#include "IniManager.h"

#include <QStringList>
#include <QList>
#include <QImage>
#include <QPainter>
#include <QRect>
#include <QMessageBox>
#include <iostream>
#include <math.h>

// ==> RenderManager(povFile, outputFIle, width, height, nprocesses)
//		povFile : input POV-Ray file to be renderen
//		outputFile: output image file
//		width: width of the output image file
//		height: height of the output image file
//		nprocesses: number of processors to be used (multiple POV-Ray instances)
//--------------------------------------------------------------------
RenderManager::RenderManager(QString povFile, QString outputFile, int width, int height, int nprocesses) : _povFile(povFile), _width(width), _height(height), _nProcess(nprocesses)
{
	_inputFileName = povFile;
	_outputFileName = outputFile;
	_outputFileType = "N";
	_quality = 1;
	_antialias = false;
	_isSnapShot = false;

	cthread = thread();
	
	// Give the information to the ini manager for POV-Ray
	IniManager::getSingletonPtr()->setInputFileName(_inputFileName);
	IniManager::getSingletonPtr()->setOutputFileType(_outputFileType);
	IniManager::getSingletonPtr()->setQuality(_quality);
	IniManager::getSingletonPtr()->setAnitalias(_antialias);
	IniManager::getSingletonPtr()->setWidth(_width);
	IniManager::getSingletonPtr()->setHeight(_height);
	
	_isFirstRenderedImage = false;
}


// ==> ~RenderManager()
// Destructor
//--------------------------------------------------------------------
RenderManager::~RenderManager()
{
	;
}


// ==> createRenderers()
//		create a vector of PovRayRenderers (based on the number of processors to be used)
//--------------------------------------------------------------------
void RenderManager::createRenderers()
{
	IniManager::getSingletonPtr()->setInputFileName(_inputFileName);
	IniManager::getSingletonPtr()->setOutputFileType(_outputFileType);
	IniManager::getSingletonPtr()->setQuality(_quality);
	IniManager::getSingletonPtr()->setAnitalias(_antialias);
	IniManager::getSingletonPtr()->setWidth(_width);
	IniManager::getSingletonPtr()->setHeight(_height);

	if (_renderers.size() > 0)
	{
		for (int i=0; i<_renderers.size(); i++)
		{
			delete _renderers[i];
		}
	}
	_renderers.clear();
	_renderersProgress.clear();
	int stroke = ceil(float(_width)/float(_nProcess));

	_progress = 0;
	int startRow = 1;
	int endRow = _height;

	for (int i=0; i<_nProcess; i++)
	{
		int startColumn = i*stroke+1;
		int endColumn = startColumn + stroke-1;
		//IniManager::getSingletonPtr()->setInputFileName("combined.pov");
		//IniManager::getSingletonPtr()->setOutputFileName("../temp/temp" + QString::number(i) + ".png");
		//IniManager::getSingletonPtr()->createIniFile("mcnpx" + QString::number(i) + ".ini", startColumn, endColumn, startRow, endRow);
		IniManager::getSingletonPtr()->setOutputFileName(QString::fromStdString(Config::getSingleton().TEMP) + "temp" + QString::number(i) + ".png");
		IniManager::getSingletonPtr()->createIniFile(QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx" + QString::number(i) + ".ini", startColumn, endColumn, startRow, endRow);
		PovRayRenderer* renderer = new PovRayRenderer(_inputFileName, QString::fromStdString(Config::getSingleton().TEMP) + "mcnpx" + QString::number(i) + ".ini");
		renderer->setInfo(PovRayRendererInformation(QString::fromStdString(Config::getSingleton().TEMP) + "temp" + QString::number(i) + ".png", i, startColumn, endColumn, startRow, endRow));
		connect(renderer, SIGNAL(finishedRendering(PovRayRendererInformation)), this, SLOT(finishedRendering(PovRayRendererInformation)));
		connect(renderer, SIGNAL(rendererCallOutput(QString, PovRayRendererInformation, bool)), this, SLOT(rendererCallOutput(QString, PovRayRendererInformation, bool)));
		this->_renderers.push_back(renderer);
		this->_renderersProgress.push_back(0);
	}
}


// ==> render(isSnapShot)
//		start rendering the different POV-Ray Renderers
//			isSnapShot: if true, it emmits a different signal
//--------------------------------------------------------------------
void RenderManager::render(bool isSnapShot)
{
	_progress = 0.0;
	_isFirstRenderedImage = true;
	_isSnapShot = isSnapShot;
	for (int i=0; i<this->_renderers.size(); i++)
	{
		_renderers[i]->render();
	}
}

// ==> finishedRendering(param)
//		Called when a POV-Ray Renderer finished (it uses the params to see which area is finished)
//		It emmits a signal that the rendering is partly completed
//--------------------------------------------------------------------
void RenderManager::finishedRendering(PovRayRendererInformation params)
{
	moveToThread(cthread);
	std::cout << "Finished rendering " << params.outputFile.toStdString().c_str() << std::endl;

	if (_isFirstRenderedImage)
	{
		this->outputImage = new QImage(params.outputFile);
		_isFirstRenderedImage = false;
	}
	else
	{
		QPainter painter(outputImage);

		QImage image(params.outputFile);
		if (image.isNull()) {
			return;
		}

		QRect target(params.startColumn-1, params.startRow-1, params.endColumn-params.startColumn+1, params.endRow-params.startRow+1);
		QRect source = target;
		painter.drawImage(target, image, source);
	}
	this->outputImage->save(_outputFileName);
	updateProgress();
	emit finishedRendering(_isSnapShot);
}

// ==> getProgressTime()
//		Returns the current progress of the rendering (in appropriate string format)
//--------------------------------------------------------------------
QString RenderManager::getProgressTime()
{
	int renderHour = 0;
	int renderMin = 0;
	int renderSec = 0;
	for (uint i=0; i<_nProcess; i++)
	{
		if (this->_renderers[i]->getInfo().renderHour > renderHour)
			renderHour = this->_renderers[i]->getInfo().renderHour;
		if (this->_renderers[i]->getInfo().renderMin > renderMin)
			renderMin = this->_renderers[i]->getInfo().renderMin;
		if (this->_renderers[i]->getInfo().renderSec > renderSec)
			renderSec = this->_renderers[i]->getInfo().renderSec;
	}				
	return QString("(%1:%2:%3)").arg(renderHour, 2).arg(renderMin, 2).arg(renderSec, 2);											
}

// ==> getParsingTime()
//		Returns the current progress of the parsing (of the POV-Ray scene) (in appropriate string format)
//--------------------------------------------------------------------
QString RenderManager::getParsingTime()
{
	int parseHour = 0;
	int parseMin = 0;
	int parseSec = 0;
	for (uint i=0; i<_nProcess; i++)
	{
		if (this->_renderers[i]->getInfo().parseHour > parseHour)
			parseHour = this->_renderers[i]->getInfo().parseHour;
		if (this->_renderers[i]->getInfo().parseMin > parseMin)
			parseMin = this->_renderers[i]->getInfo().parseMin;
		if (this->_renderers[i]->getInfo().parseSec > parseSec)
			parseSec = this->_renderers[i]->getInfo().parseSec;
	}				
	return QString("(%1:%2:%3)").arg(parseHour, 2).arg(parseMin, 2).arg(parseSec, 2);											
}

// ==> updateProgress()
//		Recalculate the progress of the rendering
//--------------------------------------------------------------------
void RenderManager::updateProgress()
{
	float tempProgress = 0;
	for (uint i=0; i<_nProcess; i++)
	{
		tempProgress += this->_renderers[i]->getInfo().progress/_nProcess;
	}
	_progress = int(tempProgress / float(_height) * 100.0);
	std::cout << "Total progress: " << QString::number(_progress).toStdString() << std::endl;
}

// ==> rendererCallOutput(output, param, isError)
//		Called when a POV-Ray Renderer emmited a signal that there is standard output for it
//--------------------------------------------------------------------
void RenderManager::rendererCallOutput(QString output, PovRayRendererInformation param, bool isError)
{
	moveToThread(cthread);
	updateProgress();
	emit onRendererCallOutput(output, param, isError);	
}
