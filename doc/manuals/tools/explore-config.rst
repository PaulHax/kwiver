==============
explore-config
==============

This program assists in debugging config loading problems. It loads a
configuration and displays the contents or displays the search
path.Additional paths can be specified in "KWIVER_CONFIG_PATH" environment
variableor on the command line with the -I or --path options.If -ds is
specified, the detailed search paths that apply to the application
aredisplayed only, otherwise the config file is loaded.

The option -dc only has effect when a config file is specified and causes
adetailed output of the config entries.

If -I or --path are specified, then the config file is only searched for
usingthe specified path. The application name based paths are not used.

kwiver explore-config       [options] config-file
------------------------------------------

  config-file  - name of configuration file.

**Options are:**

  ``-h, --help``
    Display applet usage information.

  ``-h, --ds``
    Display detailed application search path.

  ``-h, --dc``
    Display detailed config contents.

  ``-I, --path arg``
    Add directory to config search path.

  ``-a, --application arg``
    Application name.

  ``-v, --version arg``
    Application version.

  ``-p, --prefix arg``
    Non-standard installation prefix. (e.g. /opt/kitware)
