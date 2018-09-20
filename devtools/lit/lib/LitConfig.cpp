// This source file is part of the polarphp.org open source project
//
// Copyright (c) 2017 - 2018 polarphp software foundation
// Copyright (c) 2017 - 2018 zzu_softboy <zzu_softboy@163.com>
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://polarphp.org/LICENSE.txt for license information
// See http://polarphp.org/CONTRIBUTORS.txt for the list of polarphp project authors
//
// Created by polarboy on 2018/08/30.

#include "LitConfig.h"
#include "TestingConfig.h"
#include "Utils.h"
#include <cassert>
#include <iostream>

namespace polar {
namespace lit {

LitConfig::LitConfig(const std::string &progName, const std::list<std::string> &path,
                     bool quiet, bool useValgrind, bool valgrindLeakCheck,
                     const std::list<std::string> &valgrindArgs, bool noExecute, bool debug,
                     bool singleProcess, bool isWindows, const std::map<std::string, std::any> &params,
                     const std::optional<std::string> &configPrefix, int maxIndividualTestTime,
                     const std::optional<int> &maxFailures, const std::map<std::string, std::string> &parallelismGroups,
                     bool echoAllCommands)
   : m_progName(progName), m_path(path),
     m_quiet(quiet), m_useValgrind(useValgrind),
     m_valgrindLeakCheck(valgrindLeakCheck),
     m_valgrindUserArgs(valgrindArgs), m_noExecute(noExecute),
     m_debug(debug), m_singleProcess(singleProcess),
     m_isWindows(isWindows), m_params(params), m_bashPath(std::nullopt),
     m_configPrefix(configPrefix.has_value() ? configPrefix.value() : "lit"),
     m_suffixes({"cfg.cmake"}), m_maxFailures(maxFailures),
     m_parallelismGroups(parallelismGroups), m_echoAllCommands(echoAllCommands)
{
   setMaxIndividualTestTime(maxIndividualTestTime);
   for (const std::string &suffix : m_suffixes) {
      m_configNames.push_back(m_configPrefix + "." + suffix);
      m_siteConfigNames.push_back(m_configPrefix + ".site." + suffix);
      m_localConfigNames.push_back(m_configPrefix + ".local." + suffix);
   }
   if (useValgrind) {
      m_valgrindArgs.push_back("valgrind");
      m_valgrindArgs.push_back("-q");
      m_valgrindArgs.push_back("--run-libc-freeres=no");
      m_valgrindArgs.push_back("--tool=memcheck");
      m_valgrindArgs.push_back("--trace-children=yes");
      m_valgrindArgs.push_back("--error-exitcode=123");
      if (m_valgrindLeakCheck) {
         m_valgrindArgs.push_back("--leak-check=full");
      } else {
         m_valgrindArgs.push_back("--leak-check=no");
      }
      for (const std::string &userArg : m_valgrindUserArgs) {
         m_valgrindArgs.push_back(userArg);
      }
   }
}

LitConfig &LitConfig::setMaxIndividualTestTime(int value)
{
   assert(value >= 0);
   m_maxIndividualTestTime = value;
}

void LitConfig::writeMessage(const std::string &kind, const std::string &message,
                             const std::string &file, const std::string &line) const
{
   std::cerr << m_progName << ": " << file << ":" << line << " :" << kind << " :" << message
             << std::endl;
}

TestingConfigPointer LitConfig::loadConfig(TestingConfigPointer config, const std::string &path)
{
   if (m_debug) {
      note(std::string("loadConfig from ") + path);
   }
   config->loadFromPath(path, *this);
   return config;
}

std::string LitConfig::getBashPath()
{
   if (m_bashPath.has_value()){
      return m_bashPath.value();
   }
   m_bashPath = which("bash", join_string_list(m_path, ":"));
   if (!m_bashPath.has_value()) {
      m_bashPath = which("bash");
   }
   if (!m_bashPath.has_value()) {
      m_bashPath = "";
   }
   return m_bashPath.value();
}

std::optional<std::string> LitConfig::getToolsPath(std::optional<std::string> dir, const std::string &paths,
                                                   const std::list<std::string> &tools)
{
   if (dir.has_value() && fs::path(dir.value()).is_absolute() && fs::is_directory(dir.value())) {
      if (!check_tools_path(dir.value(), tools)) {
         return std::nullopt;
      }
   } else {
      dir = which_tools(tools, paths);
   }
   m_bashPath = which("bash", dir);
   if (!m_bashPath.has_value()) {
      m_bashPath = "";
   }
   return dir;
}

const std::string &LitConfig::getProgName() const
{
   return m_progName;
}

const std::list<std::string> &LitConfig::getPaths() const
{
   return m_path;
}

bool LitConfig::isQuiet() const
{
   return m_quiet;
}

bool LitConfig::isUseValgrind() const
{
   return m_useValgrind;
}

bool LitConfig::isValgrindLeakCheck() const
{
   return m_valgrindLeakCheck;
}

const std::list<std::string> &LitConfig::getValgrindUserArgs() const
{
   return m_valgrindUserArgs;
}

bool LitConfig::isNoExecute() const
{
   return m_noExecute;
}

bool LitConfig::isDebug() const
{
   return m_debug;
}

bool LitConfig::isSingleProcess() const
{
   return m_singleProcess;
}

bool LitConfig::isWindows() const
{
   return m_isWindows;
}

const std::map<std::string, std::any> &LitConfig::getParams() const
{
   return m_params;
}

const std::optional<std::string> &LitConfig::getBashPath() const
{
   return m_bashPath;
}

const std::string &LitConfig::getConfigPrefix() const
{
   return m_configPrefix;
}

const std::list<std::string> &LitConfig::getSuffixes() const
{
   return m_suffixes;
}

const std::list<std::string> &LitConfig::getConfigNames() const
{
   return m_configNames;
}

const std::list<std::string> &LitConfig::getSiteConfigNames() const
{
   return m_siteConfigNames;
}
const std::list<std::string> &LitConfig::getLocalConfigNames() const
{
   return m_localConfigNames;
}

int LitConfig::getNumErrors() const
{
   return m_numErrors;
}

int LitConfig::getNumWarnings() const
{
   return m_numWarnings;
}

const std::list<std::string> &LitConfig::getValgrindArgs() const
{
   return m_valgrindArgs;
}

int LitConfig::getMaxIndividualTestTime() const
{
   return m_maxIndividualTestTime;
}

const std::optional<int> &LitConfig::getMaxFailures() const
{
   return m_maxFailures;
}

const std::map<std::string, std::string> &LitConfig::getParallelismGroups() const
{
   return m_parallelismGroups;
}

bool LitConfig::isEchoAllCommands() const
{
   return m_echoAllCommands;
}

} // lit
} // polar
