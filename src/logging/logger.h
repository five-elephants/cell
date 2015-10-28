#pragma once

#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>


static void init_logging() {
  using namespace log4cxx;

  LayoutPtr layout(new PatternLayout("%-5p %r [%c]:  %m%n"));
  AppenderPtr appender(new ConsoleAppender(layout));
  auto logger = Logger::getRootLogger();
  logger->addAppender(appender);
  logger->setLevel(Level::getInfo());
  //logger->setLevel(Level::getDebug());
  //logger->setLevel(Level::getTrace());
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
