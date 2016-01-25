#ifndef LOGFILEHANDLER
#define LOGFILEHANDLER

/* LogFileHandler
 * A class to redirect the OSG internal notify messages
 * to an external log file
 *
 * The usage:
 *
 * //...
 * osg::setNotifyLevel(osg::INFO);
 * osg::setNotifyHandler(new LogFileHandler("out_filename.txt"));
 * //...
 *
 */

#include <osgDB/ReadFile>
#include <fstream>

class LogFileHandler : public osg::NotifyHandler {
public:
    LogFileHandler(const std::string& file){
        _log.open(file.c_str());
    }

    virtual ~LogFileHandler(){
        _log.close();
    }

    virtual void notify(osg::NotifySeverity severity, const char *message){
        _log << msg;
    }
protected:
    std::ofstream _log;
};

#endif // LOGFILEHANDLER

