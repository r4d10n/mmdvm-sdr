# mmdvm-sdr

----
[MMDVM firmware](https://github.com/g4klx/mmdvm) by Johnathan Taylor G4KLX - ported for x86 / arm  - Tested on x86 Linux / RasPi 3 / PlutoSDR (embedded) / Android 6 (arm - rooted - w/ termux) . 

----

> Communicates with MMDVMHost using Virtual PTY (emulated serial port) in Linux. Requires single point modification in MMDVMHost code (disable RTS check).


> Duplex/simplex DMR hotspot with a SDR device like the ADALM Pluto or LimeSDR, transmits both timeslots in duplex mode, but can receive only in DMO mode. Other digital modes can be added with minor effort.

----

## Install:

Requires an additional dependency, libzmq for GNU radio integration. Install libzmq and libzmq-dev. On Debian Bullseye:

    apt-get install libzmq3-dev libzmq5

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
5. In [Modem] section, point Modem port to the Virtual PTY (modify as suited), also enable RX invert and TX invery and Debug:
```     
    Port=/home/pi/mmdvm-sdr/build/ttyMMDVM0
    TXInvert=1
    RXInvert=1
    PTTInvert=0
    TXDelay=0
    RXOffset=0
    TXOffset=0
    DMRDelay=0
    RXLevel=100
    TXLevel=100
    RXDCOffset=0
    TXDCOffset=0
    RFLevel=100
    RSSIMappingFile=RSSI.dat
    UseCOSAsLockout=0
    Trace=0
    Debug=1
```
6. Run MMDVMHost using the modified MMDVM.ini
```
./MMDVMHost MMDVM.ini
```

The RRC filtered pre-FM modulation audio samples are sent to GNU radio via ZeroMQ, TCP ports 5990 and 5991 at a rate of 24000 samples per second baseband.

----
## Use cases:

* Duplex/simplex DMR hotspot with a SDR device like the ADALM Pluto or LimeSDR, transmits both timeslots in duplex mode, but can receive only in DMO mode, which means any MS transmission will be picked up by MMDVM and sent to the net on timeslot 2. QRadioLink can then set up the duplex frequency pairs and bridge to the SDR device. GNU radio flowgraphs can be used as well with ZeroMQ flowgraphs. The frequency defined in MMDVM.ini are not necessarily the same as the SDR frequencies, it is up to you to set up the SDR correctly.

    See https://github.com/qradiolink/qradiolink/tree/mmdvm_integration for the QRadioLink integration code.

* GNU Radio Flowgraph for Duplex DMR Hotspot using PlutoSDR

    [gr-mmdvm](https://github.com/r4d10n/mmdvm-sdr/tree/master/gr-mmdvm) directory contains example GNU Radio Flowgraph based on [kantooon](https://github.com/qradiolink/qradiolink)'s ZMQ implementation for setting up realtime Duplex DMR hotspot using PlutoSDR (other duplex SDRs should be possible too). Also includes [gr-dsd](https://github.com/argilo/gr-dsd) based decoder for decoding network traffic from MMDVMHost networks. 
    
    ![gr-mmdvm-flowgraph](https://imgur.com/bUChUFn.jpg)
    
    ![gr-mmdvm-ui](https://imgur.com/p1w0ods.jpg)

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
## TODO:
* Interface with SoapySDR for handling RX/TX
* IO Stub for generic IQ input/output [resampling + FM mod/demod]

----
## Disclaimer

Standard emission warnings apply. Use Filtering while transmitting using PWM/DAC modes.

This code is a hack. Feel free to break, fix, push and merge ! 


----
## Licensing

ZeroMQ and GNU radio integration code written by Adrian Musceac YO8RZZ, redistribute with same license (GPLv3) as
the original MMDVM code by Jonathan Naylor G4KLX. 

All copyrights on MMDVM and mmdvm-sdr by original authors apply.


 

