//#########################################################################################################
//## RenderManager.h
//#########################################################################################################
//##
//## Handles to complete POV-Ray Rendering process
//## It creates a vector of POV-Ray renderers, based on how many processors to be used
//## It emmits signals when parts of the rendered image is finished
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################


#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <QString>
#include <QImage>
#include <iostream>
#include <vector>

#include "PovRayRenderer.h"


class RenderManager : public QObject
{
	Q_OBJECT
	public:
		RenderManager(QString povFile="../temp/mcnpx.pov", QString outputFile="../temp/output.png", int width=400, int height=300, int nprocesses=1);
		~RenderManager();

	
		void render(bool isSnapShot = false);

		void setParams(int width, int height, int quality, bool antialias, int nProcesses)
		{
			_width = width;
			_height = height;
			_quality = quality;
			_antialias = antialias;
			_nProcess = nProcesses;
			createRenderers();
		}

		QImage* getOutputImage(){return outputImage;}	// returns the image of the (partly) rendered scene

		int getProgress(){ return _progress; }			// returns the progress of the rendering (only for Linux)
		void updateProgress();
		QString getProgressTime();
		QString getParsingTime();

	private:
		void createRenderers();		// creates the POV-Ray renderers to be used
	
		QThread* cthread;			// stores the current thread of the manager

		QString _povFile;			// input POV-Ray file
		int _nProcess;				// number of processors to be used

		QString _inputFileName;		// input POV-Ray file
		QString _outputFileName;	// output image file
		QString _outputFileType;	// output image type
		int _quality;				// quality of the rendering [0:11]
		bool _antialias;			// turn on/off antialiasing for the rendered image
		int _width;					// width of the rendered image
		int _height;				// height of the rendered image

		bool _isFirstRenderedImage;

		QImage* outputImage;		// output image

		std::vector<PovRayRenderer*> _renderers;	// list of all the POV-Ray Renderers needed to render the image
		std::vector<int> _renderersProgress;		// list of the progress of all the POV-Ray Renderers
		int _progress;				// Total progress of the rendering

		bool _isSnapShot;			// if the rendering is a snapshot of a normal rendering

	private slots:
		void finishedRendering(PovRayRendererInformation params);
		void rendererCallOutput(QString output, PovRayRendererInformation param, bool isError = true);

	signals:
		// emitted when the rendering or a part of the rendering is finished
		void finishedRendering(bool isSnapShot = false);
		// emitted when there is standard output for the rendering
		void onRendererCallOutput(QString output, PovRayRendererInformation param, bool isError = true);
};

#endif
