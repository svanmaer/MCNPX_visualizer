//#########################################################################################################
//## Ui_RenderOptions.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the render options
//##	=> The user can edit quality parameters like resolution, antialiasing, trace depth, quality level
//##	=> The user can create a rendered snapshot of the scene
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef UI_RENDEROPTIONS_H
#define UI_RENDEROPTIONS_H

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
#include <QProgressBar>

#include "Config.h"

class Ui_RenderOptions : public QObject
{
	Q_OBJECT
	public:
		
		// basic layout
		QFormLayout *rendererFormLayOut;
		QWidget *rendererFormLayOutWidget;

		// All Qt objects for the rendering options
		QComboBox *qualityComboBox;
		QSpinBox *widthSpinbox;
		QSpinBox *heightSpinbox;
		QCheckBox *antialiasCheckBox;
		QSpinBox *maxTraceSpinbox;
		QToolButton *snapPushButton;
		QLabel* snapshotArea;
		QScrollArea* scrollArea;
		QProgressBar* progressBar;
		QSpinBox *processes;
		
		// ==> setupUi(renderOptions)
		//   Setup the UI of the widget (with parent renderOptions)
		//--------------------------------------------------------------------
		void setupUi(QWidget *renderOptions)
		{
			qualityComboBox = new QComboBox(renderOptions);
			qualityComboBox->setObjectName(QString::fromUtf8("qualityComboBox"));
			qualityComboBox->setGeometry(QRect(0, 0, 60, 20));
			qualityComboBox->clear();
			qualityComboBox->insertItems(0, QStringList()
				<< QApplication::translate("Form", "0", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "1", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "2", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "3", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "4", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "5", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "6", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "7", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "8", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "9", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "10", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "11", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "12", 0, QApplication::UnicodeUTF8)
			 );
			qualityComboBox->setCurrentIndex(1);

			widthSpinbox = new QSpinBox(renderOptions);
			widthSpinbox->setObjectName(QString::fromUtf8("widthSpinBox"));
			widthSpinbox->setGeometry(QRect(0, 0, 60, 20));
			widthSpinbox->setMaximum(1920);
			widthSpinbox->setMinimum(1);
			widthSpinbox->setValue(400);
			heightSpinbox = new QSpinBox(renderOptions);
			heightSpinbox->setObjectName(QString::fromUtf8("heightSpinBox"));
			heightSpinbox->setGeometry(QRect(0, 0, 60, 20));
			heightSpinbox->setMaximum(1080);
			heightSpinbox->setMinimum(1);
			heightSpinbox->setValue(300);
			antialiasCheckBox = new QCheckBox(renderOptions);
			antialiasCheckBox->setObjectName(QString::fromUtf8("antialiasCheckBox"));
			antialiasCheckBox->setGeometry(QRect(0, 0, 60, 20));
			antialiasCheckBox->setChecked(false);

			processes = new QSpinBox(renderOptions);
			processes->setMaximum(50);
			processes->setMinimum(1);
			processes->setGeometry(QRect(0, 0, 60, 20));
			processes->setValue(2);

			snapPushButton = new QToolButton(renderOptions);
			snapPushButton->setIcon(QIcon(QString::fromStdString(Config::getSingleton().IMAGES) + "fingrsnap.png"));//, QString("Snap") ,renderOptions);
			
			snapPushButton->setFixedSize(77, 52);
			snapPushButton->setIcon(QPixmap(QString::fromStdString(Config::getSingleton().IMAGES) + "checkerboard.png"));
			snapPushButton->setIconSize(QSize(75, 50));

			maxTraceSpinbox = new QSpinBox(renderOptions);
			maxTraceSpinbox->setObjectName(QString::fromUtf8("heightSpinBox"));
			maxTraceSpinbox->setGeometry(QRect(0, 0, 60, 20));
			maxTraceSpinbox->setMaximum(100);
			maxTraceSpinbox->setMinimum(1);
			maxTraceSpinbox->setValue(5);
			maxTraceSpinbox->setSingleStep(1);

			progressBar = new QProgressBar(renderOptions);
			progressBar->setMaximum(100);
			progressBar->setMinimum(0);
			progressBar->setValue(0);
			progressBar->setTextVisible(true);


			// Create the layout
			rendererFormLayOutWidget = new QWidget(renderOptions);
			rendererFormLayOut = new QFormLayout;

			rendererFormLayOut->addRow(progressBar);
			rendererFormLayOut->addRow(tr("&Width:"), widthSpinbox);
			rendererFormLayOut->addRow(tr("&Height:"), heightSpinbox);
			rendererFormLayOut->addRow(tr("&Quality:"), qualityComboBox);
			rendererFormLayOut->addRow(tr("&Antialias:"), antialiasCheckBox);
			rendererFormLayOut->addRow(tr("&Processors:"), processes);
			rendererFormLayOut->addRow(tr("&Max Trace Depth:"), maxTraceSpinbox);
			rendererFormLayOut->addRow(tr("&Snap:"), snapPushButton);

			rendererFormLayOut->setRowWrapPolicy(QFormLayout::DontWrapRows);
			rendererFormLayOut->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
			rendererFormLayOut->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
			rendererFormLayOut->setLabelAlignment(Qt::AlignLeft);
			rendererFormLayOutWidget->setLayout(rendererFormLayOut);

			QMetaObject::connectSlotsByName(renderOptions);
		} // setupUi	
};

namespace Ui {
    class RenderOptions: public Ui_RenderOptions {};
} // namespace Ui


#endif // UI_TEST_H
