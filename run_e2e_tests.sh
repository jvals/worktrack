#! /usr/bin/env bash

set -e


gracefulShutdown () {
    clearDbAndStopServer $1
    echo "Exiting with code 1"
    exit 1
}


clearDbAndStopServer() {
    echo "Deleting the testing database and stopping the server"
    rm end_to_end_tests.db
    kill $1
}


main () {
    echo "Start the server"
    ./server -d end_to_end_tests.db >/dev/null &
    SERVER_ID=$!

    echo "Wait for the server to startup"
    sleep 5

    echo "Verify that db is empty"
    subject=$(sqlite3 end_to_end_tests.db 'select * from time')
    if [[ "$subject" ]]
    then
        echo "Database was not empty"
        gracefulShutdown $SERVER_ID
    fi

    echo "Check that work in progress is false"
    subject=$(curl -s 127.0.0.1:8080/api/time/work_in_progress)
    if [ "$subject" != "false" ]
    then
        echo "Expected work in progress to be false, was $subject"
        gracefulShutdown $SERVER_ID
    fi

    echo "Start the tracker"
    curl -s -X POST 127.0.0.1:8080/api/time

    echo "Check that there is a new entry in the time table"
    subject=$(sqlite3 end_to_end_tests.db 'select * from time')
    if [[ -z "$subject" ]]
    then
        echo "Database was empty"
        gracefulShutdown $SERVER_ID
    fi

    echo "Check that there are 2 columns in the table (id and fromdate)"
    columns=$(echo "$subject" | sed "s/|$//" | awk -F'|' '{print NF}')
    if [ "$columns" != "2" ]
    then
        echo "Expected number of columns: 2, actual: $columns"
        gracefulShutdown $SERVER_ID
    fi


    echo "Check that work in progress is true"
    subject=$(curl -s 127.0.0.1:8080/api/time/work_in_progress)
    if [ "$subject" != "true" ]
    then
        echo "Expected work in progress to be true, was $subject"
        gracefulShutdown $SERVER_ID
    fi

    echo "Stop the tracker"
    curl -s -X PATCH 127.0.0.1:8080/api/time

    echo "Check that there are 3 columns in the table (id, fromdate and todate)"
    subject=$(sqlite3 end_to_end_tests.db 'select * from time')
    columns=$(echo "$subject" | sed "s/|$//" | awk -F'|' '{print NF}')
    if [ "$columns" != "3" ]
    then
        echo "Expected number of columns: 3, actual: $columns"
        gracefulShutdown $SERVER_ID
    fi

    clearDbAndStopServer $SERVER_ID

    echo "All tests executed successfully"
}


main
