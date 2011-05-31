#include <QCommandLine>
#include <QApplication>
#include <QDebug>
#include <QVariant>

#include "test.h"

Test::Test()
{
  static const struct QCommandLineConfigEntry conf[] =
    {
      { QCommandLine::Option, 'v', "verbose", "Verbose level (0-3)", QCommandLine::Mandatory },
      { QCommandLine::Switch, 'l', "list", "Show a list", QCommandLine::Optional },
      { QCommandLine::Param, '\0', "target", "The target", QCommandLine::Mandatory },
      { QCommandLine::Param, '\0', "source", "The sources", QCommandLine::MandatoryMultiple },
      QCOMMANDLINE_CONFIG_ENTRY_END
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
  qWarning() << "Switch:" << name;
}

void
Test::optionFound(const QString & name, const QVariant & value)
{
  qWarning() << "Option:" << name << value.toString();
}

void
Test::paramFound(const QString & name, const QVariant & value)
{
  qWarning() << "Param:" << name << value.toString();
}

void
Test::parseError(const QString & error)
{
  qWarning() << qPrintable(error);
  cmdline->showHelp(true, -1);
  QCoreApplication::quit();
}
