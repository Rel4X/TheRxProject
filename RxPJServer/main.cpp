#include "RxProject.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RxProject w;
	w.show();
	return a.exec();
}
