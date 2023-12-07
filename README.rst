NEST Extension Module Example
=============================

This repository contains an example extension module (i.e a "plugin") for
the `NEST Simulator <https://nest-simulator.org>`_. Extension modules allow
users to extend the functionality of NEST without messing with the source
code of NEST itself, thus making pulls from upstream easy, while allowing
to extend NEST and sharing the extensions with other researchers.

In order to showcase the possibilites of extension modules and their use,
this extension module example contains the following (intentionally simple
and more or less silly) custom example components:

* A **neuron model** called ``pif_psc_alpha``, which implements a
  *non*-leaky integrate-and-fire model with alpha-function shaped
  post-synaptic potentials.
* A **synapse model** called ``drop_odd_spike_connection``, which drops
  all spikes that arrive at odd-numbered points on the simulation time
  grid and delivers only those arriving at even-numbered grid points.
* A **connection builder** called ``step_pattern_builder``, which
  creates step-pattern connectivity between the neurons of a source
  and a target population.
* A **recording backend** called ``RecordingBackendSocket``, which
  streams out the data from spike recorders to an external (or local)
  server via UDP.
* A **recording backend** called ``RecordingBackendSoundClick``, which
  creates the illusion of a realistic sound from an electrophysiological
  spike recording device.

For a list of modules developed by other users you can check out the
`list of forks <https://github.com/nest/nest-extension-module/network/members>`_
of this repository.

For developing custom neuron and synapse models, please consider using
`the NESTML modeling language <https://nestml.readthedocs.org/>`_.

.. attention::

   Please note that the code in this repository is compatible with the latest
   NEST version only; in particular the git "master" branch on
   https://github.com/nest/nest-simulator. For 2.x versions of NEST, see the
   extension module example in ``examples/MyModule`` of the source distribution.


Installation
------------

To build and install the extension module examples, first create a build
directory:

.. code-block:: sh

   mkdir build && cd build

Then compile the module:

.. code-block:: sh

   cmake \
    -Dwith-optimize=ON -Dwith-warning=ON \
    -Dwith-nest=$NEST_INSTALL_DIR/bin/nest-config 
    ..
   make
   make install

In this example ``$NEST_INSTALL_DIR`` refers to the directory that NEST
Simulator was installed to. Please ensure that the CMake flags
(``with-optimize``, ``with-mpi``, ``with-openmp`` and so on) are the same
as were used for the NEST Simulator build.

To use the new module in NEST Simulator, first source the ``nest_vars.sh``
script in the installation directory, and then use the ``nest.Install()``
API call to load the module:

.. code-block:: sh

   source $NEST_INSTALL_DIR/bin/nest_vars.sh
   python -c 'import nest; nest.Install("mymodule")'


Adapting ``MyModule``
---------------------

If you want to create your own custom extension module using MyModule
as a start, you have to perform the following steps:

1. Replace all occurences of the strings ``MyModule``, ``mymodule``
   and ``my`` by something more descriptive and appropriate for your
   module.
2. Remove the example functionality you do not need.
3. Adapt the example functionality you do need to your needs.
