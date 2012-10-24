//#########################################################################################################
//## Ui_Universes.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the universes
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################


#ifndef UI_UNIVERSES_H
#define UI_UNIVERSES_H

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

#include <iostream>

#include "Cell.h"
#include "Material.h"

class Ui_Universes : public QObject
{
	Q_OBJECT
	public:
		
		// Basic layout
		QVBoxLayout *universesFormLayOut;
		QWidget *universesFormLayOutWidget;

		// Universes are stored in a tree
		QTreeWidget* universesTree;
		int indexCounter;

		// User can render selected universes/cells
		QPushButton *renderSelected;

		// ==> createTree(data, cells, materials)
		//   Create the whole universes tree based on the data (the informations of the cells and materials are also given for extra info)
		//--------------------------------------------------------------------
		void createTree(QString data, std::map<int, Cell*> cells, std::map<int, Material*> materials)
		{
			// regular expression for parsing the depth, cellnumber and universe number
			QRegExp re("^(\\d+),(\\d+),(\\d+)\\w*$", Qt::CaseInsensitive);
			QRegExp emptyLine("^\\s*$", Qt::CaseInsensitive);

			QStringList list = data.split("\n");
			int depth;
			int cell;
			int universe;
			std::vector<QTreeWidgetItem*> treeStack;
			std::vector<QTreeWidgetItem*> topLevel;
			for (int i=0; i<20; i++)
				treeStack.push_back(NULL);
			for (int i=0; i <list.size(); i++)
			{
				if (emptyLine.exactMatch(list[i]))
					continue;
				if (re.exactMatch(list[i]))
				{
					if (re.indexIn(list[i]) != -1) 
					{
						int depth = re.cap(1).toInt();
						int cell = re.cap(2).toInt();
						int universe = re.cap(3).toInt();
						if (depth == 0)
						{
							QStringList params;
							params.clear();
							params << QString::number(cell) << "U=" + QString::number(universe) << QString::number(cells[cell]->getMaterial());
							QTreeWidgetItem* item = new QTreeWidgetItem(universesTree, params);
							QPixmap pix(20, 20);
							pix.fill(materials[cells[cell]->getMaterial()]->getColor());
							item->setIcon(0, QIcon(pix));
							treeStack[depth] = item;
							topLevel.push_back(item);
						}
						else
						{
							QStringList params;
							params.clear();
							params << QString::number(cell) << "U=" + QString::number(universe) << QString::number(cells[cell]->getMaterial());
							QTreeWidgetItem* item = new QTreeWidgetItem(treeStack[depth-1], params);
							QPixmap pix(20, 20);
							pix.fill(materials[cells[cell]->getMaterial()]->getColor());
							item->setIcon(0, QIcon(pix));
							treeStack[depth] = item;
						}
						//std::cout << depth << ", " << cell << ", " << universe  << std::endl;
					}
					else
						std::cout << "ERROR (Ui_Universes::createTree): format of the input cell tree file is wrong." << std::endl;
				}
				else
						std::cout << "ERROR (Ui_Universes::createTree): format of the input cell tree file is wrong." << std::endl;
			}
		}

		// ==> addUniverse(number, cells)
		//   Add a universe to the tree (and add all cells that are part of the universe
		//--------------------------------------------------------------------
		void addUniverse(QString number, QStringList cells)
		{
			QStringList params;
			params.clear();
			params << QString("Universe " + number);
			QTreeWidgetItem* universe = new QTreeWidgetItem((QTreeWidget*)0, params);
			for (int i=0; i<cells.size(); i++)
			{
				params.clear();
				params << QString("Cell " + cells[i]);
				QTreeWidgetItem* universeCell = new QTreeWidgetItem(universe, params);
				//universeCell->setIcon(0, QIcon(red));
			}

			universesTree->insertTopLevelItem(indexCounter, universe);
			indexCounter++;
		}

		// ==> clearUniverses()
		//   Delete all the universes out of the tree
		//--------------------------------------------------------------------
		void clearUniverses()
		{
			universesTree->clear();
			indexCounter = 0;
		}

		// ==> setupUi(universes)
		//   Setup the UI of the widget (with parent universes)
		//--------------------------------------------------------------------
		void setupUi(QWidget *universes)
		{
			indexCounter = 0;

			// Create tree for the universes and add header
			universesTree = new QTreeWidget();
			universesTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
			universesTree->setColumnCount(1);
			QStringList list;
			list << "Cell" << "Universe" << "Material";// << "Children";
			universesTree->setHeaderLabels(list);

			renderSelected = new QPushButton("Render Selected", universes);
			
			// Create layout
			universesFormLayOutWidget = new QWidget(universes);
			universesFormLayOut = new QVBoxLayout;
			universesFormLayOut->addWidget(universesTree);
			universesFormLayOut->addWidget(renderSelected);
			universesFormLayOutWidget->setLayout(universesFormLayOut);

			QMetaObject::connectSlotsByName(universes);
		} // setupUi
};

namespace Ui {
    class Universes: public Ui_Universes {};
} // namespace Ui


#endif //
