# Photon DB

Photon DB is our semester project. We are building a simple in-memory database (like Redis) in C++, using the [CodeCrafters Redis challenge](https://codecrafters.io/challenges/redis) as a guide.

Everyone on the team uses **Docker** so we all run the same code on the same tools — no matter if you use Windows, Mac, or Linux.

---

## What you need

1. [Git](https://git-scm.com/downloads)
2. [Docker Desktop](https://www.docker.com/products/docker-desktop/) — install it, then open it and wait until it says it is running

That is it. You do **not** need to install C++, g++, or anything else on your computer.

---

## Step 1 — Get the code

```bash
git clone https://github.com/AmritPant/photon-db
cd photon-db
```

---

## Step 2 — Build the Docker environment (one time)

```bash
docker compose build
```

This downloads a Linux box with the compiler and tools (g++, make, redis-cli). It can take a few minutes the first time.

---

## Step 3 — Open a work shell and start the server

```bash
docker compose run --rm dev
```

This drops you inside the container, with your project folder mounted at `/workspace`. From here, build and run the server:

```bash
make
./photondb
```

`make` compiles the project and produces the `photondb` binary. `./photondb` starts the server, which listens on port **6379**. Leave this terminal open — the server keeps running in the foreground.

---

## Step 4 — Test it from a second terminal

The server is now running inside a container. To talk to it with `redis-cli`, open a **new terminal** on your computer and connect into that _same_ running container.

**4.1 — Find the container's name**

```bash
docker ps
```

This lists running containers. Look for the one built from `photondb` (e.g. `photon-db-dev-run-...`) and copy its `NAMES` value.

**4.2 — Open a second shell inside that container**

```bash
docker exec -it <name> /bin/bash
```

Replace `<name>` with the name you copied. You are now inside the same container as the running server, in a separate shell.

**4.3 — Talk to the server with `redis-cli`**

```bash
redis-cli -p 6379
```

You should get an interactive prompt. Try a few commands:

```
127.0.0.1:6379> PING
PONG
127.0.0.1:6379> SET foo bar
OK
127.0.0.1:6379> GET foo
"bar"
```

You can also run one-off commands without the interactive prompt:

```bash
redis-cli -p 6379 PING
```

---

## Quick reference

| What you want to do                                | Command                            |
| -------------------------------------------------- | ---------------------------------- |
| Build the Docker environment                       | `docker compose build`             |
| Open a shell to write code / run the server        | `docker compose run --rm dev`      |
| Compile the project (inside container)             | `make`                             |
| Start the server (inside container)                | `./photondb`                       |
| List running containers (on your host)             | `docker ps`                        |
| Attach a second shell to the running container     | `docker exec -it <name> /bin/bash` |
| Test the server with the Redis CLI (in that shell) | `redis-cli -p 6379`                |
| Stop the server                                    | Press `Ctrl + C` in its terminal   |
| Remove build output                                | `make clean` (inside container)    |

---

## Where to edit code

- Main server entry point: `src/main.cpp`
- Configuration / AOF persistence: `src/config.cpp`, `src/aof.cpp`
- Command handlers: `lib/request-handler/`
- Core logic (store, RESP parser, request router, RDB, streams): `lib/`

---

## Problems?

**"Cannot connect to Docker"** — Open Docker Desktop and wait until it is fully started, then try again.

**"Port 6379 already in use"** — Something else is using that port. Stop it, or change `6379:6379` in `docker-compose.yml` to something like `6380:6379`.

**`docker ps` shows nothing** — Make sure the container from Step 3 (`docker compose run --rm dev` with `./photondb` running) is still open in its terminal.

**Build is slow the first time** — Normal. Docker is downloading tools and libraries. Later builds are faster.
