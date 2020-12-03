# hp-controller-RPC-56P
Get data from heat pump controller via RS-485

First stage was Delphi program for PC working with RS-485 to USB converter. It worked well, having display chart, save & load XML capabilities.
Since this kind of aquisition was problematic from logistics point of view - it was dropped.

Second stage is esp32 microcontroller based approach. Aim was to store atleast 48h of data.

Third stage will be data upload to some external (IoT) server.
