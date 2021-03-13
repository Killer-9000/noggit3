// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "log.h"

using namespace Logging;

Logger::Logger()
{
  stopThread = false;
  m_LoggingThread = std::thread(&Logger::LogAvailibleMessages, this);
}

Logger::~Logger()
{
  // Need to cleanup properly, so we make sure we got all messages,
  //  and so the queue and destroy without complaining.
  stopThread = true;
  m_LoggingThread.join();

  std::string message;
  while (m_Messages.try_pull(message) == boost::concurrent::queue_op_status::success)
  {
// #if DEBUG__LOGGINGTOCONSOLE
    printf("%s", message.c_str());
// #else
    if (m_File)
      fprintf(m_File, "%s", message.c_str());
// #endif
  }
  m_Messages.close();
  fclose(m_File);
}

void Logger::LogAvailibleMessages()
{
// #if !DEBUG__LOGGINGTOCONSOLE
  m_File = fopen(m_LogFile.c_str(), "w+");
  if (m_File == nullptr)
    printf("Failed to open log file.\n");
// #endif
  
  std::string message;
  while(!stopThread)
  {
    if (m_Messages.try_pull(message) == boost::concurrent::queue_op_status::success)
    {
// #if DEBUG__LOGGINGTOCONSOLE
      printf("%s", message.c_str());
// #else
      if (m_File)
        fprintf(m_File, "%s", message.c_str());
// #endif
    }
  }
}

std::string Logger::GetSeverityString(LogSeverity severity)
{
	switch(severity)
	{
	case LOGSEVERITY_DEBUG:
		return "[Debug] ";
	case LOGSEVERITY_INFO:
		return "";
	case LOGSEVERITY_WARN:
		return "[Warn] ";
	case LOGSEVERITY_ERROR:
		return "[Error] ";
	case LOGSEVERITY_FATAL:
		return "[Fatal] ";
	default:
		return "[Unk] ";
	}
}
