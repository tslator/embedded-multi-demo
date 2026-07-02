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
Established an application interface an integrated the led toggle into application run