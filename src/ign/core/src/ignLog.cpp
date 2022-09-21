
#include <ignConfig.h>
#include <ignLog.h>

#include <boost/algorithm/string/case_conv.hpp>

// Code from OSG

#ifndef OSG_FOUND

#include <ostream>
#include <sstream>
#include <cstdlib>


namespace ign {
namespace detail {

	class IGN_CORE_API NullStreamBuffer : public std::streambuf
	{
	private:
		std::streamsize xsputn(const std::streambuf::char_type *str, std::streamsize n)
		{
			return n;
		}
	};

	struct IGN_CORE_API NullStream : public std::ostream
	{
	public:
		NullStream():
		  std::ostream(new NullStreamBuffer)
		  { _buffer = dynamic_cast<NullStreamBuffer *>(rdbuf()); }

		  ~NullStream()
		  {
			  rdbuf(0);
			  delete _buffer;
		  }

	protected:
		NullStreamBuffer* _buffer;
	};

	struct IGN_CORE_API NotifyStreamBuffer : public std::stringbuf
	{
		NotifyStreamBuffer() : _severity(NOTICE)
		{
		}

		/** Sets severity for next call of notify handler */
		void setCurrentSeverity(NotifySeverity severity) { _severity = severity; }
		NotifySeverity getCurrentSeverity() const { return _severity; }

	private:

		int sync()
		{
			sputc(0); // string termination
			//if (_handler.valid())
			//	_handler->notify(_severity, pbase());
			if (_severity==FATAL || _severity==WARN) std::cerr << pbase();		//SKD: replacement for handlers
			else std::cout << pbase();
			pubseekpos(0, std::ios_base::out); // or str(std::string())
			return 0;
		}

		//osg::ref_ptr<osg::NotifyHandler> _handler;
		NotifySeverity _severity;
	};

	struct IGN_CORE_API NotifyStream : public std::ostream
	{
	public:
		NotifyStream():
		  std::ostream(new NotifyStreamBuffer)
		  { _buffer = dynamic_cast<NotifyStreamBuffer *>(rdbuf()); }

		  void setCurrentSeverity(NotifySeverity severity)
		  {
			  _buffer->setCurrentSeverity(severity);
		  }

		  NotifySeverity getCurrentSeverity() const
		  {
			  return _buffer->getCurrentSeverity();
		  }

		  ~NotifyStream()
		  {
			  rdbuf(0);
			  delete _buffer;
		  }

	protected:
		NotifyStreamBuffer* _buffer;
	};





	static bool s_NeedNotifyInit = true;
	static NotifySeverity g_NotifyLevel = NOTICE;
	static NullStream *g_NullStream(NULL);
	static NotifyStream *g_NotifyStream(NULL);
	static NotifyAdditionalInfo g_NotifyAdd = NOTIFY_ADD_NONE;		// SKD: Addition

	IGN_CORE_API void setNotifyLevel(NotifySeverity severity) {
		if (s_NeedNotifyInit) ::ign::detail::initNotifyLevel();
		g_NotifyLevel = severity;
	}

	IGN_CORE_API NotifySeverity getNotifyLevel() {
		if (s_NeedNotifyInit) ::ign::detail::initNotifyLevel();
		return g_NotifyLevel;
	}

	IGN_CORE_API bool initNotifyLevel() {
		static NullStream s_NullStream;
		static NotifyStream s_NotifyStream;

		g_NullStream = &s_NullStream;
		g_NotifyStream = &s_NotifyStream;

		// g_NotifyLevel
		// =============

		g_NotifyLevel = NOTICE; // Default value

		char* OSGNOTIFYLEVEL=getenv("OSG_NOTIFY_LEVEL");
		if (!OSGNOTIFYLEVEL) OSGNOTIFYLEVEL=getenv("OSGNOTIFYLEVEL");
		if (!OSGNOTIFYLEVEL) OSGNOTIFYLEVEL=getenv("IGN_NOTIFY_LEVEL");		//SKD: addition
		if(OSGNOTIFYLEVEL)
		{
			std::string stringOSGNOTIFYLEVEL(OSGNOTIFYLEVEL);

			// Convert to upper case
			boost::algorithm::to_upper(stringOSGNOTIFYLEVEL);

			if(stringOSGNOTIFYLEVEL.find("ALWAYS")!=std::string::npos)          g_NotifyLevel=ALWAYS;
			else if(stringOSGNOTIFYLEVEL.find("FATAL")!=std::string::npos)      g_NotifyLevel=FATAL;
			else if(stringOSGNOTIFYLEVEL.find("WARN")!=std::string::npos)       g_NotifyLevel=WARN;
			else if(stringOSGNOTIFYLEVEL.find("NOTICE")!=std::string::npos)     g_NotifyLevel=NOTICE;
			else if(stringOSGNOTIFYLEVEL.find("DEBUG_INFO")!=std::string::npos) g_NotifyLevel=DEBUG_INFO;
			else if(stringOSGNOTIFYLEVEL.find("DEBUG_FP")!=std::string::npos)   g_NotifyLevel=DEBUG_FP;
			else if(stringOSGNOTIFYLEVEL.find("DEBUG")!=std::string::npos)      g_NotifyLevel=DEBUG_INFO;
			else if(stringOSGNOTIFYLEVEL.find("INFO")!=std::string::npos)       g_NotifyLevel=INFO;
			else std::cout << "Warning: invalid OSG_NOTIFY_LEVEL set ("<<stringOSGNOTIFYLEVEL<<")"<<std::endl;
		}

		//SKD: addition
		g_NotifyAdd = NOTIFY_ADD_NONE;

		char* OSG_NOTIFY_ADDITIONAL=getenv("OSG_NOTIFY_ADDITIONAL");
		if (!OSG_NOTIFY_ADDITIONAL) OSGNOTIFYLEVEL=getenv("IGN_NOTIFY_ADDITIONAL");
		if(OSG_NOTIFY_ADDITIONAL)
		{
			std::string stringOSG_NOTIFY_ADDITIONAL(OSG_NOTIFY_ADDITIONAL);

			// Convert to upper case
			boost::algorithm::to_upper(stringOSG_NOTIFY_ADDITIONAL);

			if(stringOSG_NOTIFY_ADDITIONAL.find("NONE")!=std::string::npos)         g_NotifyAdd=NOTIFY_ADD_NONE;
			else if(stringOSG_NOTIFY_ADDITIONAL.find("SHORT")!=std::string::npos)   g_NotifyAdd=NOTIFY_ADD_SHORT;
			else if(stringOSG_NOTIFY_ADDITIONAL.find("FULL")!=std::string::npos)    g_NotifyAdd=NOTIFY_ADD_FULL;
			else std::cout << "Warning: invalid OSG_NOTIFY_ADDITIONAL set ("<<stringOSG_NOTIFY_ADDITIONAL<<")"<<std::endl;
		}
		// (end)

		s_NeedNotifyInit = false;

		return true;
	}

	IGN_CORE_API bool isNotifyEnabled(NotifySeverity severity) {
		if (s_NeedNotifyInit) ::ign::detail::initNotifyLevel();
		return severity<=g_NotifyLevel;
	}

	IGN_CORE_API std::ostream& notify(const NotifySeverity severity, const char * file, int line, const char * function) {
		if (s_NeedNotifyInit) initNotifyLevel();
		if (::ign::detail::isNotifyEnabled(severity))
		{
			g_NotifyStream->setCurrentSeverity(severity);
			*g_NotifyStream << getNotifyAdditionalInformationString(file, line, function);		//SKD: addition
			return *g_NotifyStream;
		}
		return *g_NullStream;
	}

}}	// namespace ign { namespace detail {

#endif	// ! OSG_FOUND


// --------------------------------------------------------------------------------

namespace ign {

	IGN_CORE_API void setNotifyAdditionalInformation(NotifyAdditionalInfo info) {
		if (detail::s_NeedNotifyInit) initNotifyLevel();
		detail::g_NotifyAdd = info;
	}

	IGN_CORE_API NotifyAdditionalInfo getNotifyAdditionalInformation() {
		if (detail::s_NeedNotifyInit) initNotifyLevel();
		return detail::g_NotifyAdd;
	}

	IGN_CORE_API std::string getNotifyAdditionalInformationString(const char * file, int line, const char * function) {
		std::ostringstream oss;
		if (detail::g_NotifyAdd==NOTIFY_ADD_SHORT) oss << function << ": ";
		else if (detail::g_NotifyAdd==NOTIFY_ADD_FULL) oss << "File " << file << " (line " << line << ") " << function << ": ";
		return oss.str();
	}

}	// namespace ign {
