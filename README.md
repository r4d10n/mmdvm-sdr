# mmdvm-sdr

----
[MMDVM firmware](https://github.com/g4klx/mmdvm) by Johnathan Taylor G4KLX - ported for x86 / arm  - Tested on x86 Linux / RasPi 3 / PlutoSDR (embedded). 

----

> Communicates with MMDVMHost using Virtual PTY (emulated serial port) in Linux. Requires single point modification in MMDVMHost code (disable RTS check).


> Currently offline Tx only. Rx possible with appropriate IO stub (see TODO). 

----

## Install:

Clone, compile mmdvm-sdr

    git clone https://github.com/r4d10n/mmdvm-sdr
    cd mmdvm-sdr
    mkdir build
    cd build

For native builds (x86): 

    cmake ..

For cross-compiling arm ([rpitools](https://github.com/raspberrypi/tools) to be installed): 

    cmake .. -DCMAKE_TOOLCHAIN_FILE=../Toolchain-rpi.cmake
    make

Running the modem binary:
 
    ./mmdvm 

It will display the PTY endpoint, which has to be specified in the MMDVHost/MMDVM.ini.

##### MMDVMHost RTS check disable modification:

1. Clone [MMDVMHost](https://github.com/g4klx/MMDVMHost)
2. Modify Modem.cpp around Line 120 from true to false [#L120](https://github.com/g4klx/MMDVMHost/blob/992b0f27ab5695a01fb43db69ed01ac2dcd47b5f/Modem.cpp#L120) 
3. make
4. Edit MMDVM.ini - Callsign, ID, Network - refer [MMDVM Setup](https://www.f5uii.net/en/installation-calibration-adjustment-tunning-mmdvm-mmdvmhost-raspberry-motorola-gm360/)
5. In [Modem] section, point Modem port to the Virtual PTY (modify as suited), also enable Debug:
```     
   Port=/home/pi/mmdvm-sdr/build/ttyMMDVM0
   Debug=1
```
6. Run MMDVMHost using the modified MMDVM.ini
```
./MMDVMHost MMDVM.ini
```

The pre-modulation audio samples are saved to disk in the mmdvm/build directory - **RXSamples.wav**. The samples can be modulated and transmitted using methods described in the *Use cases* section. Sample capture **RXSample-test.wav** can be used for TX testing.  

----
## Use cases:

* DSD monitor - Pipe audio to dsd (requires sox)
```
sox -r 24000 -t wav RXSamples.wav -r 48000 -t s16 - | dsd -i - -w DemodRXSamples.wav -v99
```
```
aplay DemodRXSamples.wav
```
* PiNBFm (based on LibRpiTx & PiFmRDS)

   Clone / install [PiNBFm](https://github.com/r4d10n/PiNBFm) [if  using [PiFMRDS](https://github.com/F5OEO/PiFMRDS/), modify DEVIATION in [pi_fm_rds.cpp] from 75000 to 6250]

```
pi_nbfm -freq 144.5 -audio RXSamples.wav -dev 6250 
```
```
pi_fm_rds -freq 144.5 -audio RXSamples.wav
```

* IQ output (tested with LeanIIOTx + PlutoSDR should be possible with USRP / LimeSDR / bladeRF / hackRF)

     > Process RXSamples.wav in GRC for upsampling to 1 MSps and FM modulation  (File Source -> Throttle -> Rational Resampler -> NBFM -> Filesink).

     > Generated IQ file can be played back in GRC using Plutosink or using LeanIIOTx on PlutoSDR

     > Install [LeanTRX](http://www.pabr.org/radio/leantrx/) on PlutoSDR 

     > Copy generated IQ file <RXSamples.iq> to PlutoSDR /tmp (max size: ~256M) using scp

```
leaniiotx -s 1000000.0 --bw 200000.0 -f 144500000.0 -v -d --bufsize 0x8000 --nbufs 32 < RXSamples.iq
```

* Osmo-fl2k (partial decode. adjust -c parameter for 144.5 MHz on spectrum analyzer / rtlsdr+gqrx )

```
sox -t wav RXSamples.wav -r 24000 -c 1 -b 16 -e signed-integer - | fl2k_fm - -f 32000 -s 100e6 -c 44.4781e6 -i 48000
```
----    
## Issues
* MMDVMHost - [RF queue overflow ](https://github.com/g4klx/MMDVMHost/issues/418)
* Fix SerialController in modem side.. CPU usage, gracious reinit….

----
## TODO:
* Interface **rtlfm** for handling RX 
* Seperate IO Stubs handling TX with PTT for PiNBFm / LeanIIOTx / etc.
* IO Stub for generic IQ input/output [resampling + FM mod/demod] 
* Merge with MMDVMHost [take out vpty/SerialController on both sides and directly invoke corresponding functions - optimize!]

----
## Disclaimer

Standard emission warnings apply. Use Filtering while transmitting using PWM/DAC modes.

This code is a hack. Feel free to break, fix, push and merge ! 

 

