# HAMPOD High-Level Requirements Document

> **Document Status:** DRAFT - Requirements Elicitation In Progress  
> **Created:** 2026-01-21  
> **Last Updated:** 2026-01-21  
> **Methodology:** Aerospace-style requirements engineering (DO-178C/DO-254 inspired)

---

## 1. Document Purpose and Scope

This document captures the formal requirements for the HAMPOD system, structured in a hierarchical manner suitable for verification and validation:

1. **System-Level Requirements (SLR)** - Define the overall system purpose, interfaces, and operational environment
2. **High-Level Requirements (HLR)** - Decompose SLRs into functional and non-functional requirements
3. **Low-Level Requirements (LLR)** - Specify implementation-level behaviors (to be documented separately)

### 1.1 Traceability Goals

Every requirement shall:
- Have a unique identifier
- Be traceable to a parent requirement (except SLRs)
- Be verifiable through test, analysis, inspection, or demonstration
- Be unambiguous and testable

### 1.2 Requirement Notation

Requirements use the following format:
```
[ID] The system SHALL/SHOULD/MAY <behavior> WHEN <condition>.
     Rationale: <why this requirement exists>
     Parent: <parent requirement ID>
     Verification: <Test | Analysis | Inspection | Demonstration>
```

**Priority Keywords:**
- **SHALL** = Mandatory requirement (must be implemented)
- **SHOULD** = Strongly desired (implement unless technically infeasible or cost-prohibitive)
- **MAY** = Optional/Nice-to-have (implement if resources permit)

---

## 2. System-Level Requirements (SLR)

### 2.1 System Purpose

**[SLR-001] Mission Statement**
> The HAMPOD SHALL provide audible output and tactile (keypad) input to vision-impaired ham radio operators with CAT-controlled radios in order to assist them in effectively using the features of their radios that would require seeing the display.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Rationale | This is the core mission of the system—enabling blind/visually impaired operators to independently operate amateur radios |
| Verification | Demonstration |

---

### 2.2 Operational Environment

**[SLR-002] Primary Operating Environment**
> The HAMPOD SHALL be designed for use in a home/shack environment.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Most amateur radio operation occurs in fixed home stations |
| Verification | Demonstration |

**[SLR-003] Portable/Field Operation Capability**
> The HAMPOD SHOULD be capable of use in portable and field operations where adequate battery or generator power is available.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-001 |
| Rationale | Many operators participate in field day, portable operations, or emergency communications |
| Verification | Demonstration |

**[SLR-004] Physical Form Factor**
> The HAMPOD SHOULD be fairly compact, ideally very small, but SHALL NOT occupy more space than a typical ham radio desktop transceiver.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (compact) / SHALL NOT (size limit) |
| Parent | SLR-002, SLR-003 |
| Rationale | Desk space is limited in most shacks; portability requires compact size |
| Verification | Inspection |

---

### 2.3 Device Control Scope

**[SLR-005] Single Device Optimized Control**
> The HAMPOD SHALL handle one radio or device well at any given time.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Primary use case is controlling one radio; this must work excellently |
| Verification | Test |

**[SLR-006] Multi-Device Switching**
> The HAMPOD SHOULD be switchable to control multiple other devices, such as other radios, antenna tuners, or antenna rotors that have computer interfaces.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-001 |
| Rationale | Many operators have multiple radios or accessories; seamless switching improves usability |
| Verification | Demonstration |

---

### 2.4 User Classes and Personalization

**[SLR-007] Single User Configuration**
> The HAMPOD SHALL be intended for configuration for a single user's preferences.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Simplifies configuration; most operators have their own dedicated equipment |
| Verification | Inspection |

---

### 2.5 Compatibility and Legacy Behavior

**[SLR-008] Original HAMPOD Functional Compatibility**
> The HAMPOD SHALL reproduce very similar functionality and interface behavior to the original HAMPOD as defined in the ICOMReaderManual.txt and associated files for other radio types.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Existing users expect familiar behavior; proven interface design should be preserved |
| Verification | Demonstration, Inspection |
| Reference | `Documentation/Original_Hampod_Docs/ICOMReaderManual.txt` |

---

### 2.6 Performance Requirements

**[SLR-009] Responsive and Fast Operation**
> The HAMPOD interface SHALL be responsive and fast to operate.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Slow audio feedback interrupts operator flow and degrades user experience |
| Verification | Test |

**[SLR-010] Terse Mode Speech Control**
> The HAMPOD SHALL provide the ability to shorten speech feedback via a "terse mode."

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-009 |
| Rationale | Experienced users want minimal speech to operate quickly |
| Verification | Test |

**[SLR-011] Speech Speed Adjustment**
> The HAMPOD SHALL allow the user to speed up or slow down speech output.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-009 |
| Rationale | Different users have different preferences and comprehension speeds |
| Verification | Test |

**[SLR-012] Elimination of Unnecessary Speech**
> The HAMPOD SHALL minimize or eliminate unnecessary speech to improve user experience.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-009 |
| Rationale | Verbose output slows operation and frustrates experienced users |
| Verification | Test, Inspection |

---

### 2.7 Reliability Requirements

**[SLR-013] Robustness to Unusual Inputs**
> The HAMPOD SHALL be very reliable and SHALL NOT crash when presented with unusual inputs, such as when the radio is turned off and back on during operation.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Operators must not lose functionality due to normal radio power cycling |
| Verification | Test |

**[SLR-014] Idle Stability**
> The HAMPOD SHALL be especially reliable when idle; it SHALL NOT require a power cycle to restore operation after sitting idle for a week or more.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-013 |
| Rationale | Operators may not use their radios daily; system must remain functional |
| Verification | Test |

**[SLR-015] Power Failure Resilience**
> The HAMPOD system SHALL NOT be corrupted by an unexpected power-down event.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-013 |
| Rationale | Power interruptions must not require user intervention or reinstallation |
| Verification | Test |

**[SLR-016] Fast Boot Time**
> The HAMPOD SHOULD boot up and be operational in less than 30 seconds.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-009 |
| Rationale | Shorter boot times improve user experience, especially for portable use |
| Verification | Test |

---

### 2.8 User Interface Requirements

**[SLR-017] Intuitive Without Manual**
> The HAMPOD interface SHOULD be easy to understand without the manual.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-001 |
| Rationale | Users should be able to discover functionality through the interface |
| Verification | Demonstration |

**[SLR-018] Key Function Announcement**
> The HAMPOD SHALL announce the behavior of a key (or with hold or shift modifiers) when appropriate.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-017 |
| Rationale | Provides discoverability and confirmation of key functions |
| Verification | Test |

**[SLR-019] Set Modifier Pattern**
> The HAMPOD SHALL allow adjustment of a parameter using the same key combination with the "set" modifier.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-017 |
| Rationale | Consistent interface pattern: query with key, adjust with set+key |
| Verification | Test |

**[SLR-020] Mode Change Announcements**
> The HAMPOD SHALL inform the user via audio when modes change (e.g., to configuration mode, DTMF mode, etc.).

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-017, SLR-001 |
| Rationale | Users must always know what mode they are in since they cannot see a display |
| Verification | Test |

**[SLR-021] Frequency and Mode Awareness**
> The HAMPOD SHALL make it easy for the user to know the frequency and transmitter mode even if it is changed on the radio rather than on the HAMPOD.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Critical for situational awareness; changes may occur via radio knobs |
| Verification | Test |

---

### 2.9 Speech Synthesis Requirements

**[SLR-022] Speech Clarity**
> The HAMPOD SHALL have a clear speech synthesizer of quality at least as good as Piper on the Raspberry Pi 5.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Clear speech is essential for accurate frequency/mode comprehension |
| Verification | Demonstration |

**[SLR-023] Lower Performance Platform Speech**
> The HAMPOD MAY accept a lower performance synthesizer (such as Festival) to enable operation on lower performance platforms.

| Attribute | Value |
|-----------|-------|
| Priority | MAY (Optional) |
| Parent | SLR-022 |
| Rationale | Trade-off acceptable if it enables broader hardware support |
| Verification | Demonstration |

**[SLR-024] Ideal Speech Synthesizer**
> A high-quality synthesizer with lower computational performance requirements SHOULD be preferred if available.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-022 |
| Rationale | Best of both worlds: quality + performance |
| Verification | Analysis |

---

### 2.10 Platform and Hardware Requirements

**[SLR-025] Primary Platform Support**
> The HAMPOD SHALL work on a system with the performance of a Raspberry Pi 5 or lower.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Pi 5 is the current reference platform |
| Verification | Test |

**[SLR-026] Legacy Platform Support**
> The HAMPOD SHOULD work on older Raspberry Pi platforms (Pi 4, Pi 3, Pi 3B, Pi 3B+) with minimal delay.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-025 |
| Rationale | Older hardware is less expensive and more widely available |
| Verification | Test |

**[SLR-027] Commodity Hardware**
> The HAMPOD SHALL be able to operate entirely on hardware (processor, keypad, speaker, cables, etc.) that can be purchased online (e.g., Amazon) as commodity parts.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | Ensures accessibility; users must be able to source parts easily |
| Verification | Inspection |

**[SLR-028] Integrated Hardware Support**
> The HAMPOD SHOULD support more customized integrated keypads or speaker hats for the RPi that would allow a more compact system.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-004 |
| Rationale | Enables more polished, compact builds for advanced users |
| Verification | Demonstration |

---

### 2.11 Extensibility Requirements

**[SLR-029] Easy Radio Model Addition**
> The HAMPOD SHALL be easy to modify to support a new radio model, including new features or behaviors that might be added.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-001 |
| Rationale | The amateur radio market has many radio models; extensibility is essential |
| Verification | Inspection, Demonstration |

**[SLR-030] Hamlib Library Usage**
> The HAMPOD SHALL use the Hamlib library to control radios.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL (Mandatory) |
| Parent | SLR-029 |
| Rationale | Hamlib provides standardized CAT control for hundreds of radio models |
| Verification | Inspection |

**[SLR-031] Automatic Radio Detection**
> The HAMPOD SHOULD provide an automated way to detect what radio is plugged in to the USB port or serial adapter cable without much input from the user.

| Attribute | Value |
|-----------|-------|
| Priority | SHOULD (Strongly Desired) |
| Parent | SLR-029 |
| Rationale | Reduces user configuration burden |
| Verification | Test |

---

### 2.12 Anti-Goals (Explicit Non-Requirements)

**[SLR-032] Sighted-User Features Not Required**
> The HAMPOD does NOT need to provide functions that are only useful for sighted users.

| Attribute | Value |
|-----------|-------|
| Priority | NOT REQUIRED |
| Rationale | Focus development effort on accessibility features |
| Verification | Inspection |

**[SLR-033] Non-Technical User Assumption**
> The HAMPOD SHALL NOT assume great technical familiarity from the primary user.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL NOT |
| Parent | SLR-017 |
| Rationale | Users are ham radio operators, not necessarily computer experts |
| Verification | Demonstration |

**[SLR-034] Minimal Non-Keypad Interaction**
> The HAMPOD SHALL NOT require a lot of interaction other than with the speaker/keypad interface, except that some initial computer setup MAY be required but SHOULD be minimized or eliminated.

| Attribute | Value |
|-----------|-------|
| Priority | SHALL NOT (ongoing) / MAY (initial setup) |
| Parent | SLR-001 |
| Rationale | Daily operation must be entirely via the accessible keypad/speaker interface |
| Verification | Demonstration |

---

## 3. High-Level Requirements (HLR)

> **Note:** HLRs decompose the SLRs into specific functional and interface requirements. These will be elicited in subsequent sessions.

### 3.1 Functional Requirements

<!-- TO BE ELICITED: Radio control, frequency entry, mode switching, etc. -->

### 3.2 Non-Functional Requirements

<!-- TO BE ELICITED: Performance timing, memory limits, etc. -->

### 3.3 Interface Requirements

<!-- TO BE ELICITED: Keypad mapping, audio output specs, CAT protocols, etc. -->

### 3.4 Safety and Reliability Requirements

<!-- TO BE ELICITED: Error handling, recovery procedures, etc. -->

---

## 4. Requirements Traceability Matrix

| Requirement ID | Parent | Priority | Verification Method | Status |
|----------------|--------|----------|---------------------|--------|
| SLR-001 | — | SHALL | Demonstration | Draft |
| SLR-002 | SLR-001 | SHALL | Demonstration | Draft |
| SLR-003 | SLR-001 | SHOULD | Demonstration | Draft |
| SLR-004 | SLR-002, SLR-003 | SHOULD/SHALL NOT | Inspection | Draft |
| SLR-005 | SLR-001 | SHALL | Test | Draft |
| SLR-006 | SLR-001 | SHOULD | Demonstration | Draft |
| SLR-007 | SLR-001 | SHALL | Inspection | Draft |
| SLR-008 | SLR-001 | SHALL | Demonstration, Inspection | Draft |
| SLR-009 | SLR-001 | SHALL | Test | Draft |
| SLR-010 | SLR-009 | SHALL | Test | Draft |
| SLR-011 | SLR-009 | SHALL | Test | Draft |
| SLR-012 | SLR-009 | SHALL | Test, Inspection | Draft |
| SLR-013 | SLR-001 | SHALL | Test | Draft |
| SLR-014 | SLR-013 | SHALL | Test | Draft |
| SLR-015 | SLR-013 | SHALL | Test | Draft |
| SLR-016 | SLR-009 | SHOULD | Test | Draft |
| SLR-017 | SLR-001 | SHOULD | Demonstration | Draft |
| SLR-018 | SLR-017 | SHALL | Test | Draft |
| SLR-019 | SLR-017 | SHALL | Test | Draft |
| SLR-020 | SLR-017, SLR-001 | SHALL | Test | Draft |
| SLR-021 | SLR-001 | SHALL | Test | Draft |
| SLR-022 | SLR-001 | SHALL | Demonstration | Draft |
| SLR-023 | SLR-022 | MAY | Demonstration | Draft |
| SLR-024 | SLR-022 | SHOULD | Analysis | Draft |
| SLR-025 | SLR-001 | SHALL | Test | Draft |
| SLR-026 | SLR-025 | SHOULD | Test | Draft |
| SLR-027 | SLR-001 | SHALL | Inspection | Draft |
| SLR-028 | SLR-004 | SHOULD | Demonstration | Draft |
| SLR-029 | SLR-001 | SHALL | Inspection, Demonstration | Draft |
| SLR-030 | SLR-029 | SHALL | Inspection | Draft |
| SLR-031 | SLR-029 | SHOULD | Test | Draft |
| SLR-032 | — | NOT REQUIRED | Inspection | Draft |
| SLR-033 | SLR-017 | SHALL NOT | Demonstration | Draft |
| SLR-034 | SLR-001 | SHALL NOT / MAY | Demonstration | Draft |

---

## 5. Glossary

| Term | Definition |
|------|------------|
| CAT | Computer-Aided Transceiver; protocol for computer control of radios |
| Hamlib | Open-source library for standardized radio control via CAT |
| Piper | Neural network-based text-to-speech engine |
| Festival | Traditional rules-based text-to-speech engine |
| Terse Mode | Operating mode with minimal speech output |
| Set Modifier | Key modifier used to change values (query vs. adjust pattern) |
| DTMF | Dual-Tone Multi-Frequency; audio tones for signaling |

---

## 6. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 0.1 | 2026-01-21 | — | Initial document structure |
| 0.2 | 2026-01-21 | Wayne Padgett / Claude | System-Level Requirements (SLR-001 through SLR-034) captured from elicitation |

