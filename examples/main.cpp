#include <QCoreApplication>
#include <QCommandLine>

#include "test.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  QCoreApplication::setOrganizationDomain("iksaif.net");
  QCoreApplication::setOrganizationName("iksaif");
  QCoreApplication::setApplicationName("Test");
  QCoreApplication::setApplicationVersion("0.1");

  Test test;

  return app.exec();
}
