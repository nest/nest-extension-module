Writing an extension module
===========================

.. note::

   The example extension module is hosted on its own repository, at:
   https://github.com/nest/nest-extension-module/

NEST has a modular architecture which allows you to add your own neuron and synapse models without any need to modify the NEST source code itself, but by just adding a new module. You can then either load this module dynamically at runtime (preferred) or you can link NEST against your module.

By writing a new module, you can add

* your own neuron models
* your own synapse types
* your own connection (or other) functions

to NEST. For the benefit of the NEST Community at large, we would encourage you to share your modules with other NEST users. Please see the `contributing <https://nest-simulator.readthedocs.io/en/stable/developer_space/index.html>`_ page to find out how to initiate the inclusion by issuing a pull request.

On this page, you will find an overview of how to create your own module, based on the example ``MyModule``, which you find at https://github.com/nest/nest-extension-module/.

If you have questions, problems, or feedback about your experience with external modules, please join the `mailing list <https://nest-simulator.readthedocs.io/en/stable/community.html>`_ to share it with us ·and other users.

.. note::

   For developing custom neuron and synapse models, please consider using `the NESTML modeling language <https://nestml.readthedocs.org/>`_.


Prerequisites
-------------

1. Download, build, and install NEST. NEST should be built outside the source code directory.
2. The NEST source code and installation directory must be accessible for building modules.
3. Define the environment variable ``NEST_INSTALL_DIR`` to contain the path to which you have installed NEST, e.g., using bash,

   .. code-block:: sh

      export NEST_INSTALL_DIR=/Users/plesser/NEST/install

   This environment variable is not strictly necessary, but saves you typing later.


Building MyModule
-----------------

1. Create a build directory:

   .. code-block:: sh

      mkdir build
      cd build

3. Configure. The configure process uses the script ``nest-config`` to find out where NEST is installed, where the source code resides, and which compiler options were used for compiling NEST. If ``nest-config`` is not in your path, you need to provide it explicitly like this

   .. code-block:: sh

      cmake -Dwith-nest=${NEST_INSTALL_DIR}/bin/nest-config ..

   Please ensure that any other custom CMake flags (such as ``with-optimize``, ``with-mpi``, ``with-openmp`` and so on) are the same as were used for the NEST Simulator build.

   It is not recommended to use ``-DCMAKE_INSTALL_PREFIX`` to select a different installation destination. If you do, you must make sure to use environment variables like ``LD_LIBRARY_PATH`` to ensure NEST can locate the module.

4. Compile and install:

   .. code-block:: sh

      make
      make install

   MyModule will then be installed to ``${NEST_INSTALL_DIR}``.


Using MyModule
--------------

To use the new module in NEST Simulator, first source the ``nest_vars.sh`` script in the installation directory, and then use the ``nest.Install()`` API call to load the module:

.. code-block:: sh

   source $NEST_INSTALL_DIR/bin/nest_vars.sh
   python -c 'import nest; nest.Install("mymodule")'

After loading the module, you should be able to see ``pif_psc_alpha`` in ``nest.node_models`` and ``drop_odd_spike`` in ``nest.synapse_models``.


Creating your own module
------------------------

1. Start with the code from MyModule.
2. Replace anything called ``mymodule`` in any form of camelcasing by the name of your module, and proceed as above.
3. When you change names of source code files or add/remove files, you need to update the variable ``MODULE_SOURCES`` in ``CMakeLists.txt``.
4. ``make dist`` will roll a tarball of your module for distribution to others.


Linking MyModule into NEST
--------------------------

1. Build NEST and MyModule as described above.
2. Change back to the NEST build directory.
3. Reconfigure NEST informing it about your MyModule. Note that the module MUST be installed in the NEST installation directory tree!

   .. code-block:: sh

      cmake [...] -Dexternal-modules=my ../src

   Several modules can be given, separated by semicolon.

   .. note::

      Instead of giving the full module name ``mymodule``, only give the ``SHORT_NAME`` ``my`` for the option ``-Dexternal-modules=...``.

4. Recompile and install NEST.
5. The module should now be available as soon as NEST has started up. It will also be available in PyNEST.
6. When you make any change to your module, you must first re-compile and re-install your module.
7. Then move to the NEST build directory and issue

   .. code-block:: sh

      make -C nest clean
      make
      make install

   This rebuilds only the NEST executable.
