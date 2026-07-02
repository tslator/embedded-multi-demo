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