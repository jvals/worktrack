from gcc:latest
COPY . /usr/src/worktrack
WORKDIR /usr/src/worktrack/clogger
RUN make
WORKDIR /usr/src/worktrack
RUN make
CMD ["./server"]
