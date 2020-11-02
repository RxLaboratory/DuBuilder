#ifndef BUILDER_H
#define BUILDER_H

#include <QObject>
#include <QFile>
#include <QRegularExpressionMatch>
#include <QThread>
#include <QDir>
#include <QProcess>

#include "script.h"

class Builder : public QThread
{
    Q_OBJECT
public:
    Builder();
    /**
     * @brief Sets the current script which has to be built
     * @param s The script
     */
    void setScript(Script *s);
    void setIgnoreJSDoc(bool ignoreJSDoc);
    void setIgnoreBlockComments(bool ignoreBlockComments);
    void setIgnoreLineComments(bool ignoreLineComments);
    void setKeepLicense(bool keepLicense);

public slots:
    /**
     * @brief Builds the current script
     */
    void run();

signals:
    /**
     * @brief Emitted when the builder finishes
     * @param builtScript The built script (empty string if anything went wrong)
     */
    void built(QString builtScript);
    /**
     * @brief Emitted for each script being built. Connected to the progress bar of the UI.
     * @param i The progress value
     * @param message A message to be displayed
     */
    void progress(int i, QString message);

private:
    /**
     * @brief The current script which will be built when run() is called
     */
    Script *script;
    /**
     * @brief Recursive method which builds the scripts and all includes
     * @param s The script to build
     * @return The built script
     */
    QString build(Script *s);
    /**
     * @brief _ignoreJSDoc Wether to keep the JSDoc in the resulting script
     */
    bool _ignoreJSDoc;
    /**
     * @brief _ignoreBlockComments Ignores blocks comments
     */
    bool _ignoreBlockComments;
    /**
     * @brief _ignoreLineComments Ignores lines starting with //
     */
    bool _ignoreLineComments;
    /**
     * @brief _keepLicense Keeps comment blocks starting with License
     */
    bool _keepLicense;
    /**
     * @brief The value used in the progress() signal to update the progress bar in the UI
     */
    int _progressValue;
};

#endif // BUILDER_H
