// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "noggit/AsyncLoader.h"
#include "noggit/DBC.h"
#include "noggit/MPQ.h"
#include "noggit/MapView.h"
#include "noggit/Model.h"
#include "noggit/ModelManager.h" // ModelManager::report()
#include "noggit/TextureManager.h" // TextureManager::report()
#include "noggit/WMO.h" // WMOManager::report()
#include "noggit/errorHandling.h"
#include "noggit/liquid_layer.hpp"
#include "noggit/ui/main_window.hpp"
#include "opengl/context.hpp"
#include "util/exception_to_string.hpp"
#include "util/log.h"

#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QOffscreenSurface>
#include <QtOpenGL/QGLFormat>
#include <QtCore/QDir>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>

#include "revision.h"
#if USE_NETWORKING
#include "network/session.h"
#endif

class Noggit
{
public:
  Noggit (int argc, char *argv[]);

private:
  void initPath(char *argv[]);
  void loadMPQs();

  std::unique_ptr<noggit::ui::main_window> main_window;

  boost::filesystem::path wowpath;

  bool fullscreen;
  bool doAntiAliasing;
};

void Noggit::initPath(char *argv[])
{
  try
  {
    boost::filesystem::path startupPath(argv[0]);
    startupPath.remove_filename();

    if (startupPath.is_relative())
    {
      boost::filesystem::current_path(boost::filesystem::current_path() / startupPath);
    }
    else
    {
      boost::filesystem::current_path(startupPath);
    }
  }
  catch (const boost::filesystem::filesystem_error& ex)
  {
    LOG_ERROR("%s.", ex.what());
  }
}

void Noggit::loadMPQs()
{
  std::array<std::string, 14> archiveNames =
  {
    "common.MPQ",
    "common-2.MPQ",
    "expansion.MPQ",
    "lichking.MPQ",
    "patch.MPQ",
    "patch-{number}.MPQ",
    "patch-{character}.MPQ",

    // "{locale}/backup-{locale}.MPQ" ,
    // "{locale}/base-{locale}.MPQ" ,
    "{locale}/locale-{locale}.MPQ",
    // "{locale}/speech-{locale}.MPQ" ,
    "{locale}/expansion-locale-{locale}.MPQ",
    // "{locale}/expansion-speech-{locale}.MPQ" ,
    "{locale}/lichking-locale-{locale}.MPQ",
    // "{locale}/lichking-speech-{locale}.MPQ" ,
    "{locale}/patch-{locale}.MPQ",
    "{locale}/patch-{locale}-{number}.MPQ",
    "{locale}/patch-{locale}-{character}.MPQ",

    "development.MPQ"
  };

  std::array<const char*, 10> locales = { "enGB", "enUS", "deDE", "koKR", "frFR", "zhCN", "zhTW", "esES", "esMX", "ruRU" };
  const char * locale("****");

  // Find locale, take first one.
  for (size_t i(0); i < locales.size(); ++i)
  {
    if (boost::filesystem::exists (wowpath / "Data" / locales[i] / "realmlist.wtf"))
    {
      locale = locales[i];
      LOG_INFO("Locale: %s.", locale);
      break;
    }
  }
  if (!strcmp(locale, "****"))
  {
    LOG_FATAL("Could not find locale directory. Be sure, that there is one containing the file \"realmlist.wtf\".");
    //return -1;
  }


  //! \todo  This may be done faster. Maybe.
  for (size_t i(0); i < archiveNames.size(); ++i)
  {
    std::string path((wowpath / "Data" / archiveNames[i]).string());
    std::string::size_type location(std::string::npos);

    do
    {
      location = path.find("{locale}");
      if (location != std::string::npos)
      {
        path.replace(location, 8, locale);
      }
    } while (location != std::string::npos);

    if (path.find("{number}") != std::string::npos)
    {
      location = path.find("{number}");
      path.replace(location, 8, " ");
      for (char j = '2'; j <= '9'; j++)
      {
        path.replace(location, 1, std::string(&j, 1));
        if (boost::filesystem::exists(path))
          MPQArchive::loadMPQ (&AsyncLoader::instance(), path, true);
      }
    }
    else if (path.find("{character}") != std::string::npos)
    {
      location = path.find("{character}");
      path.replace(location, 11, " ");
      for (char c = 'a'; c <= 'z'; c++)
      {
        path.replace(location, 1, std::string(&c, 1));
        if (boost::filesystem::exists(path))
          MPQArchive::loadMPQ (&AsyncLoader::instance(), path, true);
      }
    }
    else
      if (boost::filesystem::exists(path))
        MPQArchive::loadMPQ (&AsyncLoader::instance(), path, true);
  }
}

namespace
{
  bool is_valid_game_path (const QDir& path)
  {
    if (!path.exists ())
    {
      LOG_ERROR("Path '%s' does not exist.", path.absolutePath().toStdString().c_str());
      return false;
    }

    QStringList locales;
    locales << "enGB" << "enUS" << "deDE" << "koKR" << "frFR"
      << "zhCN" << "zhTW" << "esES" << "esMX" << "ruRU";
    QString found_locale ("****");

    foreach (const QString& locale, locales)
    {
      if (path.exists (("Data/" + locale)))
      {
        found_locale = locale;
        break;
      }
    }

    if (found_locale == "****")
    {
      LOG_ERROR("Path '%s' does not contain a locale directory "
        "(invalid installation or no installation at all).", path.absolutePath().toStdString().c_str());
      return false;
    }

    return true;
  }
}

Noggit::Noggit(int argc, char *argv[])
  : fullscreen(false)
  , doAntiAliasing(true)
{
  assert (argc >= 1); (void) argc;
  initPath(argv);

  LOG_INFO("Noggit Studio - %s.", STRPRODUCTVER);

#if USE_NETWORKING
  sSession.StartSocket();
#endif

  QSettings settings;
  doAntiAliasing = settings.value("antialiasing", false).toBool();
  fullscreen = settings.value("fullscreen", false).toBool();

  srand(::time(nullptr));
  QDir path (settings.value ("project/game_path").toString());

  while (!is_valid_game_path (path))
  {
    QDir new_path (QFileDialog::getExistingDirectory (nullptr, "Open WoW Directory", "/", QFileDialog::ShowDirsOnly));
    if (new_path.absolutePath () == "")
    {
      LOG_INFO("Could not auto-detect game path and user canceled the dialog.");
      throw std::runtime_error ("no folder chosen");
    }
    std::swap (new_path, path);
  }

  wowpath = path.absolutePath().toStdString();
  std::string project_path = settings.value ("project/path", path.absolutePath()).toString().toStdString();

  LOG_INFO("Game path: '%s'.", wowpath.c_str());
  LOG_INFO("Project path: '%s'.", project_path.c_str());

  settings.setValue ("project/game_path", path.absolutePath());
  settings.setValue ("project/path", QString::fromStdString(project_path));

  loadMPQs(); // listfiles are not available straight away! They are async! Do not rely on anything at this point!
  OpenDBs();

  if (!QGLFormat::hasOpenGL())
  {
    throw std::runtime_error ("Your system does not support OpenGL. Sorry, this application can't run without it.");
  }

  QSurfaceFormat format;

  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);

  int vsync = settings.value ("vsync", 0).toInt();
  format.setSwapBehavior(vsync ? QSurfaceFormat::TripleBuffer : QSurfaceFormat::DoubleBuffer);
  format.setSwapInterval(vsync);

  if (doAntiAliasing)
  {
    format.setSamples (4);
  }

  QSurfaceFormat::setDefaultFormat (format);

  QOpenGLContext context;
  context.create();
  QOffscreenSurface surface;
  surface.create();
  context.makeCurrent (&surface);

  opengl::context::scoped_setter const _ (::gl, &context);

  LOG_DEBUG("GL: Version: %s.", gl.getString (GL_VERSION));
  LOG_DEBUG("GL: Vendor: %s.", gl.getString (GL_VENDOR));
  LOG_DEBUG("GL: Renderer: %s.", gl.getString (GL_RENDERER));

  main_window = std::make_unique<noggit::ui::main_window>();
  if (fullscreen)
  {
    main_window->showFullScreen();
  }
  else
  {
    main_window->showMaximized();
  }
}

namespace
{
  void noggit_terminate_handler()
  {
    std::string const reason
      {util::exception_to_string (std::current_exception())};

    if (qApp)
    {
      QMessageBox::critical ( nullptr
                            , "std::terminate"
                            , QString::fromStdString (reason)
                            , QMessageBox::Close
                            , QMessageBox::Close
                            );
    }

    LOG_FATAL("std::terminate: %s.", reason.c_str());
  }

  struct application_with_exception_printer_on_notify : QApplication
  {
    using QApplication::QApplication;

    virtual bool notify (QObject* object, QEvent* event) override
    {
      try
      {
        return QApplication::notify (object, event);
      }
      catch (...)
      {
        std::terminate();
      }
    }
  };
}
#if USE_NETWORKING
static std::thread sessionThread;
#endif

int main(int argc, char *argv[])
{
  noggit::RegisterErrorHandlers();
  std::set_terminate (noggit_terminate_handler);

  QApplication qapp (argc, argv);
  qapp.setApplicationName ("Noggit");
  qapp.setOrganizationName ("Noggit");

  Noggit app (argc, argv);

#if USE_NETWORKING
  sessionThread = std::thread([&](){ sSession.Update(); });
#endif

  return qapp.exec();
}
