# STM32 local safety state machine — host-mocked design
Outputs are disabled at MCU reset. `AETHER_ACTUATION_ENABLED=0` permanently prevents real motor drive in this revision.
| State | Trigger | Next | Action |
|---|---|---|---|
| BOOT | reset | SELF_TEST | PWM not configured, external nSLEEP pulldowns keep off |
| SELF_TEST | prerequisites valid | DISARMED | mark safe startup; remain unarmed |
| SELF_TEST | invalid | FAULT | latch cause, sleep disabled |
| DISARMED | E-stop released + locally rearmed, self-test, valid heartbeat, interlock, explicit ARM | ARMED | retain PWM zero |
| ARMED | validated DRIVE and fresh link | RUNNING | motor outputs allowed only if compiled hardware interlock separately approved |
| RUNNING | valid DRIVE / HEARTBEAT | RUNNING | refresh 200ms independent command and heartbeat time budgets |
| RUNNING/ARMED | no valid DRIVE/HEARTBEAT for >=200ms | DISARMED | immediately sleep disable and PWM zero; optional ramp may begin earlier at 170ms |
| Any except BOOT | NC E-stop loop opens | ESTOP | immediately sleep disable, hardware contacts cut motor VM |
| Any except ESTOP | driver nFAULT, overcurrent, encoder stall, battery-critical | FAULT | sleep disabled, latch fault; report, NO silent compensation |
| ARMED/RUNNING | DISARM or STOP | DISARMED or ARMED (STOP) | PWM zero; STOP cannot rearm |
| ESTOP | physical release stable + explicit local rearm | DISARMED | require new valid heartbeat + separate ARM |
| FAULT | manual fault review + explicit clear | DISARMED | no automatic restart |

**Priority:** physical ESTOP > latched FAULT > link/phone-command timeout > velocity command. Independent IWDG must be started only after loop health metric is operational and fed only from main loop (never ISR). Current HAL bootstrap does not start IWDG and MUST NOT be deployed as a production safety controller.

**Important:** electrical output disable by 200ms does not guarantee the rover mechanically stops within 200ms; coasting/braking distance requires measurement. A safe hardware E-stop cutting VM is independent of USB/Pi.
