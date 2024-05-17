# Exporting Plugin Implementations
Python version 3.8 is required to export plugins.
Add to your ``setup.py`` entrypoints section a "kwiver_plugins" namespace.
This namespace is defined in the ``kwiver.vital.plugins.constants`` module under
the ``PLUGIN_NAMESPACE`` attribute.

# Building
The required python packages areincluded in the source code.
The runtime requirements are defined in the
``kwiver/src/python/requirements_dev.txt`` file.

Using the appropriate [virtual] environment with python activated, the python
packages can be installed with::

  $ pip install -r <kwiver-source-dir>/python/requirements_dev.txt

We assume CMake is available on the system.
Otherwise, CMake is also installable via pip.
It will be installed via the above requirements file.

PyBind11 is used extensively here to facilitate the binding interface.
PyBind11 version 2.10.3 is currently being utilize

```bash
python setup.py bdist_wheel -- -C $GIT_CLONE_PATH/.gitlab/ci/configure_wheel.cmake
LD_LIBRARY_PATH=/opt/fletch/lib/ auditwheel show dist/*.whl
LD_LIBRARY_PATH=/opt/fletch/lib/ auditwheel repair dist/*.whl
```

This should create a `dist/` directory here after successful completion.
In that directory will be the wheel file for the python version built against.

# What Scikit-Build Does
Scikit-build will perform a "normal build," however "installed" items will be
placed such that they are included under the package's root module.
For example, if we install the file `<prefix>/lib/foo.so`, then the package
built by Scikit-build will include `<site-packages>/kwiver/lib/foo.so`.

Note that certain build and installation locations are different for
scikit-build runs vs. "normal" build runs.
This is done in order to facilitate proper library and module library build and
installation locations for python packages.
This is managed by the KWIVER CMake functions and should not need additional
management by binding developers.

Example: `kwiver_add_python_library` may result in, for a "normal" build, a
library placed into
`<build_dir>/lib/python3.6/site-packages/kwiver/vital/config/_config.so`
however for a scikit-build run, the same library would be placed in
`<source_dir>/_skbuild/.../cmake-build/vital/config/_config.so/`.
