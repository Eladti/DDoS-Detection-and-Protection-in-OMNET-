# DDoS-Detection-and-Protection-in-OMNET-

This project implements a full simulation environment in **OMNeT++ + INET** for detecting large-scale DDoS attacks using **SNMP-based telemetry**, inspired by the LADS system ("Large-scale Automated DDoS Detection System", V. Sekar et al., 2006).  
The system monitors traffic passively, analyzes suspicious flows, detects anomalies, and dynamically blocks attack ports in real time.

The simulation includes several attack scenarios (single-burst, LADS-style detection windows, and multi-attacks) and evaluates the accuracy of the detection method.

---

## 📌 Project Structure
Project_DDoS_LADS/
│
├── NED/
│ ├── CFlowAnalyzer.ned
│ ├── CFlowCollector.ned
│ ├── CSNMPMonitor.ned
│ ├── CustomBurstSender.ned
│ ├── ServerModule.ned
│ └── CombinedTest1.ned ← Main network topology
│
├── SRC/
│ ├── CFlowAnalyzer.cc / .h
│ ├── CFlowCollector.cc / .h
│ ├── CSNMPMonitor.cc / .h
│ ├── CSNMP.msg (SNMP message format)
│ ├── CustomBurstSender.cc / .h
│ ├── RouterModule.cc / .h ← Custom router with tapping + blocking logic
│ ├── ServerModule.cc / .h
│ └── FlowRecord.h
│
├── results/
│ ├── SIM-90s-#.sca / .vec / .vci
│ ├── LADS-30s-#.sca / .vec / .vci
│ ├── LADS-60s-#.sca / .vec / .vci
│ ├── MULTIATTACKS-30s-#.sca / .vec / .vci
│ └── MULTIATTACKS-60s-#.sca / .vec / .vci
│
├── omnetpp.ini ← All simulation configurations
├── Makefile
└── Project_DDoS_LADS ← Executable (auto-generated)


---

## 🖧 Network Architecture

The simulated network includes:

- **Multiple attackers** (burst senders)
- **One legitimate client**
- A **custom router** that:
  - forwards packets normally
  - *taps* a lightweight copy (only source port) to the monitoring system
- **CSNMPMonitor**
- **CFlowCollector**
- **CFlowAnalyzer**
- A **server** receiving all traffic

The router performs the packet duplication needed for passive SNMP monitoring.

attackers ----
goodClient -----> [Custom Router] ---> [Server]
|
----> [CSNMPMonitor] → [Collector] → [Analyzer]



---

## 📡 Detection Pipeline (LADS-inspired)

### **1. CSNMPMonitor**
- Receives packet metadata (only source port)
- Records:
  - total packet count per window
  - per-port statistics
  - SNMP triggers
  - sampleRate vectors
- Computes anomalies based on:
  - `threshold`
  - `samplePeriod`
  - `floorFactor`

### **2. CFlowCollector**
- Aggregates flows every `collectionWindow`
- Exports suspicious flows
- Counts exported flows (vector: `flowsExported`)

### **3. CFlowAnalyzer**
- Identifies top attacking ports (`topN`)
- Sends commands to the router to **block attack ports**

---

## ⚙️ Simulation Configurations (from `omnetpp.ini`)

The project includes 5 predefined scenarios:

### **1. SIM-90s**
Single burst attack  
- 15s window  
- Trigger at ~45s  

### **2. LADS-30s**
Single attack  
- LADS 30s detection window

### **3. LADS-60s**
Single attack  
- LADS 60s window (slower but more stable)

### **4. MULTIATTACKS-30s**
Six attackers  
Multiple bursts at different times  
30s windows → detects each burst separately

### **5. MULTIATTACKS-60s**
Same attacks as above  
60s windows → merges nearby attacks into a single alert

---

## ▶️ Running the Simulation

### **Prerequisites**
- **OMNeT++ 6.x**
- **INET Framework 4.x**
- C++17 support enabled

### **Build**




### **Run a specific configuration**
Example:
./Project_DDoS_LADS -c SIM-90s
-c LADS-30s
-c LADS-60s
-c MULTIATTACKS-30s
-c MULTIATTACKS-60s


---

## 📈 Results Summary

### ✔️ Main trends observed:
- DDoS bursts cause **sharp increases in sampleRate**
- Detection is **accurate** when thresholds match the environment
- **Short windows (30s)** → fast detection but sensitive to noise
- **Long windows (60s)** → stable but may merge close attacks
- No false positives recorded
- Router successfully blocks attacking ports after detection

### 📊 Example peak values (max sampleRate)
| Scenario | Peak sampleRate |
|----------|------------------|
| SIM-90s | ~63k |
| MULTIATTACKS-30s | ~58k |
| MULTIATTACKS-60s | ~116k |
| LADS-30s | ~127k |
| LADS-60s | ~255k |

---

## 🧩 Challenges & Solutions

### ❗ Extending INET modules caused structural conflicts  
**Solution:**  
Build our own lightweight router instead of inheriting from INET modules.

### ❗ Full packets contained too much unnecessary data  
**Solution:**  
Tap only **source port numbers** → reduced overhead dramatically.

### ❗ Mismatches between detection windows & attack durations  
**Solution:**  
Tune `threshold`, `samplePeriod`, `collectionWindow`.

---

## 🚀 Future Improvements
- Detection using additional flow features (not only ports)
- Support for port randomization attacks
- More realistic attacker behavior models
- Integration with MULTOPS decision trees

---

## 👥 Authors
- **Elad Tibi**  

Course: *Computer Network Design – 371-1-0281*  
Ben-Gurion University of the Negev

---

