#!/bin/bash
cd ../../..
docker rmi -f school21/valgrind:1.0 && docker build . -t school21/valgrind:1.0 -f materials/build/valgrinder_test/Dockerfile && docker run -it school21/valgrind:1.0 
docker rm $(docker ps -aqf status=exited)
