# Homomorphic Encryption-Based DC Motor Control (CKKS + Microsoft SEAL)

This project implements a **PD-controlled DC motor simulation** where **all computations are performed on encrypted data** using the **CKKS scheme** from [Microsoft SEAL](https://github.com/microsoft/SEAL).  
It demonstrates the concept of **post-quantum secure encrypted control systems**, ensuring that sensitive control parameters and system states remain private.



## 📜 Overview

- **Scheme:** CKKS (supports approximate arithmetic on encrypted real numbers)
- **Library:** Microsoft SEAL 3.6
- **Programming Language:** C++  
- **Control Type:** PD (Proportional-Derivative) control
- **Encryption Context:** Parameters chosen to balance accuracy and noise budget for multiple operations.



## 🛠 Features

1. **Encrypted Motor State Updates**  
   Motor speed and error values are updated while encrypted.
   
2. **PD Controller on Encrypted Data**  
   Control voltage `v` is computed from encrypted error and derivative terms.

3. **Partial Decryption in Loop**  
   For simplicity, intermediate decryption is performed within the loop for analysis, but the structure supports a fully encrypted pipeline.

4. **Scientific Notation Output**  
   Errors are printed in scientific format (e.g., `1.23e-08`) for precision clarity.



## 📄 File Explanations

### **DC_Motor_PD.cpp**
- Sets CKKS parameters (poly modulus degree, coefficient modulus, scale).
- Initializes keys (public, secret, relinearization, Galois).
- Defines motor parameters (`J`, `b`, `K`, `R`, `L`).
- Implements PD control:
  ```cpp
  v = kp * error + kd * derivative;


