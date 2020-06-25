<h1 align="center">Welcome to cs143 👋</h1>
<p>
  <img alt="Version" src="https://img.shields.io/badge/version-1.0.0-blue.svg?cacheSeconds=2592000" />
  <a href="https://github.com/colinaaa/cs143/blob/master/LICENSE" target="_blank">
    <img alt="License: Unlicense" src="https://img.shields.io/badge/License-Unlicense-yellow.svg" />
  </a>
</p>

> laboratory assignments of cs143-Compilers

### 🏠 [Homepage](https://web.stanford.edu/class/cs143/)

## Build

```sh
docker build -t colinaaa/cs143 .
```

## Install

```sh
make install
```

## Usage

```fish
 docker run -it -v (pwd):/root/cs143 --name cs143 colinaaa/cs143:latest
```

## Readings
No textbook is required for the class, but if you would like to do some additional reading this page lists the relevant material from three popular texts:

- Compilers: Principles, Techniques, and Tools (CPTT, aka "The Dragon Book")
2nd edition
Aho, Lam, Sethi, and Ullman

- Engineering a Compiler (EC)
2nd edition
Cooper and Torczon

- Modern Compiler Implementation (MCI)
Appel, with Palsberg
Note: there are versions of this book tailored to C and Java, as well as ML.

#### Lexical Analysis and Finite Automata

- CPTT: Sections 3.1, 3.3, 3.4, 3.6, 3.7, 3.8
- EC: Chapter 2 through Section 2.5.1 except for 2.4.4
- MCI: Chapter 2

#### Parsing
- CPTT: Sections 4.1-4.6, 4.8.1, 4.8.2
- EC: Sections 3.1-3.5
- MCI: Chapter 3

#### Semantic Analysis and Types
- CPTT: Sections 5.1-5.3 6.3, 6.5
- EC: Sections 4.1-4.4
- MCI: Chapters 4 and 5

#### Runtime Systems and Code Generation
- CPTT: Sections 6.2, 7.1-7.4, 8.1-8.3, 8.6
- EC: Chapter 5, Sections 7.1-7.2
- MCI: Chapters 6, 7, and 14

#### Optimization
- CPTT: Sections 8.5, 8.7, 9.1-9.4
- EC: Sections 8.1-8.4, 8.6, 9.1-9.3
- MCI: Chapter 10

#### Advanced Topics: Register Allocation, Garbage Collection
- CPTT: Sections 7.5-7.6, Section 8.8
- EC: Sections 13.1-13.2, 13.4,
- MCI: Chapters 11 and 13

## Author

👤 **Colin Wang**

* Github: [@colinaaa](https://github.com/colinaaa)

## Show your support

Give a ⭐️ if this project helped you!

## 📝 License

Copyright © 2020 [Colin Wang](https://github.com/colinaaa).<br />
This project is [Unlicense](https://github.com/colinaaa/cs143/blob/master/LICENSE) licensed.

***
_This README was generated with ❤️ by [readme-md-generator](https://github.com/kefranabg/readme-md-generator)_
