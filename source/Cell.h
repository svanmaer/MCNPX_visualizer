//#########################################################################################################
//## Cell.h
//#########################################################################################################
//##
//## Contains card number, optional name, color and alpha value of a material
//##
//## Part of MCNPX Visualiser
//## (c) Nick Michiels for SCK-CEN Mol (2011)
//#########################################################################################################

#ifndef CELL_H
#define CELL_H

#include <QString>

struct Cell
{
	public:
		Cell(int number, int material = 0)
			: _number(number), _material(material)
		{
			_density = 0.0;
		}
		~Cell(){}

		
		void setNumber(int number){  _number = number; }
		int getNumber(){ return _number; }
		
		void setMaterial(int material){ _material = material; }
		int getMaterial(){ return _material;}

		void setDensity(float density){ _density = density; }
		float getDensity(){ return _density;}

		void setGeometry(QString geometry){ _geometry = geometry; }
		QString getGeometry(){ return _geometry;}

		void setParameters(QString parameters){ _parameters = parameters; }
		QString getParameters(){ return _parameters;}


	private:
		int _number;
		int _material;
		float _density;
		QString _geometry;
		QString _parameters;
};

#endif