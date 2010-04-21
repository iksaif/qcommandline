#include <QCommandLine>
#include <QApplication>
#include <QDebug>
#include <QVariant>

#include "test.h"

Test::Test()
{
  static const struct QCommandLineConfigEntry conf[] =
    {
      { QCommandLine::Option, "v", "verbose", "verbose level (0-3)", QCommandLine::Mandatory },
      { QCommandLine::Switch, "l", "list", "show a list", QCommandLine::Optional },
      { QCommandLine::Param, NULL, "target", "the target", QCommandLine::Mandatory },
      { QCommandLine::Param, NULL, "source", "the sources", QCommandLine::MandatoryMultiple },
      { QCommandLine::None, NULL, NULL, NULL, QCommandLine::Default }
    };
  /*
   * Similar to:
   * cmdline->addOption("v", "verbose", "verbose level (0-3)");
   * cmdline->addSwitch("l", "list", "show a list")
   * cmdline->addParam("source", "the sources", QCommandLine::Mandatory|QCommandLine::Multiple);
   * cmdline->addParam("target", "the target", QCommandLine::Mandatory);
   */

   cmdline = new QCommandLine(this);
   cmdline->setConfig(conf);
   cmdline->enableVersion(true); // enable -v // --version
   cmdline->enableHelp(true); // enable -h / --help

  connect(cmdline, SIGNAL(switchFound(const QString &)),
	  this, SLOT(switchFound(const QString &)));
  connect(cmdline, SIGNAL(optionFound(const QString &, const QVariant &)),
	  this, SLOT(optionFound(const QString &, const QVariant &)));
  connect(cmdline, SIGNAL(paramFound(const QString &, const QVariant &)),
	  this, SLOT(paramFound(const QString &, const QVariant &)));
  connect(cmdline, SIGNAL(parseError(const QString &)),
	  this, SLOT(parseError(const QString &)));

  cmdline->parse();
}

void
Test::switchFound(const QString & name)
{
  qDebug() << "Switch:" << name;
}

void
Test::optionFound(const QString & name, const QVariant & value)
{
  qDebug() << "Option:" << name << value;
}

void
Test::paramFound(const QString & name, const QVariant & value)
{
  qDebug() << "Param:" << name << value;
}

void
Test::parseError(const QString & error)
{
  qDebug() << qPrintable(error);
  cmdline->showHelp(true, -1);
  QCoreApplication::quit();
}
