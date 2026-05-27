# SlackForge: Slack Space Forensics Toolkit

SlackForge is a digital forensics utility developed in C, designed to interact with the exFAT filesystem. It provides a technical framework for identifying, analyzing, and utilizing "slack space"—the unused storage area between the logical end of a file and the physical end of the last cluster assigned to it.

## Technical Capabilities

*   **Low-Level exFAT Parsing**: Directly scans disk structures to identify clusters, volume boot records, and file allocation tables to calculate exact slack regions.
*   **AES-Based Hidden Storage**: Implements a proof-of-concept module for storing data within slack space, protected by AES encryption.
*   **Forensic Slack Carving**: Automates the scanning and extraction of data fragments residing in the slack regions of a storage device.
*   **Entropy-Based Detection**: Includes a heuristic scanner that uses entropy analysis to identify suspicious or encrypted content hidden in otherwise "empty" areas.

## Core Modules

1.  **Filesystem Analyzer**: Maps the exFAT structure to locate unallocated space within assigned clusters.
2.  **Cryptographic Engine**: Handles AES-256 encryption and decryption for data-hiding operations.
3.  **Heuristic Scanner**: Calculates Shannon entropy to differentiate between zeroed-out slack and high-entropy hidden payloads.

## Getting Started

### Prerequisites
*   **Environment**: Linux (Kali Linux preferred for raw block device access).
*   **Compiler**: `gcc`.
*   **Libraries**: `openssl` (for AES implementation).

### Installation & Compilation
```bash
# Clone the repository
git clone [https://github.com/21-Pranav/SlackForge-Slack-Space-Forensics-Toolkit.git](https://github.com/21-Pranav/SlackForge-Slack-Space-Forensics-Toolkit.git)
cd SlackForge-Slack-Space-Forensics-Toolkit

# Compile the source code
gcc -o slackforge main.c -lcrypto
