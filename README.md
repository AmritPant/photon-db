# Photon DB

Photon DB is our semester project. We are building a simple in-memory database (like Redis) in C++ using the [CodeCrafters Redis challenge](https://codecrafters.io/challenges/redis) as a guide.

Everyone on the team uses **Docker** so we all run the same code on the same tools — no matter if you use Windows, Mac, or Linux.

---

## What you need

1. [Git](https://git-scm.com/downloads)
2. [Docker Desktop](https://www.docker.com/products/docker-desktop/) — install it, then open it and wait until it says it is running

That is it. You do **not** need to install C++, CMake, or anything else on your computer.

---

## Step 1 — Get the code

```bash
git clone https://github.com/AmritPant/photon-db
cd photon-db
```

Replace `https://github.com/AmritPant/photon-db` with the real link to your team's GitHub repo.

---

## Step 2 — Build the Docker environment (one time)

```bash
docker compose build
```

This downloads a Linux box with the right compiler and tools. It can take a few minutes the first time.

---

## Step 3 — Open a work shell

```bash
docker compose run --rm dev
```

You are now inside the container. Your project folder is mounted here, so any file you edit on your computer shows up inside too.

---

## Step 4 — Build and run the server

Inside the container, run:

```bash
./your_program.sh
```

The server starts on port **6379**. Leave this terminal open while the server runs.

---

## Step 5 — Test it (optional)

Open a **second terminal** on your computer. Make sure the server is still running from Step 4, then run:

```bash
docker compose run --rm dev bash -c "echo -e '*1\r\n\$4\r\nPING\r\n' | nc -w 2 host.docker.internal 6379"
```

If things work, you should see a `+PONG` reply.

---

## Quick reference

~
`| What you want to do                         | Command                       |
| ------------------------------------------- | ----------------------------- |
| Build the Docker environment                |`docker compose build`       |
| Open a shell to write code                  |`docker compose run --rm dev`|
| Build and run the server (inside container) |`./your_program.sh`          |
| Run the server from outside (one command)   |`docker compose up server`   |
| Stop the server                             | Press`Ctrl + C` |

---

## Where to edit code

- Main server code: `src/main.cpp`
- Command handlers: `lib/request-handler/`
- Other logic: `lib/`

---

## Pushing to CodeCrafters

When you want to submit a stage to CodeCrafters:

```bash
git add .
git commit -m "describe your change"
git push origin master
```

CodeCrafters will test your code on their servers. You do not need Docker for that step — just push your changes.

---

## Problems?

**"Cannot connect to Docker"** — Open Docker Desktop and wait until it is fully started, then try again.

**"Port 6379 already in use"** — Something else is using that port. Stop it, or change `6379:6379` in `docker-compose.yml` to something like `6380:6379`.

**Build is slow the first time** — Normal. Docker is downloading tools and libraries. Later builds are faster.
