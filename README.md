# Homomorphic Encryption-Based DC Motor Control

This project implements a **PD-controlled DC motor simulation** where **all computations are performed on encrypted data** using the **CKKS scheme** from [Microsoft SEAL](https://github.com/microsoft/SEAL).  
It demonstrates the concept of **post-quantum secure encrypted control systems**, ensuring that sensitive control parameters and system states remain private.



## Overview

- **Scheme:** CKKS (supports approximate arithmetic on encrypted real numbers)
- **Library:** Microsoft SEAL 3.6
- **Programming Language:** C++  
- **Control Type:** PD (Proportional-Derivative) control
- **Encryption Context:** Parameters chosen to balance accuracy and noise budget for multiple operations.



## Features

1. **Encrypted Motor State Updates**  
   Motor speed and error values are updated while encrypted.
   
2. **PD Controller on Encrypted Data**  
   Control voltage `v` is computed from encrypted error and derivative terms.

3. **Partial Decryption in Loop**  
   For simplicity, intermediate decryption is performed within the loop for analysis, but the structure supports a fully encrypted pipeline.

4. **Scientific Notation Output**  
   Errors are printed in scientific format (e.g., `1.23e-08`) for precision clarity.



## File Explanations

### **DC_Motor_PD.cpp**
- Sets CKKS parameters (poly modulus degree, coefficient modulus, scale).
- Initializes keys (public, secret, relinearization, Galois).
- Defines motor parameters (`J`, `b`, `K`, `R`, `L`).
- Implements PD control:
  ```cpp
  v = kp * error + kd * derivative;
- Runs simulation loop until target speed is reached or maximum time is exceeded.
- Encrypts/decrypts at each step (partial decryption for analysis).

### **CKKS_encrypt_decrypt.cpp**
- Wraps Microsoft SEAL API for easy encryption/decryption of:
  - double
  - vector<double>

- Example functions:
  - encrypt_double
  - decrypt_double
  - encrypt_vector
  - decrypt_vector

### **CKKS_homomorphic_operations.cpp**
- Implements basic CKKS operations using Evaluator:
  - Addition of ciphertexts
  - Multiplication (with relinearization)
  - Scalar multiplication
  - Rescaling

- These keep the main simulation code clean



## Building the Project

### 1. Clone Repository
```bash
git clone https://github.com/ananya-verma190/Homomorphic-Encryption.git
cd CKKS
```
### 2. Install Dependencies
- Microsoft SEAL (v3.6 or earlier)
- CMake ≥ 3.10
- C++17 compiler (GCC, Clang, MSVC)

### 3. Build
```bash
mkdir build
cd build
cmake ..
make
```

### 4. Run
```bash
./CKKS_encrypt_decrypt
./CKKS_homomorphic_operations
./DC_Motor
./DC_Motor_PD
```


## Learning Resources
- Microsoft SEAL Examples
- Homomorphic Encryption for Arithmetic of Approximate Numbers (CKKS Paper)
- Control Theory Basics (PD Control)


## Notes
- This code is designed for educational demonstration.
- For real-world secure control, avoid intermediate decryption inside the control loop.
