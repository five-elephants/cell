#pragma once

#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>


static void init_logging() {
  using namespace log4cxx;

  LayoutPtr layout(new PatternLayout("%-5p %d{yyyy-MM-dd HH:mm:ss,SSS}:  %m%n"));
  AppenderPtr appender(new ConsoleAppender(layout));
  auto logger = Logger::getRootLogger();
  logger->addAppender(appender);
  logger->setLevel(Level::getInfo());
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
