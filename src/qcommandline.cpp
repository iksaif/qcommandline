#include <QtCore/QCoreApplication>
#include <QtCore/QQueue>
#include <QtCore/QVariant>
#include <QtCore/QFileInfo>
#include <QDebug>
#include <iostream>

#include "qcommandline.h"

const QCommandLineConfigEntry QCommandLine::helpEntry = { QCommandLine::Switch, 'h', "help", "Display this help and exit", QCommandLine::Optional };

const QCommandLineConfigEntry QCommandLine::versionEntry = { QCommandLine::Switch, 'V', "version", "Display version and exit", QCommandLine::Optional };

QCommandLine::QCommandLine(QObject * parent)
  : QObject(parent)
{
  setArguments(QCoreApplication::instance()->arguments());
}

QCommandLine::QCommandLine(const QCoreApplication & app,
			   const QCommandLineConfig & config,
			   QObject * parent)
  : QObject(parent)
{
  setArguments(app.arguments());
  setConfig(config);
  enableHelp(true);
  enableVersion(true);
}

QCommandLine::QCommandLine(int argc, char *argv[],
			   const QCommandLineConfig & config,
			   QObject * parent)
  : QObject(parent)
{
  setArguments(argc, argv);
  setConfig(config);
  enableHelp(true);
  enableVersion(true);
}

QCommandLine::QCommandLine(const QStringList args,
			   const QCommandLineConfig & config,
			   QObject * parent)
  : QObject(parent)
{
  setArguments(args);
  setConfig(config);
  enableHelp(true);
  enableVersion(true);
}

QCommandLine::~QCommandLine()
{
}

void
QCommandLine::setConfig(const QCommandLineConfig & config)
{
  d.config = config;
}

void
QCommandLine::setConfig(const QCommandLineConfigEntry config[])
{
  d.config.clear();

  while (config->type) {
    d.config << *config;
    config++;
  }
}

QCommandLineConfig
QCommandLine::config()
{
  return d.config;
}

void
QCommandLine::setArguments(int argc, char *argv[])
{
  d.args.clear();
  for (int i = 0; i < argc; i++)
    d.args.append(argv[i]);
}

void
QCommandLine::setArguments(QStringList args)
{
  d.args = args;
}

QStringList
QCommandLine::arguments()
{
  return d.args;
}

void
QCommandLine::enableHelp(bool enable)
{
  d.help = enable;
}

bool
QCommandLine::helpEnabled()
{
  return d.help;
}

void
QCommandLine::enableVersion(bool enable)
{
  d.version = enable;
}

bool
QCommandLine::versionEnabled()
{
  return d.version;
}

bool
QCommandLine::parse()
{
  QMap < QString, QCommandLineConfigEntry > conf;
  QMap < QString, QCommandLineConfigEntry > confLong;
  QQueue < QCommandLineConfigEntry > params;
  QMap < QString, QList < QString > > optionsFound;
  QMap < QString, int > switchsFound;
  QStringList args = d.args;

  bool allparam = false;

  foreach (QCommandLineConfigEntry entry, d.config) {
    if (entry.type != QCommandLine::Param && entry.shortName == '\0')
      qWarning() << "QCommandLine: Empty shortname detected";
    if (entry.longName.isEmpty())
      qWarning() << "QCommandLine: Empty shortname detected";
    if (entry.type != QCommandLine::Param && conf.find(entry.shortName) != conf.end())
      qWarning() << "QCommandLine: Duplicated shortname detected " << entry.shortName;
    if (conf.find(entry.longName) != conf.end())
      qWarning() << "QCommandLine: Duplicated longname detected " << entry.shortName;

    if (entry.type == QCommandLine::Param)
      params << entry;
    else
      conf[entry.shortName] = entry;
    confLong[entry.longName] = entry;
  }

  if (d.help) {
    conf[helpEntry.shortName] = helpEntry;
    confLong[helpEntry.longName] = helpEntry;
  }

  if (d.version) {
    conf[versionEntry.shortName] = versionEntry;
    confLong[versionEntry.longName] = versionEntry;
  }

  for (int i = 1; i < args.size(); ++i) {
    QString arg = args[i];
    bool param = true, shrt = false, stay = false, forward = false;

    /* A '+' was found, all remaining options are params */
    if (allparam)
      param = true;
    else if (arg.startsWith("--")) {
      param = false;
      shrt = false;
      arg = arg.mid(2);
    } else if (arg.startsWith('-') || arg.startsWith('+')) {
      if (arg.startsWith('+'))
	allparam = true;
      param = false;
      shrt = true;
      /* Handle stacked args like `tar -xzf` */
      if (arg.size() > 2) {
	args[i] = arg.mid(0, 1) + arg.mid(2);
	arg = arg.mid(1, 1);
      } else {
	arg = arg.mid(1);
      }
    }

    /* Handle params */
    if (param) {
      if (!params.size()) {
	emit parseError(QString("Unknown param: \"%1\"").arg(arg));
	return false;
      }

      QCommandLineConfigEntry & entry = params.first();

      if (entry.flags & QCommandLine::Mandatory) {
	entry.flags = (QCommandLine::Flags) (entry.flags & ~QCommandLine::Mandatory);
	entry.flags = (QCommandLine::Flags) (entry.flags | QCommandLine::Optional);
      }

      emit paramFound(entry.longName, arg);

      if (!(entry.flags & QCommandLine::Multiple))
	params.dequeue();

    } else { /* Options and switchs* */
      QString key;
      QString value;
      int idx = arg.indexOf('=');

      if (idx != -1) {
	key = arg.mid(0, idx);
	value = arg.mid(idx + 1);
      } else {
	key = arg;
      }

      QMap < QString, QCommandLineConfigEntry > & c = shrt ? conf : confLong;

      if (c.find(key) == c.end()) {
	emit parseError("Unknown option: " + key);
	return false;
      }

      QCommandLineConfigEntry & entry = c[key];

      if (entry.type == QCommandLine::Switch) {
	if (entry.flags & QCommandLine::Multiple)
	  switchsFound[entry.longName]++;
	else
	  switchsFound[entry.longName] = 1;
      } else {
	if (stay) {
	  emit parseError("Option " + key + " need a value");
	  return false;
	}

	if (idx == -1) {
	  if (i+1 < args.size() && !args[i+1].startsWith("-")) {
	    value = args[i+1];
	    forward = true;
	  } else {
	    emit parseError("Option " + key + " need a value");
	    return false;
	  }
	}

	if (!(entry.flags & QCommandLine::Multiple))
	  optionsFound[entry.longName].clear();
	optionsFound[entry.longName].append(value);
      }

      if (entry.flags & QCommandLine::Mandatory) {
	entry.flags = (QCommandLine::Flags) (entry.flags & ~QCommandLine::Mandatory);
	entry.flags = (QCommandLine::Flags) (entry.flags | QCommandLine::Optional);
	conf[entry.shortName] = entry;
	confLong[entry.shortName] = entry;
      }
    }
    /* Stay here, stacked args */
    if (stay)
      i--;
    else if (forward)
      i++;
  }

  foreach (QCommandLineConfigEntry entry, params) {
    if (entry.flags & QCommandLine::Mandatory) {
      emit parseError("Param " + entry.longName + " is mandatory");
      return false;
    }
  }

  foreach (QCommandLineConfigEntry entry, conf.values()) {
    if (entry.flags & QCommandLine::Mandatory) {
      QString type;

      if (entry.type == QCommandLine::Switch)
	type = "Switch";
      if (entry.type == QCommandLine::Option)
	type = "Option";

      emit parseError(type + " " + entry.longName + " is mandatory");
      return false;
    }
  }

  foreach (QString key, switchsFound.keys()) {
    for (int i = 0; i < switchsFound[key]; i++) {
      if (d.help && key == helpEntry.longName)
	showHelp();
      if (d.version && key == versionEntry.longName)
	showVersion();
      emit switchFound(key);
    }
  }

  foreach (QString key, optionsFound.keys()) {
    foreach (QString opt, optionsFound[key])
      emit optionFound(key, opt);
  }  return true;
}

void
QCommandLine::addOption(const QChar & shortName,
			const QString & longName,
			const QString & descr,
			QCommandLine::Flags flags)
{
  QCommandLineConfigEntry entry;

  entry.type = QCommandLine::Option;
  entry.shortName = shortName;
  entry.longName = longName;
  entry.descr = descr;
  entry.flags = flags;
  d.config << entry;
}

void
QCommandLine::addSwitch(const QChar & shortName,
			const QString & longName,
			const QString & descr,
			QCommandLine::Flags flags)
{
  QCommandLineConfigEntry entry;

  entry.type = QCommandLine::Switch;
  entry.shortName = shortName;
  entry.longName = longName;
  entry.descr = descr;
  entry.flags = flags;
  d.config << entry;
}

void
QCommandLine::addParam(const QString & name,
		       const QString & descr,
		       QCommandLine::Flags flags)
{
  QCommandLineConfigEntry entry;

  entry.type = QCommandLine::Param;
  entry.longName = name;
  entry.descr = descr;
  entry.flags = flags;
  d.config << entry;
}

void
QCommandLine::removeOption(const QString & name)
{
  int i;

  for (i = 0; i < d.config.size(); ++i) {
    if (d.config[i].type == QCommandLine::Option &&
	(d.config[i].shortName == name.at(0) || d.config[i].longName == name)) {
      d.config.removeAt(i);
      return ;
    }
  }
}

void
QCommandLine::removeSwitch(const QString & name)
{
  int i;

  for (i = 0; i < d.config.size(); ++i) {
    if (d.config[i].type == QCommandLine::Switch &&
	(d.config[i].shortName == name.at(0) || d.config[i].longName == name)) {
      d.config.removeAt(i);
      return ;
    }
  }
}

void
QCommandLine::removeParam(const QString & name)
{
  int i;

  for (i = 0; i < d.config.size(); ++i) {
    if (d.config[i].type == QCommandLine::Param &&
	(d.config[i].shortName == name.at(0) || d.config[i].longName == name)) {
      d.config.removeAt(i);
      return ;
    }
  }
}


QString
QCommandLine::help(bool logo)
{
  QString h;

  if (logo)
    h = version() + "\n";
  h = "Usage:\n   ";
  /* Executable name */
  if (!d.args.isEmpty())
    h += QFileInfo(d.args[0]).baseName();
  else
    h += QCoreApplication::applicationName();
  h.append(" [switchs] [options]");
  /* Arguments, short */
  foreach (QCommandLineConfigEntry entry, d.config) {
    if (entry.type == QCommandLine::Option) {
      if (entry.flags & QCommandLine::Mandatory)
	h.append(" --" + entry.longName + "=<val>");
    }
    if (entry.type == QCommandLine::Param) {
      h.append(" ");
      if (entry.flags & QCommandLine::Optional)
	h.append("[");
      h.append(entry.longName);
      if (entry.flags & QCommandLine::Multiple)
	h.append(" [" + entry.longName +" [...]]");
      if (entry.flags & QCommandLine::Optional)
	h.append("]");
    }
  }
  h.append("\n\n");

  h.append("Options:\n");

  QStringList vals;
  QStringList descrs;
  int max = 0;

  foreach (QCommandLineConfigEntry entry, d.config) {
    QString val;

    if (entry.type == QCommandLine::Option)
      val = "-" + QString(entry.shortName) + ",--" + entry.longName + "=<val>";
    if (entry.type == QCommandLine::Switch)
      val = "-" + QString(entry.shortName) + ",--" + entry.longName;
    if (entry.type == QCommandLine::Param)
      val = entry.longName;

    if (val.size() > max)
      max = val.size();

    vals.append(val);
    descrs.append(entry.descr + "\n");
  }

  for (int i = 0; i < vals.size(); ++i) {
    h.append(QString("  "));
    h.append(vals[i]);
    h.append(QString(" ").repeated(max - vals[i].size() + 2));
    h.append(descrs[i]);
  }

  h.append("\nMandatory arguments to long options are mandatory for short options too.\n");

  return h;
}

QString
QCommandLine::version()
{
  QString v;

  v = QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion();
  if (!QCoreApplication::organizationDomain().isEmpty() || !QCoreApplication::organizationName().isEmpty())
    v = v + " - " + QCoreApplication::organizationDomain() + " " + QCoreApplication::organizationDomain();
  return v + "\n";
}

void
QCommandLine::showHelp(bool quit, int returnCode)
{
  std::cerr << qPrintable(help());
  if (quit) {
    // Can't call QApplication::exit() here, because we may be called before app.exec()
    exit(returnCode);
  }
}

void
QCommandLine::showVersion(bool quit, int returnCode)
{
  std::cerr << qPrintable(version());
  if (quit) {
    exit(returnCode);
  }
}

