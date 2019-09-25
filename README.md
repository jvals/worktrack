# Worktracker

## What is this?
Honestly, a spreadsheet with extra steps.

The purpose of this project is to provide a simple way to record the
amount of hours you spend at work. This is useful if you are working a
lot of unregistered overtime. The API provides endpoints for starting
and stopping a work session, computing the total amount of overtime
and checking if you are currently working overtime.


## How does it work?
The program is written in C using UNIX sockets. The main loop listens
for incomming TCP requests and parses the requests manually. If a
valid HTTP message is detected, the request is routed to a matching
function. If there is no match for the path, a 404 response is
dispatched. All requests must have an authorization header containing
a constant bearer token.

SQLite is used for persistence. The most important table is the time
table, which holds a "from" date and a "to" date stored as epoch
timestamps in seconds. Every entry in the time table corresponds to a
work session.


## How to run it?
There are two ways to run the program, direct or Docker.

### Direct
#### Dependecies
* gcc/clang
* sqlite headers

#### Build
You need to build the logging library first:

`cd clogger && make && cd ..`

Then you can build the application:

`make`

#### Run
`./server`

### Docker
Build the Docker image:

`docker build --rm -t worktrack:latest .`

The `--rm` flag cleans up intermediate images, and the `-t` flag tags the image.

Run a container from the image:

`docker run -p 8080:8080 -d --rm --name worktrack worktrack:latest`

This will expose the container port 8080 on the host port 8080. The `-d` flag runs the container in the background and the `--rm` flag cleans up the container after it exits. The `--name` flag names the container (we use this to easily check the logs without copying the container ID).

To observe the application:

`docker logs worktrack`

## Why was this made?
I wanted to learn how to make a HTTP web server in C.
