# Secure Image Steganography Using C

A modular C-based application for securely hiding and extracting confidential text messages within BMP images using Least Significant Bit (LSB) steganography and cryptographic techniques.

---

## About the Project

Image Steganography is a technique used to hide confidential information within a digital image without causing noticeable changes to its appearance.

This project implements a secure image steganography system using the C programming language. It uses Least Significant Bit (LSB) data hiding techniques to embed secret text messages into BMP image files.

The application also includes separate encryption, encoding, and decoding modules for better security and code organization.

---

## Features

- Hide confidential text messages inside BMP images.
- Extract hidden messages from stego images.
- Implemented using the Least Significant Bit (LSB) technique.
- Separate encoding and decoding modules.
- Cryptography module for secure data handling.
- File handling for image and text data.
- Modular C programming structure.

---

## Technologies Used

- C Programming
- File Handling
- Bitwise Operations
- Image Processing
- Least Significant Bit (LSB) Steganography
- Cryptography
- Modular Programming

---

## Project Structure

```text
Image-Steganography-for-Secure-Data-Communication/
│
├── README.md
├── LICENSE
├── .gitignore
│
├── src/
│   ├── main.c
│   ├── encode.c
│   ├── encode.h
│   ├── decode.c
│   ├── decode.h
│   ├── crypto.c
│   ├── crypto.h
│   └── types.h
│
└── samples/
    ├── secret.txt
    └── README.md
