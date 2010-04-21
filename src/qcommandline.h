#ifndef QCOMMAND_LINE_H
# define QCOMMAND_LINE_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QStringList>

#ifndef QCOMMANDLINE_EXPORT
# if defined(QCOMMANDLINE_MAKEDLL)
   /* We are building this library */
#  define QCOMMANDLINE_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define QCOMMANDLINE_EXPORT Q_DECL_IMPORT
# endif
#endif

class QCoreApplication;

struct QCommandLineConfigEntry;
typedef QList< QCommandLineConfigEntry > QCommandLineConfig;

struct QCommandLinePrivate {
    bool version;
    bool help;
    QStringList args;
    QCommandLineConfig config;
};
#define QCOMMANDLINE_CONFIG_ENTRY_END       { QCommandLine::None, NULL, NULL, NULL, QCommandLine::Default }

class QCOMMANDLINE_EXPORT QCommandLine : public QObject
{
  Q_OBJECT
public:
    typedef enum { None = 0, Switch, Option, Param } Type;
    typedef enum {
	Default = 0, Mandatory = 0x01, Optional = 0x02, Multiple = 0x04,
	MandatoryMultiple = Mandatory|Multiple,
	OptionalMultiple = Optional|Multiple,
    } Flags;

    QCommandLine(QObject * parent = 0);
    QCommandLine(const QCoreApplication & app,
		 const QCommandLineConfig & config = QCommandLineConfig(),
		 QObject * parent = 0);
    QCommandLine(int argc, char *argv[],
		 const QCommandLineConfig & config = QCommandLineConfig(),
		 QObject * parent = 0);
    QCommandLine(const QStringList args,
		 const QCommandLineConfig & config = QCommandLineConfig(),
		 QObject * parent = 0);
    ~QCommandLine();

    void setConfig(const QCommandLineConfig & config);
    void setConfig(const QCommandLineConfigEntry config[]);
    QCommandLineConfig config();
    void setArguments(int argc, char *argv[]);
    void setArguments(QStringList args);
    QStringList arguments();
    void enableHelp(bool enable);
    bool helpEnabled();
    void enableVersion(bool enable);
    bool versionEnabled();

    bool parse();

    void addOption(const QString & shortName,
		   const QString & longName = QString(),
		   const QString & descr = QString(),
		   QCommandLine::Flags flags = QCommandLine::Optional);
    void addSwitch(const QString & shortName,
		   const QString & longName = QString(),
		   const QString & descr = QString(),
		   QCommandLine::Flags flags = QCommandLine::Optional);
    void addParam(const QString & name,
		  const QString & descr = QString(),
		  QCommandLine::Flags flags = QCommandLine::Optional);

    void removeOption(const QString & name);
    void removeSwitch(const QString & name);
    void removeParam(const QString & name);

    QString help(bool logo = true);
    QString version();

    void showHelp(bool exit = true, int returnCode = 0);
    void showVersion(bool exit = true, int returnCode = 0);

    static const QCommandLineConfigEntry helpEntry;
    static const QCommandLineConfigEntry versionEntry;

signals:
    void switchFound(const QString & name);
    void optionFound(const QString & name, const QVariant & value);
    void paramFound(const QString & name, const QVariant & value);
    void parseError(const QString & error);
private:
    QCommandLinePrivate d;
    Q_DECLARE_PRIVATE(QCommandLine);
};

struct QCommandLineConfigEntry {
    QCommandLine::Type type;
    QString shortName;
    QString longName;
    QString descr;
    QCommandLine::Flags flags;
};

#endif
