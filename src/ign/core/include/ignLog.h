///\file
/// Log utilities, from PVLE and RCP source

#ifndef IGN_UTILS_LOG_H
#define IGN_UTILS_LOG_H

#include <iostream>

#include <ignConfig.h>

namespace ign {
	enum NotifySeverity {
		ALWAYS=0,
		FATAL=1,
		WARN=2,
		NOTICE=3,
		INFO=4,
		DEBUG_INFO=5,
		DEBUG_FP=6
	};
	// Temporary implementation which will allow coders to do as if the proper code was there
	// Missing features: set/get notify handler (Note: how to cope with OSG???)
	namespace detail {
		/*extern*/ IGN_CORE_API void setNotifyLevel(NotifySeverity severity);
		/*extern*/ IGN_CORE_API NotifySeverity getNotifyLevel();
		/*extern*/ IGN_CORE_API bool initNotifyLevel();
		/*extern*/ IGN_CORE_API bool isNotifyEnabled(NotifySeverity severity);
		/*extern*/ IGN_CORE_API std::ostream& notify(const NotifySeverity severity, const char * file, int line, const char * function);
		//inline std::ostream & notify(const NotifySeverity severity) {
		//	if (severity==FATAL || severity==WARN) return std::cerr;
		//	return std::cout;
		//}
	}


	/// Additional information to prepend to logs.
	enum NotifyAdditionalInfo {
		NOTIFY_ADD_NONE,		///< Add no information to logs
		NOTIFY_ADD_SHORT,		///< Add short information to logs (current funtion)
		NOTIFY_ADD_FULL			///< Add all informations to logs (current function, file, line)
	};

	/// Set information to add to logs.
	/*extern*/ IGN_CORE_API void setNotifyAdditionalInformation(NotifyAdditionalInfo info);
	/*extern*/ IGN_CORE_API NotifyAdditionalInfo getNotifyAdditionalInformation();
	/*extern*/ IGN_CORE_API std::string getNotifyAdditionalInformationString(const char * file, int line, const char * function);

	inline void setNotifyLevel(NotifySeverity severity) { detail::setNotifyLevel(severity); }
	inline NotifySeverity getNotifyLevel() { return detail::getNotifyLevel(); }
	inline bool initNotifyLevel() { return detail::initNotifyLevel(); }
	inline bool isNotifyEnabled(NotifySeverity severity) { return detail::isNotifyEnabled(severity); }
	inline std::ostream& notify(const NotifySeverity severity, const char * file, int line, const char * function) { return detail::notify(severity, file, line, function); }
}

// --------------------------------------------
// Defines
// --------------------------------------------

#include <boost/current_function.hpp>

// Log defines

#define OSG_NOTIFY(level) ign::notify(level, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)
#ifndef OSG_ALWAYS
#define OSG_ALWAYS OSG_NOTIFY(ign::ALWAYS)
#endif
#ifndef OSG_FATAL
#define OSG_FATAL OSG_NOTIFY(ign::FATAL)
#endif
#ifndef OSG_WARN
#define OSG_WARN OSG_NOTIFY(ign::WARN)
#endif
#ifndef OSG_NOTICE
#define OSG_NOTICE OSG_NOTIFY(ign::NOTICE)
#endif
#ifndef OSG_INFO
#define OSG_INFO OSG_NOTIFY(ign::INFO)
#endif
#ifndef OSG_DEBUG
#define OSG_DEBUG OSG_NOTIFY(ign::DEBUG_INFO)
#endif
#ifndef OSG_DEBUG_FP
#define OSG_DEBUG_FP OSG_NOTIFY(ign::DEBUG_FP)
#endif

#define LOG_ALWAYS		OSG_ALWAYS
#define LOG_FATAL		OSG_FATAL
#define LOG_WARN		OSG_WARN
#define LOG_NOTICE		OSG_NOTICE
#define LOG_INFO		OSG_INFO
#define LOG_DEBUG		OSG_DEBUG
#define LOG_DEBUG_FP	OSG_DEBUG_FP

/// Class used to debug a given function by raising the log level until scope exit.
///\warning Use this only for debugging since it will overwrite the log level at scope exit; except with the second constructor with activate==false.
class ScopedLogLevel {
public:
	/// If force is false, then increases the log level to the given one (it will not decrease the log level); else forces the given log level.
	ScopedLogLevel(ign::NotifySeverity severity, bool force=false) : activated(true), oldSeverity(ign::getNotifyLevel()) {
		if (force || severity>oldSeverity) ign::setNotifyLevel(severity);
	}
	/// Same as previous constructor, except you can give a "verbose flag" (activate==false) to simply turn the effect off.
	ScopedLogLevel(bool activate, ign::NotifySeverity severity, bool force=false) : activated(activate), oldSeverity(ign::getNotifyLevel()) {
		if (activate && (force || severity>oldSeverity)) ign::setNotifyLevel(severity);
	}
	~ScopedLogLevel() { if (activated) ign::setNotifyLevel(oldSeverity); }
private:
	bool activated;
	const ign::NotifySeverity oldSeverity;
};


//VCTOOLS_EXPORT const char * getLogPrefix(osg::NotifySeverity severity);

namespace ign {
	inline std::string ident(unsigned int i=1) {
		return std::string("  ", i);		// Need to handle case i==0???
	}
}

#endif // UTILS_LOG_H
