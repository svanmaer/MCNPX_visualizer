//#########################################################################################################
//## Ui_CellCards.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the cell cards
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef UI_CELLCARDS_H
#define UI_CELLCARDS_H

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
#include <QPushButton>

#include "Cell.h"

class Ui_CellCards : public QObject
{
	Q_OBJECT
	public:
		// layout of the ui
		QVBoxLayout *cellCardsFormLayOut;
		QWidget *cellCardsFormLayOutWidget;

		// all cells are added to a tree
		QTreeWidget* cellCardsTree;
		int indexCounter;

		// action to render only the selected cells
		QPushButton *renderSelected;

		// container to store all the cell cards
		std::map<int, Cell*> _cells;

		// ==> addCell(number, material, materialColor, density, geometry, parameters)
		//  Add a cell to the widget (and store all the information)
		//--------------------------------------------------------------------
		void addCell(QString number, QString material, QColor materialColor, QString density, QString geometry, QString parameters)
		{
			// Create Cell and store in the container
			Cell* cellCard = new Cell(number.toInt(), material.toInt());
			cellCard->setDensity(density.toFloat());
			cellCard->setGeometry(geometry);
			cellCard->setParameters(parameters);
			_cells[cellCard->getNumber()] = cellCard;

			// Add the cell to the tree widget
			QStringList params;
			params.clear();
			params << number << material << density << geometry << parameters;
			QTreeWidgetItem* cell = new QTreeWidgetItem((QTreeWidget*)0, params);
			if (material.toInt() > 0)
			{
				QPixmap pix(20, 20);
				pix.fill(materialColor);
				cell->setIcon(1, QIcon(pix));
			}
			cellCardsTree->insertTopLevelItem(indexCounter, cell);
			indexCounter++;
		}

		// ==> findAllCellsWithMaterial(material)
		//  Returns a list of all the cells that contains a material
		//--------------------------------------------------------------------
		QList<QTreeWidgetItem*> findAllCellsWithMaterial(QString material)
		{
			return cellCardsTree->findItems(material, Qt::MatchCaseSensitive, 1);
		}

		// ==> findAllCells(list)
		//--------------------------------------------------------------------
		/*QList<QTreeWidgetItem*> findAllCells(QStringList list)
		{
			QList<QTreeWidgetItem*> output;
			for (int i=0; i<list.size(); i++)
			{
				QTreeWidgetItem* item = findCell(list[i]);
				output.append(item);
			}
			return output;
		}*/


		// ==> findCell(cellNumber)
		//   Returns the QTreeWidgetItem wich contains cell with number cellNumber
		//--------------------------------------------------------------------
		QTreeWidgetItem* findCell(QString cellNumber)
		{
			QList<QTreeWidgetItem*> list = cellCardsTree->findItems("300", Qt::MatchCaseSensitive, 0);
			if (list.size() == 0)
				return NULL;
			else
				return list.front();
		}


		// ==> clearCells()
		//   Remove all the cells out of the tree
		//--------------------------------------------------------------------
		void clearCells()
		{
			cellCardsTree->clear();
			indexCounter = 0;
		}

		// ==> setupUi(cellCards)
		//   Setup the UI of the widget (with parent cellCards)
		//   Create the basic tree and layout
		//--------------------------------------------------------------------
		void setupUi(QWidget *cellCards)
		{
			indexCounter = 0;

			// Create tree with header
			cellCardsTree = new QTreeWidget();
			cellCardsTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
			cellCardsTree->setColumnCount(4);
			cellCardsTree->setSortingEnabled(true);
			QStringList list;
			list << "Cell" << "Material" << "Density" << "Geometry" << "Parameters";// << "Children";
			cellCardsTree->setHeaderLabels(list);

			renderSelected = new QPushButton("Render Selected", cellCards);

			cellCardsFormLayOutWidget = new QWidget(cellCards);
			cellCardsFormLayOut = new QVBoxLayout;
			cellCardsFormLayOut->addWidget(cellCardsTree);
			cellCardsFormLayOut->addWidget(renderSelected);
			cellCardsFormLayOutWidget->setLayout(cellCardsFormLayOut);

			QMetaObject::connectSlotsByName(cellCards);
		} // setupUi
};

namespace Ui {
    class CellCards: public Ui_CellCards {};
} // namespace Ui


#endif //
