//#########################################################################################################
//## Ui_MaterialCards.h
//#########################################################################################################
//##
//## Graphical User Interface to visualize the material cards
//## The user can edit color and transparancy level of the materials to render
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef UI_MATERIALCARDS_H
#define UI_MATERIALCARDS_H

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

#include <iostream>

#include "Material.h"


class Ui_MaterialCards : public QObject
{
	Q_OBJECT
	public:
		
		// basic layout of the widget
		QVBoxLayout *materialCardsFormLayOut;
		QWidget *materialCardsFormLayOutWidget;

		// materials are visualized in a tree
		QTreeWidget* materialCardsTree;
		int indexCounter;

		// Container for material colors and transparancy
		//std::map<QString, QColor> _materialColors;
		//std::map<QString, int> _materialTransparancy;

		// Container for storing material data
		std::map<int, Material*> _materials;

		
		// ==> addMaterial(material)
		//  Add a material object to the container and add it to the tree widget
		//--------------------------------------------------------------------
		void addMaterial(Material *material)
		{
			if (material == NULL)
				return;

			// see if the material is already defined in the container
			std::map<int, Material*>::iterator iter;
			for(iter = _materials.begin(); iter != _materials.end(); ++iter){
				if ((*iter).first == material->getNumber())
				{
					std::cout << "Warning: Material " << material->getNumber() << " already defined." << std::endl;
					delete (*iter).second;
					(*iter).second = NULL;
					break;
				}
			}
			
			// add material to the container
			_materials[material->getNumber()] = material;

			// add the material to the tree structure
			QStringList params;
			params.clear();
			params << QString::number(material->getNumber()) << material->getName() << "" << QString::number(int(material->getAlpha()*100));
			QTreeWidgetItem* materialItem = new QTreeWidgetItem((QTreeWidget*)0, params);
			QPixmap pix(20, 20);
			pix.fill(material->getColor());
			materialItem->setIcon(2, QIcon(pix));
			materialCardsTree->insertTopLevelItem(indexCounter, materialItem);
			indexCounter++;
		}


		// ==> getMaterialColor(material)
		//--------------------------------------------------------------------
		QColor getMaterialColor(int material)
		{
			return _materials[material]->getColor();
			
		}

		// ==> getMaterialTransparancy(material)
		//--------------------------------------------------------------------
		float getMaterialTransparancy(int material)
		{
			return _materials[material]->getAlpha();
			
		}

		// ==> hasMaterial(material)
		//    Returns if the material exists in the container
		//--------------------------------------------------------------------
		bool hasMaterial(int material)
		{
			if (_materials.find(material) == _materials.end() )
				return false;
			return true;
		}

		// ==> getMaterial(number)
		//   Returns the material object out of the container, based on its number
		//--------------------------------------------------------------------
		Material* getMaterial(int number)
		{
			bool exists = false;
			std::map<int, Material*>::iterator iter;
			for(iter = _materials.begin(); iter != _materials.end(); ++iter){
				if ((*iter).first == number)
				{
					exists = true;
					break;
				}
			}

			if (!exists)
			{
				std::cout << "Warning: couldn't get material " << number << " , material not found." << std::endl;
				return NULL;
			}
				
			return _materials[number];
		}
			

		// ==> updateMaterial(number)
		//   Redraws the material (defined by number) in the tree
		//--------------------------------------------------------------------
		void updateMaterial(int number)
		{
			if (_materials.find(number) == _materials.end() )
			{
				std::cout << "Warning: couldn't update color material " << number << " , material not found." << std::endl;
				return;
			}
			QList<QTreeWidgetItem*> list = materialCardsTree->findItems(QString::number(number), Qt::MatchCaseSensitive, 0);
			QPixmap pix(20, 20);
			pix.fill(_materials[number]->getColor());
			for (int i=0; i<list.size(); i++)
			{
				list[i]->setIcon(2, pix);
				list[i]->setText(3, QString::number(int(_materials[number]->getAlpha()*100)));
			}	
		}

		// ==> updateMaterials()
		//   Redraws all the materials in the tree
		//--------------------------------------------------------------------
		void updateMaterials()
		{
			emptyMaterialsWidget();
			std::map<int, Material*>::const_iterator iter;
			for(iter = _materials.begin(); iter != _materials.end(); ++iter){
				drawMaterial((*iter).second);
			}			
		}

		// ==> drawMaterial(material)
		//   Draw a material in the tree
		//--------------------------------------------------------------------
		void drawMaterial(Material* material)
		{
			QStringList params;
			params.clear();
			params << QString::number(material->getNumber()) << material->getName() << "" << QString::number(int(100*material->getAlpha()));
			QTreeWidgetItem* materialItem = new QTreeWidgetItem((QTreeWidget*)0, params);
			QPixmap pix(20, 20);
			pix.fill(material->getColor());
			materialItem->setIcon(2, QIcon(pix));
			materialCardsTree->insertTopLevelItem(indexCounter, materialItem);
			indexCounter++;
		}

		// ==> clearMaterials()
		//   Remove all the materials out of the tree and the container
		//--------------------------------------------------------------------
		void clearMaterials()
		{
			materialCardsTree->clear();
			indexCounter = 0;

			std::map<int, Material*>::iterator iter;

			for(iter = _materials.begin(); iter != _materials.end(); ++iter){
				delete (*iter).second;
				(*iter).second = NULL;
			}
			_materials.clear();
		}

		// ==> emptyMaterialsWidget()
		//   Remove all the materials only out of the tree widget
		//--------------------------------------------------------------------
		void emptyMaterialsWidget()
		{
			materialCardsTree->clear();
			indexCounter = 0;
		}

		// ==> setupUi(materialCards)
		//   Setup the UI of the widget (with parent materialCards)
		//   Create the basic tree and layout
		//--------------------------------------------------------------------
		void setupUi(QWidget *materialCards)
		{
			indexCounter = 0;

			// Create the tree and add a header
			materialCardsTree = new QTreeWidget();
			materialCardsTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
			materialCardsTree->setColumnCount(3);
			materialCardsTree->setSortingEnabled(true);
			QStringList list;
			list << "Material" << "Name" << "Color" << "Transparancy";
			materialCardsTree->setHeaderLabels(list);

			// Create the basic layout
			materialCardsFormLayOutWidget = new QWidget(materialCards);
			materialCardsFormLayOut = new QVBoxLayout;
			materialCardsFormLayOut->addWidget(materialCardsTree);
			materialCardsFormLayOutWidget->setLayout(materialCardsFormLayOut);

			QMetaObject::connectSlotsByName(materialCards);
		} // setupUi
};

namespace Ui {
    class MaterialCards: public Ui_MaterialCards {};
} // namespace Ui


#endif //
