import logging
import os
import sys
from pathlib import Path


PYTHON_PLUGIN_ENTRYPOINT = "kwiver.python_plugin_registration"
CPP_SEARCH_PATHS_ENTRYPOINT = "kwiver.cpp_search_paths"


_LOGGING_ENVIRON_VAR = "KWIVER_PYTHON_DEFAULT_LOG_LEVEL"


def _logging_onetime_init() -> None:
    """
    One-time initialize kwiver-module-scope logging level.

    This will default to the WARNING level unless the environment variable
    "KWIVER_PYTHON_DEFAULT_LOG_LEVEL" is set to a valid case-insensitive value
    (see the map below).

    This does NOT create a logging formatter. This is left to the discretion of
    the application.
    """
    if not hasattr(_logging_onetime_init, "called"):
        # Pull logging from environment variable if set
        llevel = logging.WARN
        if _LOGGING_ENVIRON_VAR in os.environ:
            llevel_str = os.environ[_LOGGING_ENVIRON_VAR].lower()
            # error warn info debug trace
            m = {
                "error": logging.ERROR,
                "warn": logging.WARN,
                "info": logging.INFO,
                "debug": logging.DEBUG,
                "trace": 1,
            }
            if llevel_str in m:
                llevel = m[llevel_str]
            else:
                logging.getLogger("kwiver").warning(
                    f"KWIVER python logging level value set but did not match "
                    f'a valid value. Was given: "{llevel_str}". '
                    f"Must be one of: {list(m.keys())}. Defaulting to warning "
                    f"level."
                )
        logging.getLogger(__name__).setLevel(llevel)
        # Mark this one-time logic as invoked to mater calls are idempotent.
        _logging_onetime_init.called = True
    else:
        logging.getLogger(__name__).debug(
            "Logging one-time setup already called, doing nothing."
        )


def _add_library_paths() -> None:
    # For Python >=3.8  we need to explicitly add paths where dll will be imported from.
    if sys.version_info >= (3, 8) and sys.platform == "win32":
        paths = [
            # in the build directory __file__ is at Lib\site_packages\kwiver and kwiver dlls in toplevel bin
            Path(__file__).parents[3].absolute() / "bin",
            # in a a wheel __file__ is at Lib\site_packages\kwiver and kwiver dlls in  the same level bin
            Path(__file__).parents[0].absolute() / "bin",
        ]
        for path in paths:
            if path.exists():
                os.add_dll_directory(str(path))
                logging.getLogger(__name__).debug(f"Adding {path} to dll search paths")
        # xxx(python310) it look like this is not need for python310
        if sys.version_info[:2] == (
            3,
            8,
        ):
            path = (Path(__file__).parents[1] / "kwiver.libs").resolve()
            if path.exists():
                os.environ["PATH"] = f"{str(path)}{os.pathsep}{os.environ['PATH']}"
                logging.getLogger(__name__).debug(f"Adding {path} to PATH")


_logging_onetime_init()
_add_library_paths()
