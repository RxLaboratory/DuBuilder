#include "builder.h"
#include <QtDebug>

Builder::Builder()
{
    script = nullptr;
    _ignoreJSDoc = false;
    _ignoreBlockComments = false;
    _ignoreLineComments = false;
    _keepLicense = true;
    _progressValue = -1;
}

void Builder::setScript(Script *s)
{
    script = s;
}

void Builder::run()
{
    _progressValue = -1;
    QString builtScript = build(script);

    emit built(builtScript);
}

QString Builder::build(Script *s)
{
    if (script == nullptr) return "";
    QString builtScript = "";

    QFile *scriptFile = s->file();
    QFileInfo scriptInfo(*scriptFile);

    _progressValue++;
    emit progress(_progressValue, "Building " + scriptInfo.fileName());

    //Check script file integrity
    if (!scriptFile->exists())
    {
        return builtScript;
    }
    //open file
    if (!scriptFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return builtScript;
    }

    //add a comment header with the original name of the file

    QString fileName = scriptInfo.baseName();
    QString spacers = "";
    for (int i = 0; i < fileName.count(); ++i)
    {
        spacers += "-";
    }
    builtScript += "// ==================== |-" + spacers + "-| ====================\n";
    builtScript += "// ==================== | " + fileName + " | ====================\n";
    builtScript += "// ==================== |-" + spacers + "-| ====================\n\n";

    //go with data
    int lineNumber = 0;
    QList<Script*> includedScripts = s->includes();
    bool inDoc = false;
    bool inBlockComment = false;
    bool inLicense = false;
    while (!scriptFile->atEnd())
    {
        QString line = scriptFile->readLine();
        lineNumber++;
        QString trimmedLine = line.trimmed();

        //check if there's an include
        QRegularExpression reInclude("#include +(.+);?");
        QRegularExpression reIncludePath("#includepath +(.+);?");

        bool startLicense = trimmedLine.startsWith("/*") && trimmedLine.indexOf("License") >= 0;
        bool startDoc = trimmedLine.startsWith("/**");
        bool startBlockComment = trimmedLine.startsWith("/*") && !startDoc && !startLicense;

        //if #include
        if (reInclude.match(trimmedLine).hasMatch())
        {
            qDebug() << "=== MATCH ===" << line ;
            //get the included script using line number
            QString includedBuild = "";
            foreach(Script *is,includedScripts)
            {
#ifdef QT_DEBUG
                qDebug() << lineNumber ;
                qDebug() << is->name() << is->line();
#endif
                if (is->line() == lineNumber)
                {
                    //build the script
                    includedBuild = build(is);
                    break;
                }
            }
            //TODO Use name if not found by line number
            //Needs to fix the name in the Script class

            //accept if build available
            if (includedBuild != "") line = includedBuild;
        }
        //if #includepath
        else if (reIncludePath.match(line).hasMatch())
        {
            //TODO maybe optionnaly remove it
            //right now, do nothing
        }
        //if documentation, let's remove it
        else if ( startLicense && !_keepLicense ) inLicense = true;
        else if ( startBlockComment && _ignoreJSDoc) inDoc = true;
        else if ( startDoc && _ignoreBlockComments) inBlockComment = true;
        else if (trimmedLine.startsWith("//") && _ignoreLineComments) continue;
        else if (trimmedLine.endsWith("*/") && inDoc)
        {
            inDoc = false;
            continue;
        }
        else if (trimmedLine.endsWith("*/") && inBlockComment)
        {
            inBlockComment = false;
            continue;
        }

        if (inDoc) continue;
        if (inBlockComment) continue;
        if (inLicense) continue;

        //add content
        builtScript += line;
    }

    scriptFile->close();

    return builtScript;
}

void Builder::setKeepLicense(bool keepLicense)
{
    _keepLicense = keepLicense;
}

void Builder::setIgnoreLineComments(bool ignoreLineComments)
{
    _ignoreLineComments = ignoreLineComments;
}

void Builder::setIgnoreBlockComments(bool ignoreBlockComments)
{
    _ignoreBlockComments = ignoreBlockComments;
}

void Builder::setIgnoreJSDoc(bool ignoreJSDoc)
{
    _ignoreJSDoc = ignoreJSDoc;
}
