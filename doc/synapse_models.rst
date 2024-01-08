Writing synapse models
======================

.. note::

   For developing custom neuron and synapse models, please consider using `the NESTML modeling language <https://nestml.readthedocs.org/>`_.

NEST has a very flexible system to allow users to write their own synapse types. Synapses in NEST can either have static parameters or apply some dynamics on them. Each connection needs to have at least the following parameters:

* The connection delay
* The connection weight
* The target node of the connection
* The receiver port, which identifies the connection on the postsynaptic side

The source node of a connection is implicitly stored by the position in the data structure that is used internally. These parameters are implemented in the StaticConnection synapse type, which can be used as a base class for more advanced synapse types.  


Synapses in NEST
----------------

This section gives a brief overview over how synapses are currently implemented in NEST. Synapses in NEST come in two parts:

1. a Connection object that manages synaptic weight and delay, and

2. code implementing the synaptic currents or conductances resulting from a spike arrival; the latter is always coded in the model neuron class.

Both parts will be discussed in turn. Some of the discussion below is simplified, ignoring issues of parallelization; the intent is to give you the general idea.


Storage of synapses
^^^^^^^^^^^^^^^^^^^

All targets of a given neuron are stored in a target list containing one ``Connection`` object per target. When a spike is sent, NEST traverses the target list and sends information about the spike, that is, weight, delay and spike time to each receiving neuron.

Some synapses implement spike-time dependent plasticity, that is, they modify the synaptic weight stored in the ``Connection`` object. To compute the weight update, these synapses typically access the spike history through special member functions of the receiving neuron class, which must be derived from ``ArchivingNode`` (most models are).

The details of connection storage infrastructure are explained in [1]_.


Event delivery
^^^^^^^^^^^^^^

When a spike arrives at a neuron by a call to its ``handle(SpikeEvent&)`` method, the ``SpikeEvent`` provides information about the synaptic weight, the spike time, and the delay, so that the actual arrival time of the spike can be calculated.

It is entirely up to the neuron class to handle this information. In particular, all dynamics of synaptic currents or conductances must be implemented in the neuron model.


Using different receptor ports
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Introducing different receptor channels (NMDA, AMPA, GABA, etc.) raises the question of how to differentiate input to different types of channels on a neuron.

Most NEST models are very simplistic in this respect: they differentiate mainly between an excitatory and an inhibitory synapse. 
All input with positive weight is handled by the excitatory synapse, and all with input with negative weight is handled by the inhibitory synapse.

More channels can be handled using the receptor type/receiver port mechanism in NEST. Please see the ``iaf_psc_alpha_multisynapse`` NEST model for an example.


Temporal aspects
^^^^^^^^^^^^^^^^

Time is a very important issue when processing spikes. Especially when the effect of a spike on the neuron depends on the history or state of the neuron, it is crucial that one considers causality carefully.

This is the reason why spiking history information about a neuron is available only via the ``ArchivingNode`` interface. The paper [2]_ has more on spike history handling.


Spike handling
^^^^^^^^^^^^^^

NEST delivers spikes in batches. Simulation proceeds for ``min_delay`` (minimum delay) time. During this time, any generated spikes are stored in a central spike queue. After each ``min_delay`` interval, spikes are delivered from the central queue via the ``ConnectionManager`` and the Connection objects to the individual neurons.

All delays are handled inside the neuron, as described above. This means that when a spike "passes through" the ``Connection`` object, the actual biological arrival time (time when spike occurred at sender plus delay) of the spike may be up to ``max_delay`` (maximum delay) time units in the future. This means, in particular, that ``Connection`` objects cannot perform any computations that depend on the state of the neuron at the time of "biological" spike arrival; they can only use historic information.

Another important point is that spikes do NOT pass the Connection object in correct order of biological arrival time---they are unordered in time.


Writing synapse models
----------------------

Writing a synapse type is basically very simple. You can directly
derive your new connection type from StaticConnection, which provides
all mechanisms to register and send a connection. A synapse type
consist of two files, a header and an implementation. Skeletons for
both of them are shown shown in the following listings:

.. code-block:: C++

   #ifndef MY_SYNAPSE
   #define MY_SYNAPSE

   #include "static_synapse.h"
   #include "generic_connector.h"

   namespace nest
   {
     class my_synapse : public static_synapse
     {
       public:
         my_synapse () {}
         my_synapse (const my_synapse &) {}
         ~my_synapse () {}

         update_dynamics ();
         void send (Event & e, double_t t_lastspike, const CommonSynapseProperties & cp);
     };

     inline void my_synapse::send (Event & e, double_t t_lastspike, const CommonSynapseProperties &)
     {
       update_dynamics();

       e.set_receiver(*target_);
       e.set_weight(weight_);
       e.set_delay(delay_);
       e.set_rport(rport_);
       e();
     }
   } // namespace nest

   #endif /* #ifndef MY_SYNAPSE */

The first thing we do is include the header files of our base class,
StaticConnection. It already defines funtions for registering the
connection with the ConnectionManager of NEST, for storing the
mandatory parameters weight and delay and functions to set and
retrieve these parameters from within the SLI interpreter.

.. code-block:: C++

   #include "my_synapse.h"

   void nest::my_synapse::update_dynamics ()
   {
     /* Do fancy stuff with weights here! */
   }

To apply some (activity dependent) dynamics on the weight of the
connection you simply have to override the method ``send()``. It is the
one that is called each time an event flows over the
connection. Except for the call to ``update_dynamics()`` in which the
synaptic weight is calculated, the function ``MyConnection::send()`` is a
copy of the implementation from StaticConnection. It fills in the rest
of the parameters of the event and sends the event to the target.

Registering the new synapse type
--------------------------------

After your files are written, you have to add their names to the
``CMakeLists.txt`` file in ``src/`` to have it be compiled
and linked to NEST.

To make the synapse type available inside of NEST scripts, you have to
include and register it with the module. Add the following line to the
beginning of ``src/mymodule.cpp``:

.. code-block:: C++

   #include "my_synapse.h"

And the following line to the ``init()`` method of the module:

.. code-block:: C++

   register_connection_model< my_synapse >( "my_synapse" );

References
----------

.. [1] Kunkel et al. (2014), Spiking network simulation code for petascale computers. Front. Neuroinform. 8:78. `doi:10.3389/fninf.2014.00078 <http://dx.doi.org/10.3389/fninf.2014.00078>`_.

.. [2] `Morrison et al. (2007) <http://dx.doi.org/10.1162/neco.2007.19.6.1437>`_

