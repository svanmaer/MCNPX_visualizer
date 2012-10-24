//#########################################################################################################
//## Ui_SurfaceCars.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the surface cards
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef UI_PARSEROPTIONS_H
#define UI_PARSEROPTIONS_H

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
#include <QTableWidget>
#include <QTreeWidget>

class Ui_SurfaceCards : public QObject
{
	Q_OBJECT
	public:
		
		// basic layout
		QVBoxLayout *parserFormLayOut;
		QWidget *parserFormLayOutWidget;

		// Surfaces are stored in a tree
		QTreeWidget *surfacesTree;
		int indexCounter;

		// ==> addSurface(number, mnemonic, entries)
		//  Add a surface to the widget (with all its information)
		//--------------------------------------------------------------------
		void addSurface(QString number, QString mnemonic, QString entries)
		{
			QStringList params;
			params.clear();
			params << number << mnemonic << entries;
			QTreeWidgetItem* surface = new QTreeWidgetItem((QTreeWidget*)0, params);
			surfacesTree->insertTopLevelItem(indexCounter, surface);
			indexCounter++;
		}

		// ==> clearSurfaces()
		//  Remove all the surfaces from the widget
		//--------------------------------------------------------------------
		void clearSurfaces()
		{
			surfacesTree->clear();
			indexCounter = 0;
		}

		// ==> setupUi(parserOptions)
		//   Setup the UI of the widget (with parent parserOptions)
		//   Create the basic tree and layout
		//--------------------------------------------------------------------
		void setupUi(QWidget *parserOptions)
		{
			// Create the tree and add a header
			indexCounter = 0;
			surfacesTree = new QTreeWidget();
			surfacesTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
			surfacesTree->setSortingEnabled(true);
			surfacesTree->setColumnCount(3);
			QStringList list;
			list << "Card Nr." << "Mnemonic" << "Entries";
			surfacesTree->setHeaderLabels(list);

			// Create layout
			parserFormLayOutWidget = new QWidget(parserOptions);
			parserFormLayOut = new QVBoxLayout;
			parserFormLayOut->addWidget(surfacesTree);
			parserFormLayOutWidget->setLayout(parserFormLayOut);

			QMetaObject::connectSlotsByName(parserOptions);
		} // setupUi
};

namespace Ui {
    class SurfaceCards: public Ui_SurfaceCards {};
} // namespace Ui

#endif //
