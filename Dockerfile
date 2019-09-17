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
#RUN echo "nobody:x:65534:65534:Nobody:/:" > /etc/passwd

FROM scratch
COPY --from=build /usr/src/worktrack/server .
COPY --from=build /usr/src/worktrack/routes.ini .
COPY --from=build /usr/src/worktrack/index.html .
#COPY --from=build /etc/passwd /etc/passwd
#USER nobody
CMD ["./server -d /var/data/worktrack.db"]
