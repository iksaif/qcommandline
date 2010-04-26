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

/**
 * @brief Main class used to convert parse command line
 */
class QCOMMANDLINE_EXPORT QCommandLine : public QObject
{
  Q_OBJECT
public:
    /**
     * Enum used to determine entry type in QCommandLineConfigEntry
     */
    typedef enum {
	None = 0, /**< can be used for the last line of a QCommandLineConfigEntry[] . */
	Switch, /**< a simple switch wihout argument (eg: ls -l) */
	Option, /**< an option with an argument (eg: tar -f test.tar) */
	Param /**< a parameter without '-' delimiter (eg: cp foo bar) */
    } Type;

    /**
     * Flags that can be applied to a QCommandLineConfigEntry
     */
    typedef enum {
	Default = 0, /**< can be used for the last line of a QCommandLineConfigEntry[] . */
	Mandatory = 0x01, /**< mandatory argument, will produce a parse error if not present */
	Optional = 0x02, /**< optional argument */
	Multiple = 0x04, /**< argument can be used multiple time and will produce multiple signals. */
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

    /**
     * Parse command line and emmit signals when switchs, options, or
     * param are found.
     * @returns true if successfully parsed; otherwise returns false.
     * @sa parseError
     */
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

    /**
     * Standard --help, -h entry
     */
    static const QCommandLineConfigEntry helpEntry;

    /**
     * Standard --version, -V entry
     */
    static const QCommandLineConfigEntry versionEntry;

signals:
    /**
     * Signal emitted when a switch is found while parsing
     * @param name The "longName" of the switch.
     * @sa parse
     * @sa addSwitch
     */
    void switchFound(const QString & name);

    /**
     * Signal emitted when an option is found while parsing
     * @param name The "longName" of the switch.
     * @param value The value of that option
     * @sa parse
     * @sa addSwitch
     */
    void optionFound(const QString & name, const QVariant & value);

    /**
     * Signal emitted when a param is found while parsing
     * @param name The "longName" of the switch.
     * @param value The actual argument
     * @sa parse
     * @sa addSwitch
     */
    void paramFound(const QString & name, const QVariant & value);

    /**
     * Signal emitted when a parse error is detected
     * @param error Parse error description
     * @sa parse
     */
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
