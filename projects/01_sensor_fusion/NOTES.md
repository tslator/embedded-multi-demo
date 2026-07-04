**02JUL26** - I want to work top down initially.  I think there is an advantage in developing the application first with stubbed layers.  Also, I don't yet have my IMU hardware or encoder infrastructure in place yet.  While Phase 2 of the plan proposes a folder structure, I'm not inclined to create a bunch of empty folders.  The plan is a reference, not a dictate.

I experimented with creating a sample application using the Pico extension, but it is not very flexible in that is expects you to use it for creation.  I don't want to be beholden to a tool, so I will use the blinky project as an example of how to setup a project.

**Sensor Fusion Project Setup**
- For the most part, it was a simple copy and paste from the blinky project.  The `main.c` resides in the project root and the cmake lists file associates `sensor_fusion` as the project name and executable with `main.c`

Great.  Now we're ready to establish a separate application that will live in the `app` folder.  The application will represent the top-most perspective and business logic.  From the `main.c` perspective, the application presents an interface that allows it to be initialized, configured, executed and potentially terminated due to exceptional conditions.  From experience, I'm thinking:

```c
app_init();
app_config();
app_run();
app_stop();     // for testing purposes
app_shutdown(); // for testing and exception conditions
```

**Application Interface**
Established an application interface an integrated the led toggle into application run.

This is a good initial skeleton but it binds directly to the pico and doesn't match the phase 2 plan. I want to establish the platform and driver hierachy using the LED as the prototype.  Also, I can address phase 3 serial console and debugging tasks.

Refined the organization of platform and rp2040 implementations.  There is still room for improvement because now we aren't selecting the platform based on a build configuration and the naming is still awkward.  Serial debugging is working and abstracted.

**Multi-Core Setup**
This is the first time I have worked with multiple cores so I'm consulting with LLMs to educate myself.

- Pico boots to Core 0
- Core 0 must explicitly start Core 1 using `multicore_launch_core1` and a function to call.
- SRAM and peripherals are shared so concurrency hygiene is required.
- hardware spinlocks and inter-core FIFO supported

Phase 3 plan is to share a count on Core 0 with Core 1 and write to serial output.

Success!

- had to add pico_multicore library
- added heartbeat function to app, used platform abstraction to start, push and pop
- extended platform to wrap start, push and pip functions.

**Platform Renaming**
The file names for the platform implementation seem not particularly consistent especially considering the idea of including an implementation on another multicore MCU like ESP32.

Decide on the following:

platform/<target>/core.c
platform/<target>/delay.c
platform/<target>/led.c
platform/<target>/serial.c

Updated the rp2040 files. 

**Driver Interfaces**
For phase 4, I need to create the driver interfaces for each of the sensors: IMU, encoder, and ADC.  From the application perspective, via a services that retrieves the latest samples, the interface to each driver is the same:

```c
int initialize(void);
int start(void);
void stop(void);
int read(void *out, size_t len);
```

I'm not a fan of `void` types, but this is C and I'm not entirely sure of the details yet.  Once I know, I might prefer a `read_as_xxx` or `to_xxx` where I can add typing and hide the details in code.

Preliminary drivers in place.

**03JUL26** - Today I want to flesh out a complete driver, I'm choosing ADC -- it's the simplest, its types and data members, and enumerations for return times.

**ADC Driver Detail**
First on the list, is to address return types.  Raw integer return types are no fun, so I'm defining enumerations.  Also, fixing but the `const` correctness.  I'm a `east-const`.

```c
enum {
    ADC_OK,
    ADC_ERR_NOT_INIT,
    ADC_ERR_NOT_STARTED,
    ADC_ERR_BAD_ARG,
    ADC_ERR_PLATFORM
} adc_status_t;
```

For the configuration, we need a channel and a voltage reference. Although for the purpose of this project, there is only on ADC, allowing the certain fixed values to be pass facilitates testing.  For conveniences, I'm adding an over sample parameter to allow more suffisticated sample beyond the hardware capabilities and a boolean to control how the output is returned -- raw counts vs millivolts.

```c
typedef struct {
    uint8_t channel;
    uint8_t vref_millivolt;
    uint8_t oversample_count;
    bool as_millivolt;
} adc_config_t;
```

For the sample data, we need a timestamp for when the sample was captured, the raw count (12-bit), value as millivolts and a boolean for whether the sample is valid.

```c
typedef struct {
    uint32_t timestamp_us;
    uint16_t raw_12bit;
    uint16_t millivolt;
    bool valid;
} adc_sample_t;
```

And lastly, the driver itself needs to track if it has been initialized, started, it's configuration, and the last sample captured.

```c
typedef struct {
    bool initialized;
    bool started;
    adc_config_t config;
    adc_sample_t last_sample;
} adc_driver_t;
```

**NOTE: With this ordering, there should be a single byte of padding at the end of adc_driver_t due to the bool in adc_sample_t**

At this point, I realize that `app_init` has content that is not appropriate for application.  I need to properly structure how the system, platform, drivers and application are initialized, configured and executed.

First, its important to separate concerns.

- Platform: how hardware/MCU features are initialized and used (clock, stio, multicore launch, time, etc)
- Drivers: sensor/peripheral lifecycle and data access
- Runtime or system bootstrap: startup order, driver configuration, platform setup
- App: behavior policy and use-case flow

I'm introducing a `system` folder and abstraction.

                    main
                      |
                    system
                ______|_____
                |           |
            platform      drivers
           
I have discovered that the multicore api is rather simple in that the function passed to launch is immediately executed.  That makes sense, but it does not work well with an `init`, `config`, `run` flow.  I learned about a few similar solutions that essentially include a logical semaphore (flag, command, etc) that holds the function is an idle state until it is signaled to start processing.  An alternative is to hard reset the core and launch when needed.  This one is more low-level and seems out of place for how I'm structuring things.


**04JUL26** - There were many changes to get the improved structure and I left the changes uncommitted.  Things have stabilized, except for how to run Core 1, first thing to do is commit.