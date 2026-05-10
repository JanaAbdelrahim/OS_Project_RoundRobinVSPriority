CPU Scheduler Simulator

A modern CPU Scheduling Simulator developed using C++ and SFML for the Operating Systems course project.

The simulator demonstrates and compares different CPU scheduling algorithms through an interactive graphical interface, Gantt chart visualization, and performance metrics analysis.

---

Implemented Algorithms

1. Round Robin (RR)

- Time-sharing scheduling algorithm
- Uses configurable Quantum
- Fair CPU distribution among processes
- Prevents starvation

2. Priority Scheduling (Preemptive)

- Higher-priority processes can interrupt currently running processes
- Lower priority number = higher priority

3. Priority Scheduling (Non-Preemptive)

- Process runs until completion once selected
- Simpler implementation
- Can cause starvation for low-priority processes

---

Features

- Modern GUI using SFML
- Interactive process input panel
- Dynamic Gantt Chart visualization
- Process metrics table
- Average Waiting Time calculation
- Average Turnaround Time calculation
- Average Response Time calculation
- Algorithm comparison screen
- Built-in test scenarios
- Input validation
- Scrollable results screen

---

Technologies Used

- C++
- SFML 2.6.1
- Object-Oriented Programming (OOP)
- Data Structures
- Scheduling Algorithms

```

Project Structure

project-root/
│
├── src/
│   ├── algorithms/
│   ├── core/
│   ├── gui/
│   ├── utils/
│   └── main.cpp
│
├── include/
│   ├── algorithms/
│   ├── core/
│   ├── gui/
│   └── utils/
│
├── assets/
│
├── screenshots/
│
├── test-cases/
│
├── README.md
└── .gitignore

```
GUI Screens

The application includes:

- Main Input Window
- Scheduling Algorithm Selection
- Process List Panel
- Gantt Chart Visualization
- Results Tables
- Comparison & Conclusion Screen

---

Metrics Calculated

Turnaround Time

Turnaround Time = Finish Time - Arrival Time

Waiting Time

Waiting Time = Turnaround Time - Burst Time

Response Time

Response Time = First Start Time - Arrival Time

---

Sample Test Scenario

Processes

Process| Arrival| Burst| Priority
P1| 0| 5| 2
P2| 1| 3| 1
P3| 2| 8| 4

Quantum

2

---

How To Run

Requirements

- C++ Compiler supporting C++17
- SFML 2.6.1
- CodeBlocks / Visual Studio / CLion

---

SFML Setup

Download SFML from:

https://www.sfml-dev.org/download.php

Recommended Version:

SFML 2.6.1 - MinGW 11.2.0 (64-bit)

---

Build Instructions

Using CodeBlocks

1. Open the project
2. Configure SFML include/lib paths
3. Build and Run

---

Required SFML Libraries

sfml-graphics
sfml-window
sfml-system

---

Screenshots

Main Window

Add screenshot here:

screenshots/main-window.png

Round Robin Result

screenshots/round-robin.png

Priority Scheduling

screenshots/priority.png

Comparison Screen

screenshots/comparison.png

---

Test Cases

The repository includes multiple documented scenarios inside:

test-cases/

Example scenarios:

- Basic workload
- Fairness testing
- Starvation case
- Urgent processes
- Validation cases

---

Key Concepts Demonstrated

- CPU Scheduling
- Time Sharing
- Context Switching
- Starvation
- Fairness
- Preemption
- Queue Management
- Performance Metrics

---

Team Members

- Maryam Mostafa
- Add remaining team members here

---

Notes

- Lower priority number means higher scheduling priority.
- Round Robin uses FIFO queue scheduling.
- Preemptive scheduling may interrupt currently running processes.
- Gantt chart bars are generated dynamically based on execution order.

---

Future Improvements

- Add more scheduling algorithms
- Export results to files
- Real-time animation
- CPU utilization calculation
- Multilevel Queue Scheduling
- SJF & FCFS algorithms

---

License

This project was developed for educational purposes as part of the Operating Systems course project.