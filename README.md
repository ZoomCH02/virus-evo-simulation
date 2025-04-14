# 🦠 virus-evo-simulation

**virus-evo-simulation** is a visual, real-time simulation of virus spread and mutation, written in **C** with **SDL2**. The project models the behavior of viral strains inside a cellular automaton: infection, death, revival, and evolution through random mutations. It features a multithreaded engine and a graphical interface powered by SDL2.

[🇷🇺 Читать на русском](./README.ru.md)

![Virus Simulation Demo](https://github.com/user-attachments/assets/fae9d706-6876-4eb4-a032-8e36f6680d50)

## 📸 Live Preview

![0414](https://github.com/user-attachments/assets/c660ac21-eacb-4e9f-8285-b95a095e919c)

## ⚙️ Features

- Cellular automaton simulation with 3 states: **Healthy**, **Infected**, **Dead**
- Dynamic mutations that generate new virus strains with unique properties and visuals
- Dead cells can be **revived** by nearby healthy cells
- Up to **10 concurrent strains** on the grid
- **Mutation history log** with descriptive messages
- **Real-time graphics** with SDL2 and TTF text rendering
- **Multithreaded simulation** using `pthread` and mutex protection for data safety

## 🛠️ Build & Run (Linux)

### Prerequisites

Install required libraries (example for Debian/Ubuntu-based systems):

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev build-essential
```

Build the project

```bash
make
```

Run the simulation

```bash
./virus_sim
```

Clean compiled files

```bash
make clean
```

## 📁 Project Structure

```bash
.
├── include/             # Header files
├── src/
│   ├── main.c           # Entry point
│   ├── simulation/      # Simulation logic
│   │   ├── core.c       # Main loop and control
│   │   ├── grid.c       # Grid and cell operations
│   │   └── mutation.c   # Mutation logic
│   └── rendering/       # SDL rendering engine
│       └── render.c     # Drawing cells and mutation effects
├── Makefile             # Build script
└── README.md            # Project documentation
```

## 🧬 Simulation Logic
* Each cell can be in one of three states: HEALTHY, INFECTED, or DEAD
* Infected cells either recover or die, based on the virus strain's parameters
* Dead cells have a chance to be revived
* Each virus strain includes:
  * A unique color
  * Infection, death, and revival probabilities
  * Recovery time
* When a strain infects enough cells, it mutates, creating a new strain with modified properties and color
* Mutations trigger a visual effect and are recorded in a mutation log

## 🤝 Contributions Welcome
Pull requests are highly appreciated! If you have ideas for improvements — like enhanced mutation systems, charts, save/load functionality, or performance optimizations — join the development!

If you like this project, consider ⭐️ starring it on GitHub!

