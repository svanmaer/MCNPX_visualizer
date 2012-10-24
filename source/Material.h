//#########################################################################################################
//## Material.h
//#########################################################################################################
//##
//## Contains card number, optional name, color and alpha value of a material
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef MATERIAL_H
#define MATERIAL_H

#include <QString>

struct Material
{
	public:
		Material(int number, QString name = "")
			: _number(number), _name(name)
		{
			_alpha = 1.0;
		}
		~Material(){}

		
		void setNumber(int number){  _number = number; }
		int getNumber(){ return _number; }
		
		void setName(QString name){ _name = name; }
		QString getName(){ return _name;}

		void setColor(QColor color){ _color = color; }
		QColor getColor(){ return _color; }
		int getRed(){ return _color.red(); }
		int getGreen(){ return _color.green(); }
		int getBlue(){ return _color.blue(); }

		void setAlpha(float alpha){ _alpha = alpha; }
		float getAlpha(){ return _alpha; }



	private:
		int _number;
		QString _name;
		QColor _color;
		float _alpha;
};

#endif