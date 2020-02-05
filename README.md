# Migrate units properties from Wialon Pro to Wialon Local
This console application is a converter, wich will help migrate units created in Wialon Pro to Wialon Local.

Tested on Wialon Pro 1401 and Wialon Local 1904. For creating wlp file is using [SDK API documentation](https://sdk.wialon.com/wiki/en/local/remoteapi1904/apiref/apiref).

## Features
Currently supports next unit properties for migrate:
* General
  * Name
  * Device type
  * Unique ID
  * Phone number
  * Counters
* Sensors
  * Name
  * Sensor type
  * Metrics
  * Parameter
  * Description
  * Validator
  * Validation type
  * Calculation table
* Custom Fields
* Trip Detection
  * Movement detector
  * Allow GPS correction
  * Min sattelites count
  * Min moving speed
  * Min parking timeout
  * Max distance between messages
  * Min trip timeout
  * Min trip distance
* Fuel Consumption
  * Fuel filling/thefts detection
    * Minimum fuel filling volume
    * Minimum fuel theft volume
    * Ignore the messages after the start of motion
    * Minimum stay timeout to detect fuel theft
    * Detect fuel filling only while stopped
    * Detect fuel theft in motion
    * Time-based calculation of filling
    * Time-based calculation of thefts
    * Calculate filling volume by raw data
    * Calculate theft volume by raw data
  * General sensors parameters
    * Merge same name sensors (fuel level)
	* Merge same name sensors (fuel consumption)
  * Consumption math
    * Enable/disable
	* Idling
	* Urban cicle
	* Suburban cicle
  * Consumption by rates
    * Enable/disable
	* Summer consumption
	* Winter consumption
	* Winter from
	* Winter to
  * Fuel level sensors
    * Enable/disable
	* Replace invalid values with math consumption
	* Time-based calculation of fuel consumption
	* Filter fuel level sensor values
	* Filter quality
  * Impulse fuel consumption sensors
    * Max impulses
	* Skip first zero values
  * Absolute fuel consumption sensors
  * Instant fuel consumption sensors

## Usage
Usage:

    wialon_units_migrate.exe [--help | -h] [--ipath=<path>] [--ifile=<filepath>] [--opath=<path>] [--overwrite]

Options:
* `--help` `-h` Prints the synopsis and a list of the commands
* `--ipath=<path>` Absolute path to input files (with extension .xml) directory for convertation
* `--ifile=<filepath>` Absolute path to single input file (with extension .xml) for convertation

  **Note:** Only one of `--ipath=<path>` or `--ifile=<filepath>` must be specified.
* `--opath=<path>` Absolute path to output file(s) (with extension .wlp) directory
* `--overwrite` Overwrite files in output directory

## Getting started
* Install [Visual Studio](https://visualstudio.microsoft.com/en/vs/)
* Get the code:
    ```
    git clone --recursive https://github.com/sledopyt8/wialon-units-migrate.git
    ```
* Open *wialon_units_migrate.sln* in Visual Studio to build and run the app

## Third party
For XML parse is using [pugixml library](https://pugixml.org) witch is licensed under the [MIT](https://opensource.org/licenses/MIT) License. Copyright (C) 2006-2019 Arseny Kapoulkine.

For create JSON is using [JSON for Modern C++ ](https://github.com/nlohmann/json) witch is licensed under the [MIT](https://opensource.org/licenses/MIT) License. Copyright (C) 2013-2019 Niels Lohmann.

## License
This project is available to anybody free of charge, under the terms of MIT License - see the [LICENSE](./LICENSE) file for details