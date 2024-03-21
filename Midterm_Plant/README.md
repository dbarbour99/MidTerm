# MidTerm_Plant


## Table of Contents
- [Introduction](#introduction)
- [Prerequisites To Use This Template](#prerequisites-to-use-this-repository)
- [Getting Started](#getting-started)
- [Particle Firmware At A Glance](#particle-firmware-at-a-glance)


## Introduction

This creates an automatic watering system (AWS), which monitors the moisture content of the surrounding soil.  
It it is deemed to be dry, the AWS, turns on a pump to water the plant.

Please visit hackster.io to see a more complete description of this project (https://www.hackster.io/david-barbour/keep-plant-alive-796db8)

## Prerequisites To Use This Repository

To use this software/firmware on a device, you'll need:

- Particle Photon 2(https://www.particle.io/devices/).
- Windows/Mac/Linux for building the software and flashing it to a device.
- [Particle Development Tools](https://docs.particle.io/getting-started/developer-tools/developer-tools/) installed and set up on your computer.
- SparkFun Atmospheric Sensor Breakout - BME280
- Seeed Studio Grove - Dust Sensor（PPD42NS）
- Seeed Studio Grove - Temperature Sensor
- Seeed Studio Grove - Capacitive Moisture Sensor (Corrosion Resistant)
- Seeed Studio Grove - OLED Display 1.12'' V2

## Getting Started

1. While not essential, we recommend running the [device setup process](https://setup.particle.io/) on your Particle device first. 
This ensures your device's firmware is up-to-date and you have a solid baseline to start from.

2. If you haven't already, open this project in Visual Studio Code (File -> Open Folder). 
Then [compile and flash](https://docs.particle.io/getting-started/developer-tools/workbench/#cloud-build-and-flash) your device. 
Ensure your device's USB port is connected to your computer.

3. Verify the device's operation by monitoring its logging output:
    - In Visual Studio Code with the Particle Plugin, open the [command palette](https://docs.particle.io/getting-started/developer-tools/workbench/#particle-commands) and choose "Particle: Serial Monitor".
    - Or, using the Particle CLI, execute:
    ```
    particle serial monitor --follow
    ```

4. Uncomment the code at the bottom of the cpp file in your src directory to publish to the Particle Cloud! 
Login to console.particle.io to view your devices events in real time.

5. Customize this project! For firmware details, see [Particle firmware](https://docs.particle.io/reference/device-os/api/introduction/getting-started/). For information on the project's directory structure, visit [this link](https://docs.particle.io/firmware/best-practices/firmware-template/#project-overview).

