#include <QtGui/QApplication>
#include "MCNPXVisualizer.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MCNPXVisualizer w(&a);
	w.show();
	return a.exec();

}
