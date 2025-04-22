# hp-controller-RPC-56P

This can be used for Timel's heat pump controllers: RPC-56P & RPC-100 readout. RPC-56P was, and RPC-100 is used by Eko Dom heat pumps polish producer.

This software only listens data from heat pump controller via RS-485. It does not write anything to the controller.

First stage was Delphi program for PC working with RS-485 to USB converter. It worked well, having display chart, save & load XML capabilities.
Since this kind of aquisition was problematic from logistics point of view - it was dropped.

Second stage is esp32 microcontroller based approach. Aim was to store atleast 48h of data.

Third stage was esp8266 (esp12) port

Last stage is translate all this thing to ESPHome (and Home Assistant)

Since Home Assistant and ESPHome decided to drop support for custom components in favour of external components - new repo was created to fullfill compatibility with 2025.03 Home Assistant / ESPHome versions.

Please treat this repo for ESPHome as deprecated and use this new repo: https://github.com/elproko/esphome_rpc56p
