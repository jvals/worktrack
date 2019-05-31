FROM alpine:latest AS build

RUN apk add build-base
RUN apk add sqlite-dev
RUN apk add sqlite-static

# Enable static linking. The final binary will
# contain everything needed to run, including sqlite.
ENV LDFLAGS -static
COPY . /usr/src/worktrack
WORKDIR /usr/src/worktrack/clogger
RUN ["make"]
WORKDIR /usr/src/worktrack
RUN ["make"]

FROM scratch
COPY --from=build /usr/src/worktrack/server .
COPY --from=build /usr/src/worktrack/routes.ini .
CMD ["./server"]
